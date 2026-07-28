param(
    [Parameter(Mandatory=$true)]
    [string]$Version,

    [string]$Notes = "Croonies patch build",
    [switch]$SkipBuild,
    [switch]$SkipImport,
    [switch]$NoZip
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot "CrooniesTreasureTroop.uproject"
$EngineRoot = "D:\Epic Games\UE_5.5"
$UAT = Join-Path $EngineRoot "Engine\Build\BatchFiles\RunUAT.bat"
$EditorCmd = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$UbtArgs = "-CompilerVersion=14.44.35228"
$ImportScript = Join-Path $ProjectRoot "Scripts\ImportIconImages.py"
$PackageRoot = Join-Path $ProjectRoot "Packaged"
$PackagedWindows = Join-Path $PackageRoot "Windows"
$ReleaseRoot = Join-Path $ProjectRoot "Releases"
$ReleaseName = "CrooniesTreasureTroop-$Version"
$ReleaseDir = Join-Path $ReleaseRoot $ReleaseName
$BuildOutputDir = Join-Path $ReleaseDir "Windows"
$ManifestPath = Join-Path $ReleaseDir "patch-manifest.json"
$LatestPath = Join-Path $ReleaseRoot "latest.json"
$ZipPath = Join-Path $ReleaseRoot "$ReleaseName.zip"

function Assert-Exists([string]$Path, [string]$Label) {
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "$Label not found: $Path"
    }
}

function Invoke-Step([string]$Name, [scriptblock]$Action) {
    Write-Host "`n== $Name ==" -ForegroundColor Cyan
    & $Action
}

Assert-Exists $ProjectFile "Project file"
Assert-Exists $UAT "Unreal Automation Tool"
Assert-Exists $EditorCmd "UnrealEditor-Cmd"

$env:UE_SDKS_ROOT = Join-Path $ProjectRoot "AutoSDK"
New-Item -ItemType Directory -Path $ReleaseRoot -Force | Out-Null

if (-not $SkipImport) {
    Invoke-Step "Import UI/icon images" {
        Assert-Exists $ImportScript "Icon import script"
        & $EditorCmd $ProjectFile -run=pythonscript "-script=$ImportScript" -unattended -nop4 -nosplash
        if ($LASTEXITCODE -ne 0) { throw "Image import failed with exit code $LASTEXITCODE" }
    }
}

if (-not $SkipBuild) {
    Invoke-Step "Build, cook, stage, pak, and archive Windows package" {
        & $UAT BuildCookRun "-project=$ProjectFile" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive "-archivedirectory=$PackageRoot" "-ubtargs=$UbtArgs"
        if ($LASTEXITCODE -ne 0) { throw "Packaging failed with exit code $LASTEXITCODE" }
    }
}

Assert-Exists $PackagedWindows "Packaged Windows build"

Invoke-Step "Create release folder" {
    if (Test-Path -LiteralPath $ReleaseDir) {
        $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
        $backup = "$ReleaseDir.backup-$stamp"
        Move-Item -LiteralPath $ReleaseDir -Destination $backup
        Write-Host "Existing release moved to $backup"
    }
    New-Item -ItemType Directory -Path $BuildOutputDir -Force | Out-Null
    Copy-Item -Path (Join-Path $PackagedWindows "*") -Destination $BuildOutputDir -Recurse -Force
}

Invoke-Step "Generate patch manifest" {
    $files = Get-ChildItem -LiteralPath $BuildOutputDir -Recurse -File | Sort-Object FullName | ForEach-Object {
        $relative = $_.FullName.Substring($BuildOutputDir.Length).TrimStart('\') -replace '\\','/'
        $hash = Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256
        [ordered]@{
            path = $relative
            size = $_.Length
            sha256 = $hash.Hash.ToLowerInvariant()
        }
    }

    $manifest = [ordered]@{
        game = "Croonies: Treasure Troop"
        packageId = "CrooniesTreasureTroop"
        version = $Version
        notes = $Notes
        createdUtc = (Get-Date).ToUniversalTime().ToString("o")
        platform = "Windows"
        entrypoint = "CrooniesTreasureTroop.exe"
        fileCount = @($files).Count
        files = @($files)
    }

    $manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $ManifestPath -Encoding UTF8

    $latest = [ordered]@{
        game = "Croonies: Treasure Troop"
        packageId = "CrooniesTreasureTroop"
        latestVersion = $Version
        platform = "Windows"
        zipFile = if ($NoZip) { $null } else { Split-Path -Leaf $ZipPath }
        manifestFile = "patch-manifest.json"
        notes = $Notes
        updatedUtc = (Get-Date).ToUniversalTime().ToString("o")
    }
    $latest | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $LatestPath -Encoding UTF8
}

if (-not $NoZip) {
    Invoke-Step "Create patch zip" {
        if (Test-Path -LiteralPath $ZipPath) { Remove-Item -LiteralPath $ZipPath -Force }
        Compress-Archive -Path (Join-Path $ReleaseDir "*") -DestinationPath $ZipPath -Force
    }
}

Write-Host "`nPatch build complete." -ForegroundColor Green
Write-Host "Release folder: $ReleaseDir"
if (-not $NoZip) { Write-Host "Patch zip:      $ZipPath" }
Write-Host "Manifest:       $ManifestPath"


