# Croonies Patch Pipeline

This project now has a reusable patch/update workflow so we do not have to manually fight Unreal packaging every time.

## One-command patch build

From `F:\gooniesgame`, run:

```powershell
.\BuildPatch.ps1 -Version "0.1.1" -Notes "Settings/login polish"
```

That command will:

- Reimport images from `F:\gooniesgame\icon images`
- Build/cook/package the Windows game with Unreal Automation Tool
- Copy the packaged build into `F:\gooniesgame\Releases\CrooniesTreasureTroop-<version>`
- Generate a SHA256 `patch-manifest.json`
- Generate/update `F:\gooniesgame\Releases\latest.json`
- Create a zip package named `CrooniesTreasureTroop-<version>.zip`

## Faster patch from an already packaged build

If the packaged build already exists and you only want to create a release zip/manifest:

```powershell
.\BuildPatch.ps1 -Version "0.1.1" -Notes "Quick release" -SkipBuild -SkipImport
```

## Apply a patch zip

To install a patch zip into a game install folder:

```powershell
.\Scripts\ApplyGamePatch.ps1 -PackageZip "F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.1.zip" -InstallPath "F:\CrooniesInstall"
```

The installer verifies every file hash before copying. By default it also makes a timestamped backup of the existing install folder.

## Recommended version style

Use semantic versions while we are building:

- `0.1.0` first playable prototype
- `0.1.1` UI/bugfix patch
- `0.2.0` bigger gameplay/content update
- `1.0.0` first complete release candidate

## Current limits

This is a local patch pipeline, not a live app-store updater. For Google Play or Apple App Store releases, the final patch/update path still goes through their store build and signing systems. This setup is meant to make our local Windows testing and distributable builds painless while the game is still changing fast.
