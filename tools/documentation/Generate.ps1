[CmdletBinding()]
param(
    [string]$DoxygenPath,
    [switch]$InstallTool
)
$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$toolLock = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'tool-lock.json') -Raw | ConvertFrom-Json
$toolRoot = Join-Path $repoRoot ".tools/documentation/doxygen-$($toolLock.doxygenVersion)"
if (-not $DoxygenPath) { $DoxygenPath = Join-Path $toolRoot 'doxygen.exe' }
if ($InstallTool) {
    if (-not $IsWindows) { throw 'Portable installation is Windows-only; supply -DoxygenPath on other systems.' }
    $archivePath = Join-Path $repoRoot ".tools/documentation/doxygen-$($toolLock.doxygenVersion).zip"
    New-Item -ItemType Directory -Force (Split-Path $archivePath) | Out-Null
    if (-not (Test-Path -LiteralPath $archivePath)) {
        Invoke-WebRequest -Uri $toolLock.windowsArchiveUrl -OutFile $archivePath
    }
    if ((Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash -ne $toolLock.windowsArchiveSha256) {
        throw 'Portable Doxygen archive SHA-256 mismatch.'
    }
    Expand-Archive -LiteralPath $archivePath -DestinationPath $toolRoot -Force
}
if (-not (Test-Path -LiteralPath $DoxygenPath)) {
    throw 'Doxygen is missing. Use -InstallTool or supply -DoxygenPath to the pinned version.'
}
$resolvedDoxygen = (Resolve-Path -LiteralPath $DoxygenPath).Path
$version = (& $resolvedDoxygen --version | Out-String).Trim()
if ($LASTEXITCODE -ne 0 -or ($version -split '\s+')[0] -ne $toolLock.doxygenVersion) {
    throw "Expected Doxygen $($toolLock.doxygenVersion); found $version."
}
$outputRoot = Join-Path $repoRoot 'build/documentation'
New-Item -ItemType Directory -Force $outputRoot | Out-Null
$successPath = Join-Path $outputRoot 'generation.json'
Remove-Item -LiteralPath $successPath -ErrorAction SilentlyContinue
Push-Location $repoRoot
try {
    & $resolvedDoxygen (Join-Path $PSScriptRoot 'Doxyfile')
    if ($LASTEXITCODE -ne 0) { throw 'Doxygen failed; inspect build/documentation/warnings.log.' }
    $warningPath = Join-Path $outputRoot 'warnings.log'
    if ((Test-Path -LiteralPath $warningPath) -and (Get-Item -LiteralPath $warningPath).Length -gt 0) {
        throw 'Doxygen emitted warnings; inspect build/documentation/warnings.log.'
    }
    $entryPath = Join-Path $outputRoot 'html/index.html'
    if (-not (Test-Path -LiteralPath $entryPath)) { throw 'Doxygen did not produce an HTML entry page.' }
    @{ doxygen = $version; configurationSha256 = (Get-FileHash -LiteralPath (Join-Path $PSScriptRoot 'Doxyfile')).Hash; entry = 'html/index.html' } |
        ConvertTo-Json | Set-Content -LiteralPath $successPath -Encoding utf8
    Write-Output "Generated code reference: $entryPath"
} finally {
    Pop-Location
}
