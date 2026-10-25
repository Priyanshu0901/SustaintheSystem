param(
    [Parameter(Mandatory = $true)][ValidatePattern('^COM[0-9]+$')][string]$Port,
    [Parameter(Mandatory = $true)][ValidatePattern('^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$')][string]$ExpectedBaseMac,
    [switch]$EraseDevelopmentFlash
)

$ErrorActionPreference = 'Stop'
$python = Join-Path $PSScriptRoot '.tools/esptool/Scripts/python.exe'
$outputDir = Join-Path $PSScriptRoot 'artifacts/xiao-ext-usb'
$manifest = Get-Content -Raw -LiteralPath (Join-Path $outputDir 'manifest.json') | ConvertFrom-Json
$lock = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'upstream.lock.json') | ConvertFrom-Json
if ($manifest.profile -ne 'xiao-ext-usb' -or $manifest.target -ne 'esp32c6' -or
    $manifest.revision -ne $lock.revision -or $manifest.idfImage -ne $lock.idfImage -or
    $manifest.dependencyResolutionOnly) { throw 'Unreviewed or incorrect firmware manifest.' }
foreach ($file in @('factory.bin', 'app.bin', 'sdkconfig', 'dependencies.lock')) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $outputDir $file)).Hash.ToLowerInvariant()
    if ($actual -ne $manifest.files.$file) { throw "Artifact hash mismatch: $file" }
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
$identity = Invoke-EspTool -CommandArgs @('flash-id')
if ($identity -notmatch "BASE MAC:\s+$([regex]::Escape($ExpectedBaseMac))\b" -or
    $identity -notmatch 'Detected flash size: 4MB') { throw 'Board MAC or flash size differs from the identified XIAO.' }
$security = Invoke-EspTool -CommandArgs @('get-security-info')
if ($security -notmatch 'Secure Boot: Disabled' -or $security -notmatch 'Flash Encryption: Disabled') {
    throw 'Secure boot/encryption state is unsuitable for this development flasher.'
}
# No backup is automatic. This board's owner explicitly waived it for development.
if ($EraseDevelopmentFlash) { $null = Invoke-EspTool -CommandArgs @('erase-flash') }
$factory = Join-Path $outputDir 'factory.bin'
# Keep the application stopped until the full factory image is verified.
# Its first boot modifies NVS and OTA selection bytes within the merged image.
$null = Invoke-EspTool -CommandArgs @('--after', 'no-reset', 'write-flash', '0x0', $factory)
$null = Invoke-EspTool -CommandArgs @('verify-flash', '0x0', $factory)
@{
    port = $Port; baseMac = $ExpectedBaseMac.ToLowerInvariant(); chip = 'esp32c6';
    revision = $manifest.revision; profile = $manifest.profile;
    factorySha256 = $manifest.files.'factory.bin'; verified = $true;
    erasedDevelopmentFlash = [bool]$EraseDevelopmentFlash;
    timestamp = (Get-Date).ToUniversalTime().ToString('o')
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $outputDir 'flash-result.json') -Encoding utf8
