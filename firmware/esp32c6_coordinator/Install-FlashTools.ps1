param([Parameter(Mandatory = $true)][string]$Python)
$ErrorActionPreference = 'Stop'
$venv = Join-Path $PSScriptRoot '.tools/esptool'
& $Python -m venv $venv
if ($LASTEXITCODE -ne 0) { throw 'Cannot create the local Python environment.' }
& (Join-Path $venv 'Scripts/python.exe') -m pip install -r (Join-Path $PSScriptRoot 'tools/requirements-flash.txt')
if ($LASTEXITCODE -ne 0) { throw 'Flash tool installation failed.' }
