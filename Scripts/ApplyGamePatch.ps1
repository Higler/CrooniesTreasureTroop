param(
    [Parameter(Mandatory=$true)]
    [string]$PackageZip,

    [Parameter(Mandatory=$true)]
    [string]$InstallPath,

    [switch]$NoBackup
)

$ErrorActionPreference = "Stop"

function Assert-Exists([string]$Path, [string]$Label) {
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "$Label not found: $Path"
    }
}

function Copy-DirectoryContents([string]$Source, [string]$Destination) {
    New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    Copy-Item -Path (Join-Path $Source "*") -Destination $Destination -Recurse -Force
}

$PackageZip = (Resolve-Path -LiteralPath $PackageZip).Path
Assert-Exists $PackageZip "Patch zip"

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("CrooniesPatch-" + [guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Path $tempRoot -Force | Out-Null

try {
    Write-Host "Extracting patch..." -ForegroundColor Cyan
    Expand-Archive -LiteralPath $PackageZip -DestinationPath $tempRoot -Force

    $manifest = Get-ChildItem -LiteralPath $tempRoot -Recurse -File -Filter "patch-manifest.json" | Select-Object -First 1
    if (-not $manifest) { throw "Patch manifest missing from zip." }

    $releaseDir = Split-Path -Parent $manifest.FullName
    $payloadDir = Join-Path $releaseDir "Windows"
    Assert-Exists $payloadDir "Windows payload folder"

    $data = Get-Content -LiteralPath $manifest.FullName -Raw | ConvertFrom-Json
    Write-Host "Verifying $($data.game) version $($data.version)..." -ForegroundColor Cyan

    foreach ($file in $data.files) {
        $full = Join-Path $payloadDir ($file.path -replace '/', '\')
        Assert-Exists $full "Payload file"
        $actual = (Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($actual -ne $file.sha256) {
            throw "Hash mismatch for $($file.path)"
        }
    }

    if ((Test-Path -LiteralPath $InstallPath) -and -not $NoBackup) {
        $backup = "$InstallPath.backup-$(Get-Date -Format 'yyyyMMdd-HHmmss')"
        Write-Host "Backing up current install to $backup" -ForegroundColor Cyan
        Copy-DirectoryContents $InstallPath $backup
    }

    Write-Host "Installing patch to $InstallPath" -ForegroundColor Cyan
    Copy-DirectoryContents $payloadDir $InstallPath

    $installedManifest = Join-Path $InstallPath "patch-manifest.json"
    Copy-Item -LiteralPath $manifest.FullName -Destination $installedManifest -Force

    Write-Host "Patch installed successfully: version $($data.version)" -ForegroundColor Green
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}

