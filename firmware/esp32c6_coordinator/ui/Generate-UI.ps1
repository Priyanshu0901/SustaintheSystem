param([string]$EezExecutable = (Join-Path $PSScriptRoot '../.tools/eez/app/EEZ Studio.exe'))
$ErrorActionPreference = 'Stop'
function Get-CanonicalTextHash([string]$Path) {
    $bytes = [Text.Encoding]::UTF8.GetBytes([IO.File]::ReadAllText($Path).Replace("`r`n", "`n"))
    return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($bytes)).ToLowerInvariant()
}
$lock = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'lvgl.lock.json') | ConvertFrom-Json
if (-not (Test-Path -LiteralPath $EezExecutable)) { throw 'Supply -EezExecutable pointing to EEZ Studio 0.29.0; see ui/README.md.' }
$version = (Get-Item -LiteralPath $EezExecutable).VersionInfo.FileVersion
if ($version -ne $lock.generator.version) { throw "Expected EEZ $($lock.generator.version), found $version" }
$project = Join-Path $PSScriptRoot 'editor/coordinator.eez-project'
$output = Join-Path $PSScriptRoot 'generated'
$build = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../build'))
New-Item -ItemType Directory -Path $build -Force | Out-Null
$log = Join-Path $build 'eez-generate.log'
$process = Start-Process -FilePath $EezExecutable -ArgumentList @('--build-project',('"' + $project + '"')) -WindowStyle Hidden -PassThru -RedirectStandardOutput $log -RedirectStandardError (Join-Path $build 'eez-generate-error.log')
if (-not $process.WaitForExit(30000)) { throw "EEZ generation exceeded 30 seconds; inspect task process $($process.Id) and $log" }
if ($process.ExitCode -ne 0 -or -not (Select-String -LiteralPath $log -Pattern 'Build successfully finished' -Quiet) -or
    -not (Select-String -LiteralPath $log -Pattern 'No error and no warning detected' -Quiet)) {
    Get-Content -LiteralPath $log -Tail 25
    throw 'EEZ generation failed; do not hand-edit generated files.'
}
$hashes = [ordered]@{}
Get-ChildItem -LiteralPath $output -File | Where-Object Extension -In '.c','.h' | Sort-Object Name | ForEach-Object {
    $hashes[$_.Name] = Get-CanonicalTextHash $_.FullName
}
@{
    generator = $lock.generator.name; version = $lock.generator.version;
    editorApiVersion = $lock.generator.editorApiVersion; firmwareApiVersion = $lock.lvgl.version;
    flowSupport = $false;
    hashNormalization = 'UTF-8 text with LF';
    projectSha256 = Get-CanonicalTextHash $project;
    files = $hashes
} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'generation.json') -Encoding utf8NoBOM
Write-Host "Verified EEZ $version generation for LVGL $($lock.lvgl.version)"
