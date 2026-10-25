param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^COM[0-9]+$')]
    [string]$Port
)

$ErrorActionPreference = 'Stop'
$nodePath = Join-Path $PSScriptRoot '.runtime/node-v22.23.4-win-x64/node.exe'
$entryPath = Join-Path $PSScriptRoot 'node_modules/zigbee2mqtt/index.js'
if (-not (Test-Path -LiteralPath $nodePath) -or -not (Test-Path -LiteralPath $entryPath)) {
    throw 'Local Node or Zigbee2MQTT is missing. See the coordinator README.'
}
if ([System.IO.Ports.SerialPort]::GetPortNames() -notcontains $Port) {
    throw "Serial port $Port is not currently present. Connect and identify the XIAO first."
}

$dataPath = Join-Path $PSScriptRoot 'data'
$configPath = Join-Path $dataPath 'configuration.yaml'
New-Item -ItemType Directory -Path $dataPath -Force | Out-Null
if (-not (Test-Path -LiteralPath $configPath)) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'configuration.example.yaml') -Destination $configPath
}

$previousData = $env:ZIGBEE2MQTT_DATA
$previousPort = $env:ZIGBEE2MQTT_CONFIG_SERIAL_PORT
$previousGitCeiling = $env:GIT_CEILING_DIRECTORIES
Push-Location $PSScriptRoot
try {
    $env:ZIGBEE2MQTT_DATA = $dataPath
    $env:ZIGBEE2MQTT_CONFIG_SERIAL_PORT = $Port
    # The npm package includes compiled dist/.hash. Prevent its version check
    # from finding the enclosing project's Git commit and attempting a rebuild.
    $env:GIT_CEILING_DIRECTORIES = (Split-Path -Parent $PSScriptRoot).Replace('\', '/')
    & $nodePath $entryPath
    if ($LASTEXITCODE -ne 0) { throw "Zigbee2MQTT exited with code $LASTEXITCODE" }
}
finally {
    $env:ZIGBEE2MQTT_DATA = $previousData
    $env:ZIGBEE2MQTT_CONFIG_SERIAL_PORT = $previousPort
    $env:GIT_CEILING_DIRECTORIES = $previousGitCeiling
    Pop-Location
}
