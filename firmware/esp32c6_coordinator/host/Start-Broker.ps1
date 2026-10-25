# Run in a separate PowerShell terminal; Ctrl+C stops the development broker.
$ErrorActionPreference = 'Stop'
$brokerPath = Join-Path $PSScriptRoot '.runtime/mosquitto/mosquitto.exe'
if (-not (Test-Path -LiteralPath $brokerPath)) {
    throw 'Portable Mosquitto is missing. See the coordinator README.'
}
& $brokerPath -c (Join-Path $PSScriptRoot 'mosquitto.conf') -v
if ($LASTEXITCODE -ne 0) { throw "Mosquitto exited with code $LASTEXITCODE" }
