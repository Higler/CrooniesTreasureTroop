# Croonies: Treasure Troop Design Notes

## Level 1 - The Old Attic

- Current state: contained room is working better with one solid floor and connected walls.
- Follow-up: make the attic slightly larger and improve tutorial function/pacing after the other levels are sealed.

## 2026-06-12 - Warning Cleanup and Hub Polish

- Release 0.1.66 built from UE 5.5 at `D:\Epic Games\UE_5.5`.
- Build scripts now point at the current engine path and force the working VS2022 14.44 toolchain.
- Packaged runtime log confirms the prior `ted_boy` shader warning, missing `CartoonStyleHouse` references, and null input mapping warning are gone.
- Neighborhood hub now has a black road segment between the old attic and task board with sidewalks on both sides and center striping.
- Old Attic tutorial room is slightly larger for smoother first-room pacing.

## 2026-06-12 - Old Attic Shelf Containment

- Release 0.1.67 adds blocking walls to both raised shelf ends and a guarded front edge with a single ladder opening.
- The raised shelf should no longer let players jump out of the attic and fall endlessly.
- Replaced the broken line-style ladder visuals with solid procedural wooden rails and rungs while keeping the walkable collision steps.
- Packaged `0.1.67` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Alignment

- Release 0.1.68 shifts the raised shelf left so its left edge aligns with the attic wall instead of sitting offset in the room.
- The shelf is widened and deepened so shelf map pieces sit on the platform and the ladder opening remains the only intended entry/exit point.
- Guard walls, shelf end walls, front lip, back wall, and solid ladder pieces now derive from the same shelf edge constants.
- Packaged `0.1.68` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Map Fragment Containment

- Release 0.1.69 raises the shelf end, front, and back blockers above jump height so players should not be able to hop out of the attic from the shelf.
- Shelf map fragments now sit in shelf-relative blocker pens with one ladder-facing entry instead of floating near unguarded shelf edges.
- The attic floor width was expanded to match the shifted shelf edge, preventing the shelf from visually hanging outside the attic room.
- Packaged `0.1.69` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Correction

- Release 0.1.70 replaces the overbuilt shelf map-fragment wall cages with cardboard block pens that match the intended block-around-map layout.
- Shelf front guard was reduced back to a low lip so it prevents accidental falls without becoming a full wall.
- Shelf end and back walls now use normal attic wall height instead of oversized blockers.
- Shelf right edge now aligns with the attic right wall to remove the walk-under overhang.
- Packaged `0.1.70` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Left-Side Alignment Fix

- Release 0.1.71 corrects the shelf placement rule: the shelf right edge now aligns to the base attic left wall, placing the shelf fully to the left side of the attic instead of over or under the attic floor.
- The shelf is now full attic length from top wall to bottom wall, with top and bottom shelf walls matching the normal base attic wall height.
- The dark raised ladder/shelf transition blocker was removed; the attic-facing shelf lip is now a low non-colliding visual rail so players do not have to jump from the ladder onto the shelf.
- Shelf map pieces and cardboard blocker pens now move from the shelf edge constants so they stay on the shifted shelf.
- Packaged `0.1.71` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Screen-Position Revert

- Release 0.1.72 supersedes 0.1.71. The 0.1.71 full-length shelf relocation moved the shelf to the wrong screen position and should not be used as the reference.
- Restored the shelf's prior depth/screen placement, then shifted the whole shelf assembly horizontally to the right by the shelf constants so the floor, walls, ladder, cardboard boxes, map-piece pens, and map pickups move together.
- The ladder opening was widened and the front shelf guard was reduced to a low lip around the opening to avoid the dark raised blocker that forced a jump from the ladder onto the shelf.
- Packaged `0.1.72` launched in unattended/nullrhi mode without new targeted attic, shelf, ladder, mesh, or material warnings.

## 2026-06-12 - Old Attic Shelf Left-Side Accessibility Fix

- Release 0.1.73 supersedes 0.1.72 for the Old Attic shelf placement. The shelf was still reading as below the attic and inaccessible from the intended side.
- Rebuilt the shelf constants so the raised shelf is a left-side platform aligned to the attic's Y span instead of a bottom platform.
- Moved shelf floor, side/end walls, ladder, cardboard boxes, map-piece pens, map pickups, and shelf box signage from the same shelf-relative constants so the assembly moves together.
- Packaged `0.1.73` launched in unattended/nullrhi mode with a clean targeted scan for attic, shelf, ladder, mesh, material, and input warnings.

## 2026-06-12 - Old Attic Shelf Rollback To 0.1.71 Placement

- Release 0.1.74 reverts the shelf-side constants back to the 0.1.71 placement after the later shelf attempts moved the shelf to the wrong side/position.
- The raised shelf is again positioned to the left of the attic with its right edge at the attic's left side, using full attic-depth shelf bounds.
- The shelf floor, walls, ladder, cardboard boxes, map-piece pens, map pickups, and shelf signage remain tied to shelf-relative constants so they move together.
- Packaged `0.1.74` launched in unattended/nullrhi mode with a clean targeted scan for attic, shelf, ladder, mesh, material, and input warnings.

## 2026-06-12 - Old Attic Diagram Layout

- Release 0.1.76 rebuilds Level 1 from the user-provided layout sketch: the main attic is the right rectangle, the raised shelf is the left rectangle at half the main attic width, and both share the same vertical span.
- The shared wall is split with one ladder bridge opening only; the shelf has full-height top, bottom, outer, and split inner walls.
- Map pieces and cardboard blocker clusters are placed from the diagram: two on the shelf side and two inside the main attic.
- This replaces the prior attempts that shifted the whole shelf into the wrong screen position.

## 2026-06-12 - Hub Road Full-Length Sidewalk Stopping Point

- User request: fix only the Neighborhood Hub road/sidewalk layout from the screenshot with two red dots marking the intended sidewalk positions. The road should widen to span between those sidewalk positions, with a small visible grass strip/curb lawn between each sidewalk and the asphalt, and all three bands should run the full hub length.
- Main touched file: `F:\gooniesgame\Source\CrooniesTreasureTroop\CrooniesTreasureTroopGameMode.cpp`, inside `ACrooniesTreasureTroopGameMode::SpawnHub()`.
- Current generated hub road objects are `FullLengthWideAsphaltStreet`, `LeftCurbLawnStrip`, `RightCurbLawnStrip`, `LeftFullLengthSidewalk`, `RightFullLengthSidewalk`, and repeated `FullLengthRoadCenterStripe` blocks.
- Current road constants are `HubRoadHalfWidth = 1520.f`, `HubCurbLawnWidth = 90.f`, `HubSidewalkWidth = 120.f`, `HubRoadLengthScale = HubFloorScale.X + 0.20f`, and `HubRoadCenterY = 0.f`.
- Important cleanup: the first inserted version had literal PowerShell tab escape text, `` `t``, at the start of the new C++ lines. Those were removed so the source compiles normally.
- Verification completed today: `CrooniesTreasureTroopEditor Win64 Development` built successfully with UE 5.5 from `D:\Epic Games\UE_5.5` using compiler version `14.44.35228`.
- Package completed successfully as release `0.1.79` with notes `Widen hub road, add full-length sidewalks and curb lawns`.
- New packaged executable: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.79\Windows\CrooniesTreasureTroop.exe`.
- New patch manifest: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.79\patch-manifest.json`.
- Build/package warnings observed were the existing Unreal preferred-compiler warning for VS2022 14.44 and config staging notices for `Config\config.ini` and `Config\TemplateDefs.ini`; no C++ compile failure occurred after the tab-escape cleanup.
- Visual follow-up for tomorrow: launch or inspect `0.1.79`, enter the Neighborhood Hub, and confirm the full-length road spans between the intended red-dot sidewalk positions while leaving a visible grass curb lawn between road and sidewalk on both sides.
- If the road still misses the screenshot target, adjust only `HubRoadHalfWidth`, `HubCurbLawnWidth`, and/or `HubSidewalkWidth` in `SpawnHub()`. Do not touch the Old Attic shelf layout unless explicitly requested.
- Shutdown state at stopping point: no `CrooniesTreasureTroop`, `UnrealEditor`, `AutomationTool`, `ShaderCompileWorker`, `dotnet`, or `Zen` processes were found running from the cleanup checks.

## 2026-06-12 - Hub Road Width Reduced

- Follow-up request: road was too wide, reduce it by 50%.
- Applied exact scoped change in `SpawnHub()`: `HubRoadHalfWidth` changed from `1520.f` to `760.f`.
- Left `HubCurbLawnWidth = 90.f` and `HubSidewalkWidth = 120.f` unchanged so the visible grass curb lawn and sidewalk band widths remain the same.
- `CrooniesTreasureTroopEditor Win64 Development` build completed successfully after the width reduction.
- Attempted to package as `0.1.80` with notes `Reduce hub road width by 50 percent`, but the cook step became abnormally slow/stalled in `UnrealEditor-Cmd.exe` material/package loading after two attempts. No `0.1.80` release folder was created.
- Stalled package workers were stopped. Latest completed packaged build remains `0.1.79`, but the source now contains the 50% road-width reduction and has passed the editor build.

## 2026-06-13 - Hub Road Width Reduced Again

- Follow-up request: reduce the street in the hub by 50% again.
- Applied exact scoped change in `SpawnHub()`: `HubRoadHalfWidth` changed from `760.f` to `380.f`.
- Left `HubCurbLawnWidth = 90.f` and `HubSidewalkWidth = 120.f` unchanged.
- `CrooniesTreasureTroopEditor Win64 Development` build completed successfully after the second width reduction.
- No package was created for this change; previous package attempts for `0.1.80` had stalled during cook, so this entry records the current source/build state only.

## 2026-06-13 - Hub Road Width Reduced Third Time

- Follow-up request: reduce the road by half again.
- Applied exact scoped change in `SpawnHub()`: `HubRoadHalfWidth` changed from `380.f` to `190.f`.
- Left `HubCurbLawnWidth = 90.f` and `HubSidewalkWidth = 120.f` unchanged.
- `CrooniesTreasureTroopEditor Win64 Development` build completed successfully after this third width reduction.
- No package was created for this change; this entry records the current source/build state only.

## 2026-06-13 - Hub Road Width Reduced Fourth Time

- Follow-up request: reduce it by half again.
- Applied exact scoped change in `SpawnHub()`: `HubRoadHalfWidth` changed from `190.f` to `95.f`.
- Left `HubCurbLawnWidth = 90.f` and `HubSidewalkWidth = 120.f` unchanged.
- `CrooniesTreasureTroopEditor Win64 Development` build completed successfully after this fourth width reduction.
- No package was created for this change; this entry records the current source/build state only.

## 2026-06-13 - Hub Road Made Noticeably Smaller With Grass Entrances

- Follow-up request: the road still did not look reduced; reduce it again and make sure the attic entrance, cave entrances, and shops sit on grass with the road not touching them.
- Applied scoped hub-only change in `SpawnHub()`: `HubRoadHalfWidth` changed from `95.f` to `40.f` for a visibly narrower asphalt strip.
- Added explicit grass pads above the decorative road/path layer for `OldAtticGrassEntrancePad`, `CaveAndShopGrassEntrancePad`, and `AdventureShopGrassEntrancePad`.
- Left the Old Attic shelf layout untouched.
- `CrooniesTreasureTroopEditor Win64 Development` build completed successfully after the narrower road and grass entrance pads.
- No package was created for this change; this entry records the current source/build state only.

## 2026-06-13 - Hub Sidewalk Placement Corrected From Screenshot

- User clarified that the previous road edits missed the actual request: sidewalks should sit roughly where the red lines are in the screenshot, between the attic asset and cave/shop asset, with the road centered between those sidewalks. The sidewalk should not touch the cave entrance/shop assets or the attic asset.
- Removed the incorrect grass-pad workaround from the prior edit.
- Rebuilt the road layout constants in `SpawnHub()` so the asphalt is centered at `Y = 0`, road edges are at `Y +/-360`, curb lawns run from `360` to `415`, sidewalks run from `415` to `505`, and the area beyond the sidewalks remains grass toward the entrance assets.
- Current constants: `HubRoadHalfWidth = 360.f`, `HubCurbLawnWidth = 55.f`, `HubSidewalkWidth = 90.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.80` using the updated game binary and `-SkipBuild` because full cook/package had previously stalled. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.80`.
- Important: test with `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.80\Windows\CrooniesTreasureTroop.exe`; older release `0.1.79` does not include this corrected layout.

## 2026-06-13 - Hub Road Cave-Side Edge Adjusted Only

- User clarified the house-side road edge should not move. Only the side closer to the cave entrance should move outward, then the road should be refit/centered between the two sidewalks.
- Replaced symmetric road math with explicit edges in `SpawnHub()`: `HubHouseSideRoadEdgeY = 360.f` remains fixed, and `HubCaveSideRoadEdgeY = -760.f` moves only the cave-side edge outward.
- Road center is now derived from those edges with `HubRoadCenterY = -200.f`; asphalt width derives from the distance between the two edges.
- Curb lawns and sidewalks are derived independently on each side, so the house-side sidewalk stays in its existing position while the cave-side sidewalk follows the moved cave-side road edge.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.81` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.81`.

## 2026-06-13 - Hub Cave-Side Sidewalk Moved Near Cave Entrance

- User clarified there should be no grass where the player marker stood in the screenshot; the cave-side sidewalk should move much closer to the cave entrance and cover the brown line left of the marker.
- Kept `HubHouseSideRoadEdgeY = 360.f` unchanged.
- Moved only the cave-side edge from `HubCaveSideRoadEdgeY = -760.f` to `-1305.f`.
- Cave entrance row is at `Y = -1550.f`; with `HubCurbLawnWidth = 55.f` and `HubSidewalkWidth = 90.f`, the cave-side sidewalk now spans approximately `Y = -1450.f` to `Y = -1360.f`, leaving only a small grass buffer before the cave/shop assets.
- Derived road center is now `Y = -472.5f`, with the road refit between the fixed house side and moved cave side.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.82` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.82`.

## 2026-06-13 - Hub Cave-Side Sidewalk Backed Off Slightly

- User clarified the `0.1.82` cave-side move went a little too far toward the cave; reduce that move by about 25% to better match the red line in the new screenshot.
- Kept `HubHouseSideRoadEdgeY = 360.f` unchanged.
- Moved only the cave-side edge from `HubCaveSideRoadEdgeY = -1305.f` back to `-1170.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.83` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.83`.

## 2026-06-13 - Hub Cave-Side Edge Set To -1250

- User clarified that `-1170.f` was too far back and requested trying `-1250.f`.
- Kept `HubHouseSideRoadEdgeY = 360.f` unchanged.
- Changed only `HubCaveSideRoadEdgeY` from `-1170.f` to `-1250.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.84` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.84`.

## 2026-06-13 - Hub Cave-Side Edge Returned To -1170

- User clarified `-1250.f` still was not narrow enough and requested trying `-1170.f` again.
- Kept `HubHouseSideRoadEdgeY = 360.f` unchanged.
- Changed only `HubCaveSideRoadEdgeY` from `-1250.f` back to `-1170.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.85` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.85`.

## 2026-06-13 - Hub Cave-Side Edge Set To -1100

- User requested a tiny bit more narrowing from the `-1170.f` position by trying `-1100.f`.
- Kept `HubHouseSideRoadEdgeY = 360.f` unchanged.
- Changed only `HubCaveSideRoadEdgeY` from `-1170.f` to `-1100.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.86` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.86`.

## 2026-06-13 - Hub Full-Length Dock And Water

- User confirmed the road is done and requested the brown marked area become a wooden dock and the blue marked area become water.
- Added `FullLengthHubWaterChannel` across the hub at `HubWaterCenterY = -1900.f` with `HubWaterWidth = 700.f`.
- Added `FullLengthWalkableWoodenDock` across the hub at `HubDockCenterY = -1510.f` with `HubDockWidth = 285.f`.
- Added `HubDockWaterOverhangLip` on the water side of the dock, plus repeated `HubDockEndToEndPlankSeam` and `HubWaterSurfaceRipple` decor blocks so the area reads as wood planks and water.
- The dock uses `SpawnBlock` so it is walkable; water and decorative seams/ripples use `SpawnDecorBlock`.
- Road constants were left unchanged from the accepted road layout.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.87` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.87`.

## 2026-06-13 - Hub Dock And Water Rotated Across Hub

- User clarified the `0.1.87` dock/water was facing the wrong direction and should be rotated roughly 90 degrees, extending to each side of the hub.
- Replaced the parallel-to-road dock/water placement with cross-hub placement.
- Current water: `FullLengthHubWaterChannel` centered at `HubWaterCenterX = 300.f`, width `760.f`, spanning the hub side-to-side with `HubCrossLengthScale = HubFloorScale.Y + 0.20f`.
- Current dock: `FullLengthWalkableWoodenDock` centered at `HubDockCenterX = -180.f`, width `285.f`, also spanning side-to-side.
- Updated dock overhang, plank seams, and water ripple details to match the rotated orientation.
- Road constants were left unchanged from the accepted road layout.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.88` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.88`.

## 2026-06-13 - Hub Dock Moved To Pet Lane, Water Flipped To Wall Side

- User clarified the cross-hub orientation was correct but the feature was in the wrong spot. Water should be on the opposite side from the prior placement, moved down by the visible cat/dog assets, with the cat on the dock and water extending from the dock to the hub wall.
- Moved `FullLengthWalkableWoodenDock` to `HubDockCenterX = 1880.f`, matching the `HubCatMango` lane.
- Flipped water to the opposite side of the dock: `HubWaterInnerEdgeX` is derived from the dock edge, `HubWaterOuterEdgeX = -HalfX`, and water fills from the dock to the west hub wall.
- Updated the dock overhang lip to the water side of the moved dock.
- Road constants were left unchanged from the accepted road layout.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.89` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.89`.

## 2026-06-13 - Hub Dock Moved To Far Pet Reference

- User clarified the prior dock move used the wrong side of the map. The intended reference is the circled far-side pet near the cave/road area, not `HubCatMango`.
- Moved `FullLengthWalkableWoodenDock` from `HubDockCenterX = 1880.f` to `2860.f`, matching the `HubDogScout` lane.
- Flipped water so it fills only from the dock edge to the east hub wall: `HubWaterInnerEdgeX = HubDockCenterX + HubDockWidth * 0.5f`, `HubWaterOuterEdgeX = HalfX`.
- This restores grass on the opposite side instead of filling the wrong grass area with water.
- Road constants were left unchanged from the accepted road layout.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.90` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.90`.

## 2026-06-13 - STOPPING POINT: Hub Dock/Water Still Not Correct

- Stop here for the night. User is frustrated; do not keep guessing. Next pass must use the screenshot references carefully before editing.
- Important compass rule from user: interpret the screenshot like a normal map/compass. Screen up is north, screen down is south, screen left is west, and screen right is east. Do not describe screen-bottom as east/west.
- Accepted state before dock/water work: the road/sidewalk layout is finally acceptable. Do not touch the road constants unless the user explicitly asks.
- Accepted road constants currently in `SpawnHub()`: `HubHouseSideRoadEdgeY = 360.f`, `HubCaveSideRoadEdgeY = -1100.f`, `HubCurbLawnWidth = 55.f`, `HubSidewalkWidth = 90.f`.
- Main file remains `F:\gooniesgame\Source\CrooniesTreasureTroop\CrooniesTreasureTroopGameMode.cpp`.
- User's intended dock/water visual:
  - Brown marked area with wavy lines should become a wooden dock.
  - Blue marked area should become water.
  - Dock/water orientation should run across the hub side-to-side as shown in the annotated images, not parallel with the road.
  - The dock should be near the circled cave entrance and the circled cat/dog reference in the user's latest screenshots.
  - The water should fill only the intended blue area from the dock edge to the appropriate hub wall, not replace a broad unrelated grass region.
  - Grass should remain green anywhere outside that specific water band.
- User explicitly said the previous attempt put the dock/water at the wrong end of the map and filled the wrong grass area with water.
- Current source state after last attempted correction, NOT confirmed visually correct:
  - `HubDockCenterX = -2900.f`
  - `HubDockWidth = 620.f`
  - `HubWaterInnerEdgeX = HubDockCenterX - (HubDockWidth * 0.5f)`
  - `HubWaterOuterEdgeX = -HalfX`
  - This was intended to move the dock toward the north-side pet/cave references and water from dock edge to the north hub wall, but the user stopped before testing and did not accept it.
- Build status after the last source edit: both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` compiled successfully.
- No release/package was created for the last unaccepted `HubDockCenterX = -2900.f` attempt. Latest packaged release remains `0.1.90`, which uses the prior wrong far-pet placement and should not be treated as final.
- Next session recommendation:
  - Start by reviewing the latest screenshot with the compass overlay and circled cave/cat/dog references.
  - Determine which hub coordinates correspond to those exact visual references before editing.
  - Keep road constants unchanged.
  - Adjust only the dock/water constants and regenerate a new release after the user accepts the visual direction.

## 2026-06-14 - Hub Dock/Water Mirrored To Cave Side

- User clarified the next step should be simple: take the current water and dock, turn it 180 degrees, and switch it to the opposite side of the map. It was on the money-shop side and needed to be on the cave-entrance side.
- Road constants were left unchanged.
- Mirrored only the dock/water constants in `SpawnHub()`:
  - `HubDockCenterX` changed from `-2900.f` to `2900.f`.
  - Water now extends in the matching mirrored direction using `HubWaterInnerEdgeX = HubDockCenterX + (HubDockWidth * 0.5f)` and `HubWaterOuterEdgeX = HalfX`.
  - Dock water overhang lip moved to the mirrored side.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.91` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.91`.

## 2026-06-14 - Hub Dock/Water Anchored To Actual Cave Entrance

- Correction after user pointed out `0.1.91` still visibly placed the dock/water by the money-shop side.
- Verified actual portal coordinates in code: `Caverns Entrance` is at `FVector(-1200.f, -1550.f, 55.f)` and `Local Gear Store` is at `FVector(1200.f, -1550.f, 55.f)`.
- Changed dock anchor to the cave entrance X coordinate: `HubDockCenterX = -1200.f`.
- Water now extends from the dock's cave-side edge toward `-HalfX`, keeping the layout on the cave entrance side instead of the money-shop side.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully before this release.
- Created release `0.1.92` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.92`.

## 2026-06-14 - Hub Dock Fit Between Green-Line References

- User confirmed dock/water are on correct side and orientation, but dock was too far north/top and water too broad.
- Dock should sit between the two green lines/red-circled references in the latest screenshot.
- Changed dock to span from `HubDockNorthEdgeX = -1200.f` to `HubDockSouthEdgeX = 2860.f`; center and width are derived from those values.
- Water now fills only from `-HalfX` to `HubDockNorthEdgeX`, meeting the dock edge instead of flooding extra grass.
- Water surface Z raised from `6.f` to `16.f` so road underneath the water is hidden.
- Removed hub water-side trees by skipping dense hub north/west tree spawns in water-covered areas; trees should remain only in grass areas.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.93` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.93`.

## 2026-06-14 - Hub Dock Reverted, Halved, And Shifted Toward Wall

- User rejected `0.1.93`: dock was made far too large and moved farther away from intended spot.
- Reverted the dock sizing/position style back from edge-derived giant span to center/width constants.
- Reduced the dock width by half from the previous compact width: `HubDockWidth = 310.f`.
- Moved the dock farther from the cave entrance toward the hub wall: `HubDockCenterX = -1510.f`.
- Water still fills from the hub wall to the dock edge and remains above the road surface so road should not show in water.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.94` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.94`.

## 2026-06-14 - Hub Dock Band Set Between Screenshot Reference Lines

- User clarified the red arrows point to the exact dock edge positions compared to the attic and cave entrance references.
- Replaced the tiny center/width dock with explicit dock edges again, but not the previous huge full span.
- Current dock bounds: `HubDockNorthEdgeX = -1200.f`, `HubDockSouthEdgeX = 900.f`.
- Dock center/width are derived from those bounds.
- Water stops at `HubDockNorthEdgeX` and remains above the road surface.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.95` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.95`.

## 2026-06-14 - Hub Dock Compact With Light Wood And Small Dark Edge

- User rejected the previous dock adjustment and clarified the dock should not be dark or raised.
- Current source state:
  - `DockWood = FLinearColor(0.74f, 0.54f, 0.32f)` for lighter wood.
  - `HubDockNorthEdgeX = 2860.f`.
  - `HubDockWidth = 310.f`.
  - Dock uses normal height: `SpawnBlock(... Z 19.f, Scale.Z 0.12f)`.
  - Small dark edge strip only: `HubDockWaterOverhangLip` at `HubDockNorthEdgeX - 12.f` with X scale `0.24f`.
  - Plank seams remain dark brown on top of the light dock.
- Water still stops at `HubDockNorthEdgeX` and remains above road surface.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- First `0.1.96` release attempt had a stale packaged binary because the staged exe was locked; it was moved to `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.96.backup-20260614-134522`.
- Recreated release `0.1.96` after copying the rebuilt game binary. Current release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.96`.

## 2026-06-14 - Hub Water Reduced Back To Edge Strip

- User rejected `0.1.96` because water covered most of the hub grass and left assets floating in water.
- Current intended screenshot interpretation:
  - The large green-marked area above the dock should stay grass.
  - The dock is the light brown horizontal band.
  - The dark brown strip is only the small lip between dock and water; the dock itself is not raised.
  - Solid blue water should only fill from the dock water edge to the hub wall, roughly the outer 10-20% of the hub.
- Road constants were left unchanged.
- Current source state in `SpawnHub()`:
  - `HubDockGrassEdgeX = 2860.f`.
  - `HubDockWidth = 310.f`.
  - `HubDockWaterEdgeX = HubDockGrassEdgeX + HubDockWidth`, currently `3170.f`.
  - Water spans from `HubDockWaterEdgeX` to `HalfX`, so most of the hub remains the green base grass.
  - `FullLengthHubWaterChannel` is spawned after the road at Z `16.f`, so road under the water should not be visible.
  - `FullLengthWalkableWoodenDock` remains light wood at normal height.
  - `HubDockWaterOverhangLip` is the only dark strip, placed just past the dock water edge.
- Hub dense tree border cleanup was updated for this water side:
  - Skip top/bottom border trees only where `X > 3170.f`.
  - Keep the opposite side tree line and skip the `TreeEast` line for this hub border, so trees do not appear inside the water strip.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Copied the freshly rebuilt game exe/pdb into the staged packaged folder before creating the next release.
- Created release `0.1.97` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.97`.

## 2026-06-14 - Hub Dock/Water Same Size Moved Opposite Gear Store Side

- User confirmed the dock/water size was finally right in `0.1.97`, but the strip was on the wrong side of the map near the money/gear store side.
- Kept the same dock width and water-strip width, but moved the strip to the opposite X side from the gear store:
  - `HubDockGrassEdgeX = -2860.f`.
  - `HubDockWidth = 310.f`.
  - `HubDockWaterEdgeX = HubDockGrassEdgeX - HubDockWidth`, currently `-3170.f`.
  - Water spans from `-HalfX` to `HubDockWaterEdgeX`.
  - Dock center is derived from `HubDockGrassEdgeX` and `HubDockWaterEdgeX`.
  - Dark lip moved to `HubDockWaterEdgeX - 12.f`.
- Hub dense tree skip was moved to the same side: skip border trees where `X < -3170.f`, and skip the west-side tree line for this hub border.
- Road constants were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.98` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.98`.

## 2026-06-14 - Hub Road Shortened With Natural Ends

- User approved the dock/water size and side, then requested road-only cleanup.
- Dock and water placement were left unchanged:
  - `HubDockGrassEdgeX = -2860.f`.
  - `HubDockWidth = 310.f`.
  - Water still spans from `-HalfX` to `HubDockWaterEdgeX`.
- Replaced the previous full-length hub road with a trimmed main road and a dock-side turn:
  - Main straight road now ends before the real money/gear store at `HubMainRoadStoreEndX = 850.f`.
  - Main straight road starts from the turn at `HubBranchRoadCenterX`.
  - Added `HubMoneyStoreRoadEndSidewalkCap` and `HubMoneyStoreRoadSoftEndBand` so the store-side road does not end abruptly.
  - Added `HubDockSideRoadTurn`, a same-width side road that turns toward the attic/dog side.
  - The dock-side sidewalk uses `HubBranchDockSidewalkOuterX = HubDockGrassEdgeX` so it lines up with the dock grass edge opposite the water side.
  - Added `HubDockSideRoadEndSidewalkCap` at `HubBranchRoadEndY = 1040.f`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Closed two running packaged Croonies processes that were locking the staged exe, then copied the fresh rebuilt binary.
- Created release `0.1.99` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.99`.

## 2026-06-14 - Reverted Hub Road Cleanup Back To 0.1.98

- User rejected `0.1.99` and requested a full revert back to the `0.1.98` road state.
- Removed all `0.1.99` road turn/end changes:
  - Removed `HubMainAsphaltStreetTrimmed`.
  - Removed `HubDockSideRoadTurn`.
  - Removed money-store sidewalk cap/soft end band.
  - Removed dock-side road cap and side-road sidewalks/curbs.
- Restored the `0.1.98` full straight hub road block:
  - `HubRoadLengthScale = HubFloorScale.X + 0.20f`.
  - `FullLengthWideAsphaltStreet`.
  - `LeftCurbLawnStrip`, `RightCurbLawnStrip`.
  - `LeftFullLengthSidewalk`, `RightFullLengthSidewalk`.
  - `FullLengthRoadCenterStripe` loop from `-22` to `22`.
- Dock/water remain at the accepted `0.1.98` side and size:
  - `HubDockGrassEdgeX = -2860.f`.
  - `HubDockWidth = 310.f`.
  - Water spans from `-HalfX` to `HubDockWaterEdgeX`.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.100` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.100`.

## 2026-06-14 - Hub Road Ends Before Money Store With U Sidewalk Cap

- User requested one change only after `0.1.100`: end the road at the red box/black line after the Special entrance and before the real money store.
- Dock, water, and all hub assets were left unchanged.
- Road endpoint change:
  - Added `HubRoadStoreEndX = 2850.f`.
  - Road, curb lawns, and both side sidewalks now use `HubRoadCenterX` and `HubRoadLengthScale` derived from `HubRoadStartX = -HalfX` to `HubRoadStoreEndX`.
  - Added `HubStoreRoadEndUSidewalkCap`, a grey sidewalk cap crossing the road end and connecting the two side sidewalks into a U shape.
  - Added `HubStoreRoadRoundedEndFace` as a small asphalt face just inside the cap so the road does not look like it continues past the sidewalk.
  - Center stripe loop now skips stripes beyond the new road end.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.101` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.101`.

## 2026-06-14 - Hub Dock-End Road Turn Toward Attic Direction

- User requested the opposite/dock end of the road turn away from the cave entrance and toward the attic direction.
- Money-store road cap from `0.1.101` was left unchanged.
- Dock and water placement were left unchanged.
- Added dock-end turn/continuation using the existing road and sidewalk widths:
  - `HubDockGrassEdgeX = -2860.f` remains the dock grass-side alignment.
  - `HubDockTurnRoadMinX = HubDockGrassEdgeX + HubSidewalkWidth + HubCurbLawnWidth`.
  - `HubDockTurnRoadMaxX = HubDockTurnRoadMinX + (HubHouseSideRoadEdgeY - HubCaveSideRoadEdgeY)`.
  - Turn continuation runs from `HubDockTurnRoadStartY = HubHouseSideRoadEdgeY` to `HubDockTurnRoadEndY = 1040.f`.
  - Added `HubDockEndRoadAtticTurnStraight` for the continued road toward attic direction.
  - Added matching dock-side and outer-side curb/sidewalk strips.
  - Added small rotated road and sidewalk curve-fill pieces to soften the transition from the original road into the turn.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.102` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.102`.

## 2026-06-14 - Hub Dock-End Turn Cleaned To Straight L Layout

- User rejected `0.1.102` because the angled/rotated turn pieces looked bad.
- Replaced the dock-end turn with a simple straight L-style layout matching the new markup:
  - Removed the rotated road curve fill pieces.
  - Removed the rotated sidewalk curve fill pieces.
  - Main road now starts at `HubRoadStartX = HubDockTurnRoadCenterX` instead of continuing beyond the turn.
  - Dock-end continuation is `HubDockEndRoadAtticTurnStraight`, running toward the attic direction from `HubDockTurnRoadStartY = HubHouseSideRoadEdgeY` to `HubDockTurnRoadEndY = HalfY`.
  - Sidewalks and curb lawns run parallel on both sides of that dock-end continuation.
  - Added `HubDockEndTurnCenterStripe` loop so the yellow dotted center line turns with the new road.
- Money-store road cap, dock, water, and hub assets were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.103` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.103`.

## 2026-06-14 - Hub Dock-End Corner Fill And Sidewalk Cleanup

- User accepted the general `0.1.103` shape but marked three small corrections.
- Only changed the dock-end road/sidewalk corner:
  - Added `HubDockEndMissingRoadCornerFill` to fill the black-marked missing road square at the inside corner of the L.
  - Added `HubDockEndMissingBottomSidewalkFill` and `HubDockEndMissingSidewalkFill` for the grey-marked missing sidewalk areas.
  - Removed the red-circled outer vertical sidewalk strip by deleting `HubDockEndTurnOuterSidewalk`.
- Money-store cap, dock, water, and hub assets were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.104` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.104`.

## 2026-06-14 - Hub Dock-End Sidewalk Micro-Fixes

- User marked three remaining dock-end sidewalk issues in `0.1.104` and asked for only those small corrections.
- Only changed the dock-end L-turn sidewalk pieces:
  - Removed `HubDockEndMissingSidewalkFill`, the stray vertical sidewalk strip sitting inside the road.
  - Restored `HubDockEndTurnOuterSidewalk` so the sidewalk continues after the turn.
  - Added `HubDockEndDockSidewalkCornerFill` to fill the small missing L-corner sidewalk piece.
- Money-store cap, dock, water, road placement, and hub assets were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.105` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.105`.

## 2026-06-14 - Hub Dock-End Dedicated Road Corner

- User rejected the patched sidewalk-corner approach in `0.1.105` and requested a dedicated corner piece instead of cobbled-together fills.
- Only changed the dock-end road/sidewalk corner construction:
  - Added `HubDockEndDedicatedRoadCorner`, a dedicated square asphalt corner using the same road color and width basis as the existing road.
  - Shortened the main straight road to start at the far edge of the dedicated corner instead of overlapping through it.
  - Removed the old corner sidewalk/road fill-strip actors: `HubDockEndMissingRoadCornerFill`, `HubDockEndMissingBottomSidewalkFill`, and `HubDockEndDockSidewalkCornerFill`.
  - Added named corner curb/sidewalk pieces around the dedicated road corner so the outside and dock-side sidewalks meet the corner more cleanly.
  - Trimmed `HubDockEndTurnOuterSidewalk` so it starts after the main-road sidewalk and no longer blindly runs down into the street.
- Money-store cap, dock, water, hub assets, and accepted road/dock placement were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.106` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.106`.

## 2026-06-15 - Hub Dock-End Corner Stripe And Sidewalk Cleanup

- User marked four small issues after `0.1.106` and asked to fix only those.
- Dock-end L-corner fixes:
  - Shortened `HubDockEndTurnOuterSidewalk` slightly by moving its start from `HubSidewalkWidth * 0.5f` past the road sidewalk to `HubSidewalkWidth * 0.75f`.
  - Added `HubDockEndBranchFirstCenterStripe` so the horizontal branch road has a deliberate first yellow center dash after the corner.
  - Added a lower bound to the main road stripe loop so `FullLengthRoadCenterStripe` no longer appears before the shortened road starts.
  - Added `HubDockEndCornerSidewalkConnector` to close the small missing sidewalk corner at the lower-left outside connection.
- Money-store cap, dock, water, hub assets, and accepted road/dock placement were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.107` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.107`.

## 2026-06-15 - Hub Dock-End Corner Four-Issue Refinement

- User re-posted the four numbered dock-end corner issues and asked for a cleaner pass.
- Only refined those four marked areas:
  - Further shortened `HubDockEndTurnOuterSidewalk` by moving its start to `HubSidewalkWidth * 1.20f` past the house-side sidewalk center.
  - Added `HubDockEndBranchSecondCenterStripe` and `HubDockEndBranchThirdCenterStripe` so the horizontal branch road has several deliberate dashes near the corner.
  - Moved the general `FullLengthRoadCenterStripe` loop lower bound farther past the corner and removed `HubDockEndCornerRoadStripeNorth`, eliminating the unwanted vertical corner dash.
  - Widened `HubDockEndCornerSidewalkConnector` slightly to close the lower-left sidewalk connection more completely.
- Money-store cap, dock, water, hub assets, and accepted road/dock placement were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.108` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.108`.

## 2026-06-15 - Hub Dock-End Corrected Sidewalk Trim And Dash Gaps

- User clarified that the prior pass shortened the wrong sidewalk near issue `#1`.
- Only changed the same four dock-end corner problem areas:
  - Restored `HubDockTurnOuterSidewalkStartY` to the original turn-sidewalk alignment.
  - Added `HubHouseSidewalkStartX`, `HubHouseSidewalkCenterX`, and `HubHouseSidewalkLengthScale` so `RightFullLengthSidewalk` starts later and no longer protrudes into the road at issue `#1`.
  - Added `HubDockEndCornerCenterStripeSouth` and `HubDockEndCornerCenterStripeNorth` for the missing yellow center dashes at issues `#2` and `#3`.
  - Widened `HubDockEndCornerSidewalkConnector` again to close both small breaks around issue `#4`.
- Money-store cap, dock, water, hub assets, and accepted road/dock placement were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.109` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.109`.

## 2026-06-15 - Hub Dock-End Remaining Dash And Sidewalk Break Fix

- User narrowed the remaining dock-end corner issues to two small fixes.
- Only changed those two marked areas:
  - Added `HubDockEndCornerCenterStripeMidSouth` and `HubDockEndCornerCenterStripeMidNorth` to fill the remaining missing yellow divider dashes in the circled road area.
  - Widened `HubDockEndCornerSidewalkConnector` again so it reaches the remaining sidewalk break.
- Money-store cap, dock, water, hub assets, and accepted road/dock placement were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.110` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.110`.

## 2026-07-10 - Old Attic Map 4/4 Box And Wall Height Fix

- User marked the Old Attic shelf map 4/4 L2 box layout and base wall height.
- Only changed the Old Attic layout:
  - Moved the bottom shelf map-fragment pen's low jumpable box to the marked red-X side.
  - Kept the opposite marked circle side as a tall non-jumpable blocker by skipping the duplicate tall blocker only at the new low-entry position.
  - Raised the short main-platform north wall segments to match the main attic wall height.
  - Raised the shelf inner wall segments to match the raised shelf outer perimeter wall height so the platform/shelf perimeter reads as a seamless full-height wall.
- Hub road, dock, water, and unrelated attic props were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.111` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.111`.

## 2026-07-10 - Old Attic Outer Perimeter Wall And Map 4/4 Opening Swap

- User rejected `0.1.111` because it raised the divider/shelf seam walls instead of only the main attic platform outer perimeter.
- Restored divider/shelf seam walls to their prior lower height:
  - `OldAtticNorthConnectedWoodWallLeftSegment` / `RightSegment` back to `Z=45.f`, scale Z `1.10f`.
  - `OldAtticRaisedShelfInnerWallLeftSegment` / `RightSegment` back to `Z=297.f`, scale Z `1.10f`.
- Raised only the main attic platform true outer perimeter walls to match the shelf outer wall top height:
  - `OldAtticSouthConnectedWoodWall`, `OldAtticWestConnectedWoodWall`, `OldAtticEastConnectedWoodWall` use `Z=226.f`, scale Z `4.72f`.
- Swapped the bottom shelf map 4/4 L2 opening by moving the low jumpable entry box to `(170.f, -170.f, 310.f)` for `bEntryOnNegativeY`.
- Hub road, dock, water, shelf outer walls, and unrelated attic props were left unchanged.
- Built both `CrooniesTreasureTroopEditor Win64 Development` and `CrooniesTreasureTroop Win64 Development` successfully.
- Created release `0.1.112` using the updated game binary and `-SkipBuild`. Release folder: `F:\gooniesgame\Releases\CrooniesTreasureTroop-0.1.112`.
