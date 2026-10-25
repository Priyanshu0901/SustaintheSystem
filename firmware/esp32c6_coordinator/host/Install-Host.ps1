# Prepare portable Windows tools. Does not install services or change system PATH.
$ErrorActionPreference = 'Stop'
$versions = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'tool-versions.json') -Raw | ConvertFrom-Json
$runtimePath = Join-Path $PSScriptRoot '.runtime'
$downloadPath = Join-Path $runtimePath 'downloads'
New-Item -ItemType Directory -Path $downloadPath -Force | Out-Null

function Get-VerifiedDownload($Url, $Destination, $ExpectedHash) {
    if (-not (Test-Path -LiteralPath $Destination)) {
        Invoke-WebRequest -Uri $Url -OutFile $Destination
    }
    $actualHash = (Get-FileHash -LiteralPath $Destination -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actualHash -ne $ExpectedHash) {
        throw "Checksum mismatch for $Destination. Inspect the file before retrying."
    }
}

$nodeDirectory = Join-Path $runtimePath "node-v$($versions.node.version)-win-x64"
$nodeArchive = Join-Path $downloadPath "node-v$($versions.node.version)-win-x64.zip"
Get-VerifiedDownload $versions.node.url $nodeArchive $versions.node.sha256
if (-not (Test-Path -LiteralPath (Join-Path $nodeDirectory 'node.exe'))) {
    Expand-Archive -LiteralPath $nodeArchive -DestinationPath $runtimePath
}

$brokerDirectory = Join-Path $runtimePath 'mosquitto'
$brokerArchive = Join-Path $downloadPath "mosquitto-$($versions.mosquitto.version)-install-windows-x64.exe"
Get-VerifiedDownload $versions.mosquitto.url $brokerArchive $versions.mosquitto.sha256
if (-not (Test-Path -LiteralPath (Join-Path $brokerDirectory 'mosquitto.exe'))) {
    $sevenZip = Get-Command 7z.exe -ErrorAction SilentlyContinue
    $sevenZipPath = if ($sevenZip) { $sevenZip.Source } else { Join-Path $env:ProgramFiles '7-Zip/7z.exe' }
    if (-not (Test-Path -LiteralPath $sevenZipPath)) { throw '7-Zip is required to extract portable Mosquitto.' }
    & $sevenZipPath x $brokerArchive "-o$brokerDirectory" '-y'
    if ($LASTEXITCODE -ne 0) { throw 'Mosquitto extraction failed.' }
}

$previousPath = $env:PATH
Push-Location $PSScriptRoot
try {
    $env:PATH = $nodeDirectory + ';' + $previousPath
    & (Join-Path $nodeDirectory 'npm.cmd') ci --ignore-scripts --no-audit --no-fund --cache (Join-Path $runtimePath 'npm-cache')
    if ($LASTEXITCODE -ne 0) { throw 'Zigbee2MQTT dependency installation failed.' }
}
finally {
    $env:PATH = $previousPath
    Pop-Location
}
