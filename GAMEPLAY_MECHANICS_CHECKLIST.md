# Croonies Gameplay Mechanics Checklist

## Implemented Foundation

- [x] Added run stamina as a dungeon-run resource.
- [x] Added carry weight and run loot value tracking.
- [x] Added HUD readout for stamina, carried loot value, and carried weight.
- [x] Added placeholder treasure loot pickup.
- [x] Added placeholder stamina boost pickup.
- [x] Traps now reduce stamina as well as Courage.
- [x] Enemies now patrol and chase the player when close.
- [x] Story levels 2-12 now spawn procedural extra loot, stamina items, traps, and enemies each run.

## Tunable Next Pass

- [ ] Tune stamina drain rate after playtesting movement speed and level length.
- [ ] Tune carry-weight slowdown after checking whether heavy loot feels readable.
- [ ] Add extraction/sell flow so run loot converts into coins only after successful extraction.
- [ ] Add loadout selection before entering a dungeon.
- [ ] Add gear upgrades that change stamina, weight capacity, trap resistance, light radius, or revive ability.
- [ ] Add co-op-required puzzle actors with explicit two-player switches/pressure plates.
- [ ] Add creature variety with different chase, patrol, and guarding behavior.
- [ ] Add procedural trap placement rules per static dungeon room so hazards avoid unfair spawn points.
- [ ] Replace placeholder loot/stamina visuals with final art assets.
- [ ] Add difficulty tiers that unlock larger or more dangerous dungeon variants.

## Reference Direction

- Static dungeon layouts stay authored.
- Enemies, traps, loot, and stamina pickups vary per run.
- Map pieces remain the main high-value objective.
- Stamina acts as both timer pressure and action resource.
- Loot value-vs-weight decisions should create risk/reward pressure without copying outside game names or assets.

## Enemy Additions
- [x] Added Rat enemy type using simple placeholder body asset.
- [x] Added Bat enemy type using existing batsflashlight UI asset.
- [x] Rats and bats use bounded movement around their spawn area.
- [ ] Tune per-level rat/bat spawn counts and bounded-area radius after playtesting.

## Stationary Trap Additions
- [x] Added stationary trap kinds: Skeleton, Fire, Gas, Hole.
- [x] Procedural trap spawning now rotates through the four trap kinds.
- [x] Added placeholder code-built visuals for trap kinds pending final art.
- [ ] Tune trap damage/stamina effects per trap kind after playtesting.


## Cavern Level Scale and Exit Pass
- [x] Enlarged story/cavern levels 2-12 by a 10x layout scale; hub and attic were not included.
- [x] Spread fixed map pieces, artifacts, friendship tokens, friend pickups, gates, traps, props, enemies, and procedural run content across the enlarged levels.
- [x] Added hub-return stair/portal exits near each level's fourth map fragment pickup.
- [x] Updated level 2-12 teleport spawn points to match the enlarged level positions.
