param(
    [switch]$ResolveDependencies
)

$ErrorActionPreference = 'Stop'
$lock = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'upstream.lock.json') | ConvertFrom-Json
$sourcePath = Join-Path $PSScriptRoot 'source'
if (-not (Test-Path -LiteralPath (Join-Path $sourcePath '.git'))) {
    & git clone $lock.fork $sourcePath
    if ($LASTEXITCODE -ne 0) { throw 'Fork clone failed.' }
    & git -C $sourcePath remote add upstream $lock.upstream
    if ($LASTEXITCODE -ne 0) { throw 'Cannot register upstream remote.' }
    & git -C $sourcePath checkout --detach $lock.revision
    if ($LASTEXITCODE -ne 0) { throw 'Pinned source revision is missing.' }
}
$origin = & git -C $sourcePath remote get-url origin
$upstream = & git -C $sourcePath remote get-url upstream
if ($origin -ne $lock.fork -or $upstream -ne $lock.upstream) { throw 'Unexpected fork/upstream remotes.' }
$revision = & git -C $sourcePath rev-parse HEAD
if ($revision -ne $lock.revision) { throw 'Checkout differs from upstream.lock.json. Review and update the lock deliberately.' }
$changes = & git -C $sourcePath status --porcelain
if ($LASTEXITCODE -ne 0 -or $changes) { throw 'Source checkout must be clean; builds use committed source only.' }
if (-not $ResolveDependencies -and -not (Test-Path -LiteralPath (Join-Path $PSScriptRoot 'dependencies.lock'))) {
    throw 'Dependency lock missing. Maintainers must resolve and review it first; see README.'
}
& docker info --format '{{.OSType}}' | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'Docker Linux engine is unavailable. Start Docker Desktop and retry.' }
$archiveDir = Join-Path $PSScriptRoot 'build'
$outputDir = Join-Path $PSScriptRoot 'artifacts'
New-Item -ItemType Directory -Path $archiveDir, $outputDir -Force | Out-Null
$archivePath = Join-Path $archiveDir 'source.tar'
$uiLock = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'ui/lvgl.lock.json') | ConvertFrom-Json
$dependencyDir = Join-Path $archiveDir 'dependencies'
New-Item -ItemType Directory -Path $dependencyDir -Force | Out-Null
$lvglArchive = Join-Path $dependencyDir $uiLock.lvgl.archive
if (-not (Test-Path -LiteralPath $lvglArchive)) {
    Invoke-WebRequest -Uri $uiLock.lvgl.source -OutFile $lvglArchive
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $lvglArchive).Hash.ToLowerInvariant() -ne $uiLock.lvgl.sha256) {
    throw 'LVGL archive differs from the pinned release hash.'
}
& git -C $sourcePath archive --format=tar "--output=$archivePath" $revision
if ($LASTEXITCODE -ne 0) { throw 'Cannot archive pinned source.' }
$dockerArgs = @(
    'run', '--rm', '--platform', $lock.platform,
    '--mount', "type=bind,source=$PSScriptRoot,target=/integration,readonly",
    '--mount', "type=bind,source=$outputDir,target=/out",
    '--mount', 'type=volume,source=sustain-c6-waveshare-touch-usb,target=/work',
    '--env', "SOURCE_DATE_EPOCH=$($lock.sourceDateEpoch)",
    $lock.idfImage, 'python', '/integration/tools/build.py'
)
if ($ResolveDependencies) { $dockerArgs += '--resolve-dependencies' }
& docker @dockerArgs
if ($LASTEXITCODE -ne 0) { throw 'Waveshare firmware build failed. Do not flash old artifacts.' }
Write-Host "Verified build artifacts: $outputDir"
