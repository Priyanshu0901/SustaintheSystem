param(
    [ValidateSet('router', 'sleepy')][string]$Role = 'router',
    [ValidateSet('xiao', 'dk-a', 'dk-b')][string]$Board = 'xiao',
    [string]$SdkRoot = 'C:/ncs/v3.4.1',
    [string]$ToolchainRoot = 'C:/ncs/toolchains/4f5b6ad6dd',
    [string]$SeeedRoot = 'C:/Users/rayv_mini_pc/Documents/platform-seeedboards/zephyr',
    [string]$ZigbeeRoot = "$PSScriptRoot/.deps/zigbee"
)
$ErrorActionPreference = 'Stop'
$savedEnvironment = @{}
foreach ($entry in @('PATH','PYTHONPATH','ZEPHYR_BASE','ZEPHYR_SDK_INSTALL_DIR','ZEPHYR_TOOLCHAIN_VARIANT')) {
    $savedEnvironment[$entry] = [Environment]::GetEnvironmentVariable($entry)
}
try {
    $lock = Get-Content "$PSScriptRoot/build-lock.json" -Raw | ConvertFrom-Json
    $nativeManifest = Get-Content "$ToolchainRoot/manifest.json" -Raw | ConvertFrom-Json
    if ($nativeManifest.bundle_id -ne $lock.native_toolchain_bundle) {
        throw 'Native toolchain bundle differs from build-lock.json'
    }
    $toolchain = Get-Content "$ToolchainRoot/environment.json" -Raw | ConvertFrom-Json
    foreach ($entry in $toolchain.env_vars) {
        if ($entry.key -eq 'NRFUTIL_HOME') { continue }
        $value = if ($entry.type -eq 'relative_paths') {
            ($entry.values | ForEach-Object { Join-Path $ToolchainRoot $_ }) -join [IO.Path]::PathSeparator
        } else { $entry.value }
        if ($entry.existing_value_treatment -eq 'prepend_to') {
            $value += [IO.Path]::PathSeparator + [Environment]::GetEnvironmentVariable($entry.key)
        }
        [Environment]::SetEnvironmentVariable($entry.key, $value)
    }
    $env:ZEPHYR_BASE = "$SdkRoot/zephyr"
    & "$ToolchainRoot/opt/bin/python.exe" "$PSScriptRoot/tools/verify_inputs.py" $SdkRoot $SeeedRoot $ZigbeeRoot
    if ($LASTEXITCODE -ne 0) { throw 'Pinned input verification failed' }
    $target = switch ($Board) {
        'xiao' { 'xiao_nrf54lm20a/nrf54lm20a/cpuapp' }
        'dk-a' { 'nrf54lm20dk/nrf54lm20a/cpuapp' }
        'dk-b' { 'nrf54lm20dk/nrf54lm20b/cpuapp' }
    }
    $boardProfile = if ($Board -eq 'xiao') { 'xiao' } else { 'dk' }
    $buildDir = "$PSScriptRoot/build/native-$Board-$Role"
    $cmakeArgs = @('-DUSE_CCACHE=OFF', '-DCMAKE_OBJECT_PATH_MAX=180', "-DZEPHYR_EXTRA_MODULES=$ZigbeeRoot", "-DEXTRA_CONF_FILE=$PSScriptRoot/profiles/$Role.conf;$PSScriptRoot/profiles/$boardProfile.conf")
    if ($Board -eq 'xiao') { $cmakeArgs += "-DBOARD_ROOT=$SeeedRoot" }
    Remove-Item -LiteralPath "$buildDir/manifest.json" -ErrorAction SilentlyContinue
    & "$ToolchainRoot/opt/bin/python.exe" -m west build -p always --no-sysbuild --build-opt=-j4 -b $target -d $buildDir $PSScriptRoot -- @cmakeArgs
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $LASTEXITCODE" }
    & "$ToolchainRoot/opt/bin/python.exe" "$PSScriptRoot/tools/check_build.py" $buildDir $Board $Role
    if ($LASTEXITCODE -ne 0) { throw 'Effective build validation failed' }
} finally {
    foreach ($entry in $savedEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($entry, $savedEnvironment[$entry])
    }
}
