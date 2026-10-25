param(
    [Parameter(Mandatory)][ValidatePattern('^[A-Za-z0-9_-]+$')][string]$ProbeSerial,
    [ValidateSet('router','sleepy')][string]$Role = 'sleepy',
    [string]$OpenOcd = 'C:/msys64/ucrt64/bin/openocd.exe',
    [string]$OpenOcdScripts = 'C:/msys64/ucrt64/share/openocd/scripts',
    [string]$SeeedRoot = 'C:/Users/rayv_mini_pc/Documents/platform-seeedboards/zephyr'
)
$ErrorActionPreference = 'Stop'
$build = Join-Path $PSScriptRoot "build/native-xiao-$Role"
$manifest = Get-Content "$build/manifest.json" -Raw | ConvertFrom-Json
if ($manifest.board -ne 'xiao' -or $manifest.role -ne $Role) { throw 'Wrong artifact board/role' }
foreach ($name in @('zephyr.hex','zephyr.bin')) {
    if ((Get-FileHash "$build/zephyr/$name" -Algorithm SHA256).Hash.ToLowerInvariant() -ne $manifest.output_sha256.$name) {
        throw "Artifact hash mismatch: $name"
    }
}
$boardConfig = "$SeeedRoot/boards/arm/xiao_nrf54lm20a/support/openocd.cfg".Replace('\','/')
$image = "$build/zephyr/zephyr.hex".Replace('\','/')
$readback = "$build/readback.bin".Replace('\','/')
$imageSize = (Get-Item "$build/zephyr/zephyr.bin").Length
# Override Seeed's examine-fail handler before init: never auto mass-erase/recover.
# Avoid target-resident CRC verification: OpenOCD 0.12.0's CRC algorithm faulted
# on this LM20A. Full readback comparison does not run code in target SRAM.
& $OpenOcd -s $OpenOcdScripts -f $boardConfig `
    -c 'nrf54lm20a.cpu configure -event examine-fail {}' `
    -c "adapter serial $ProbeSerial" `
    -c 'gdb_port disabled; tcl_port disabled; telnet_port disabled' `
    -c 'init' `
    -c 'if {[lindex [read_memory 0x00ffc31c 32 1] 0] != 0x054bc20a} {error {Expected nRF54LM20A}}' `
    -c 'halt' -c "nrf54lm20a-load {$image}" `
    -c "dump_image {$readback} 0 $imageSize" -c 'shutdown'
if ($LASTEXITCODE -ne 0) { throw 'Programming/readback failed; target may remain halted' }
$expectedHash = $manifest.output_sha256.'zephyr.bin'
if ((Get-FileHash -LiteralPath $readback -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash) {
    throw 'Full application readback mismatch; target remains halted'
}
Write-Output "Verified all $imageSize application bytes; SHA256=$expectedHash"
& $OpenOcd -s $OpenOcdScripts -f $boardConfig `
    -c 'nrf54lm20a.cpu configure -event examine-fail {}' `
    -c "adapter serial $ProbeSerial" `
    -c 'gdb_port disabled; tcl_port disabled; telnet_port disabled' `
    -c 'init' -c 'reset run' -c 'shutdown'
if ($LASTEXITCODE -ne 0) { throw 'Verified image written, but reset failed' }
