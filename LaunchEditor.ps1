$env:UE_SDKS_ROOT = Join-Path $PSScriptRoot "AutoSDK"
& "D:\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" (Join-Path $PSScriptRoot "CrooniesTreasureTroop.uproject")
