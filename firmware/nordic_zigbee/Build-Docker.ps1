param(
    [ValidateSet('router','sleepy')][string]$Role = 'router',
    [ValidateSet('xiao','dk-a','dk-b')][string]$Board = 'xiao'
)
$ErrorActionPreference = 'Stop'
$lock = Get-Content "$PSScriptRoot/build-lock.json" -Raw | ConvertFrom-Json
$outputDir = Join-Path $PSScriptRoot "artifacts/$Board-$Role"
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
# A failed rebuild must never leave an old success marker.
Remove-Item -LiteralPath (Join-Path $outputDir 'manifest.json') -ErrorAction SilentlyContinue
& docker run --rm --platform linux/amd64 `
    --mount "type=bind,source=$PSScriptRoot,target=/app,readonly" `
    --mount 'type=volume,source=sustain-nordic-sdk-341,target=/work' `
    --mount "type=bind,source=$outputDir,target=/out" `
    $lock.docker_image python3 /app/tools/build_container.py $Board $Role
if ($LASTEXITCODE -ne 0) { throw "Docker build failed: $LASTEXITCODE" }
