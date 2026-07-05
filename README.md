# Croonies: Treasure Troop

Original 2.5D/top-down treasure adventure prototype built in Unreal Engine 5.5.

## Play

Run `LaunchEditor.ps1`, open `CoveRuntimeMap`, and press Play.

Controls:
- Mouse click / touch: move to destination
- Hold mouse / touch: steer toward pointer
- WASD / arrow keys: direct movement

Win condition:
- Recruit the 8-kid crew: Scout, Joker, Inventor, Strong, Swimmer, Flashlight, Rope, and Smart
- Clear the story level markers from the neighborhood through the final shipwreck
- Collect 12 pirate artifacts, 8 map pieces, and 8 friendship tokens
- Open the final shipwreck vault and recover the treasure

World structure:
- Hub: The Neighborhood, with houses, workshop, treehouse, lighthouse view, upgrades, and level markers
- Levels 1-4: attic/lighthouse/tunnel/mine exploration
- Levels 5-8: bat, crystal, mirror, and pirate outpost puzzle dungeons
- Levels 9-11: chasm, river, and neighborhood chase sequences
- Level 12: final shipwreck boss-puzzle vault
- Bonus 101-104: treehouse, ghost ship trial, endless caverns, and neighborhood mayhem challenge zones

Character switching:
- Press `1` for Scout
- Press `2` for Joker
- Press `3` for Inventor
- Press `4` for Strong
- Press `5` for Swimmer
- Press `6` for Flashlight
- Press `7` for Rope
- Press `8` for Smart

## Notes

The project is intentionally original and does not use direct names, likenesses, or story elements from any existing film. The first version uses Unreal built-in meshes and generated gameplay actors so it is playable before marketplace art is added.

`UE_SDKS_ROOT` is pointed at the local `AutoSDK` folder because the installed Visual Studio Community toolchain is missing its C++ standard library headers. The local AutoSDK mirrors the complete Build Tools compiler installed during setup.

## Build patches

Use `.\BuildPatch.ps1 -Version "0.1.1" -Notes "Patch notes here"` to create a packaged release, patch manifest, and zip. See `PATCHING.md` for the full workflow.
