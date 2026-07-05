param(
    [Parameter(Mandatory=$true)]
    [string]$Version,

    [string]$Notes = "Croonies patch build",
    [switch]$SkipBuild,
    [switch]$SkipImport,
    [switch]$NoZip
)

& (Join-Path $PSScriptRoot "Scripts\BuildGamePatch.ps1") -Version $Version -Notes $Notes -SkipBuild:$SkipBuild -SkipImport:$SkipImport -NoZip:$NoZip
