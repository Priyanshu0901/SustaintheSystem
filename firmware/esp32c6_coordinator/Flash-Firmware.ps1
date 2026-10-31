param(
    [Parameter(Mandatory = $true)][ValidatePattern('^COM[0-9]+$')][string]$Port,
    [Parameter(Mandatory = $true)][ValidatePattern('^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$')][string]$ExpectedBaseMac,
    [switch]$AppOnly,
    [switch]$EraseDevelopmentFlash
)

$ErrorActionPreference = 'Stop'
if ($AppOnly -and $EraseDevelopmentFlash) { throw 'App-only updates cannot erase development flash.' }
$python = Join-Path $PSScriptRoot '.tools/esptool/Scripts/python.exe'
$outputDir = Join-Path $PSScriptRoot 'artifacts'
$manifest = Get-Content -Raw -LiteralPath (Join-Path $outputDir 'manifest.json') | ConvertFrom-Json
$lock = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'upstream.lock.json') | ConvertFrom-Json
if ($manifest.board -ne 'waveshare-touch-1.9' -or $manifest.target -ne 'esp32c6' -or
    $manifest.revision -ne $lock.revision -or $manifest.idfImage -ne $lock.idfImage -or
    $manifest.dependencyResolutionOnly) { throw 'Unreviewed or incorrect firmware manifest.' }
foreach ($file in @('factory.bin', 'app.bin', 'sdkconfig', 'dependencies.lock', 'sbom.cdx.json')) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $outputDir $file)).Hash.ToLowerInvariant()
    if ($actual -ne $manifest.files.$file) { throw "Artifact hash mismatch: $file" }
}
if ($AppOnly) {
    if ($manifest.applicationOffset -ne '0x20000') { throw 'Application update requires the reviewed partition layout at 0x20000.' }
    $flashArgsHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $outputDir 'flasher_args.json')).Hash.ToLowerInvariant()
    if ($flashArgsHash -ne $manifest.files.'flasher_args.json') { throw 'Flash layout hash mismatch.' }
}
foreach ($file in $manifest.localSourceFiles.PSObject.Properties) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $PSScriptRoot $file.Name)).Hash.ToLowerInvariant()
    if ($actual -ne $file.Value) { throw "Local UI/board source differs from build: $($file.Name)" }
}
$dependencyHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $PSScriptRoot 'dependencies.lock')).Hash.ToLowerInvariant()
if ($dependencyHash -ne $manifest.files.'dependencies.lock') { throw 'Build uses a different dependency lock.' }
if ([System.IO.Ports.SerialPort]::GetPortNames() -notcontains $Port) { throw "Port not present: $Port" }
function Invoke-EspTool {
    param([string[]]$CommandArgs)
    $result = & $python -m esptool --chip esp32c6 --port $Port @CommandArgs 2>&1
    if ($LASTEXITCODE -ne 0) { throw ($result -join "`n") }
    $text = $result -join "`n"
    Write-Host $text
    return $text
}
$flashMb = 8
if ($manifest.flashSizeBytes -ne ($flashMb * 1024 * 1024)) { throw 'Manifest flash size differs from Waveshare board.' }
$identity = Invoke-EspTool -CommandArgs @('flash-id')
if ($identity -notmatch "BASE MAC:\s+$([regex]::Escape($ExpectedBaseMac))\b" -or
    $identity -notmatch "Detected flash size: $($flashMb)MB") { throw 'Board MAC or flash size differs from Waveshare.' }
$security = Invoke-EspTool -CommandArgs @('get-security-info')
if ($security -notmatch 'Secure Boot: Disabled' -or $security -notmatch 'Flash Encryption: Disabled') {
    throw 'Secure boot/encryption state is unsuitable for this development flasher.'
}
# No backup is automatic. This board's owner explicitly waived it for development.
if ($EraseDevelopmentFlash) { $null = Invoke-EspTool -CommandArgs @('erase-flash') }
$factory = Join-Path $outputDir 'factory.bin'
$writeImage = if ($AppOnly) { Join-Path $outputDir 'app.bin' } else { $factory }
$writeOffset = if ($AppOnly) { $manifest.applicationOffset } else { '0x0' }
# Keep the application stopped until the full factory image is verified.
# Its first boot modifies NVS and OTA selection bytes within the merged image.
$null = Invoke-EspTool -CommandArgs @('--after', 'no-reset', 'write-flash', $writeOffset, $writeImage)
$null = Invoke-EspTool -CommandArgs @('verify-flash', $writeOffset, $writeImage)
@{
    port = $Port; baseMac = $ExpectedBaseMac.ToLowerInvariant(); chip = 'esp32c6';
    revision = $manifest.revision; board = $manifest.board;
    factorySha256 = $manifest.files.'factory.bin'; verified = $true;
    applicationOnly = [bool]$AppOnly; programmedOffset = $writeOffset;
    erasedDevelopmentFlash = [bool]$EraseDevelopmentFlash;
    timestamp = (Get-Date).ToUniversalTime().ToString('o')
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $outputDir 'flash-result.json') -Encoding utf8
