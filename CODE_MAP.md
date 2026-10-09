# Item-wise Code Map

Open the file beside an item to show its coordinates, sizes, rotations and drawing code.
For animals and the farmer, the same `.cpp` also contains initial values and movement/task logic.
Headers stay in `include/`; implementation files stay in `src/`.

## Farm items

| Item | Code file | What to explain |
|---|---|---|
| Ground | [src/items/ground.cpp](src/items/ground.cpp) | Circular terrain mesh, distant rolling hills, gentle curvature and unchanged flat farm grass; declaration in `include/terrain_renderer.h` |
| Surrounding fields | [src/items/surroundings.cpp](src/items/surroundings.cpp) | Crop rows, distant hills and tree-line placement |
| Road | [src/items/road.cpp](src/items/road.cpp) | Entrance road and internal path |
| Boundary fence | [src/items/boundary_fence.cpp](src/items/boundary_fence.cpp) | Fence posts, rails and globe lamps |
| Entrance gate | [src/items/entrance_gate.cpp](src/items/entrance_gate.cpp) | Pillars, two hinged gate leaves and gate rotation |
| Farm name sign | [src/items/farm_sign.cpp](src/items/farm_sign.cpp) | AMS AGRO FARM sign placement |
| Owner billboard | [src/items/owner_billboard.cpp](src/items/owner_billboard.cpp) | Curved board and owner-name placement |
| 3D letters | [src/items/block_text.cpp](src/items/block_text.cpp) | Letter grid and curved text equations |
| Cow shed | [src/items/cow_shed.cpp](src/items/cow_shed.cpp) | Walls, beams and calf-side door |
| Barn | [src/items/barn.cpp](src/items/barn.cpp) | Barn walls, roof and openings |
| Cow stalls and troughs | [src/items/cow_stalls.cpp](src/items/cow_stalls.cpp) | Partitions, stall gates, delivered hay and rising drinking-water surface |
| Hay stack | [src/items/hay_stack.cpp](src/items/hay_stack.cpp) | Bales and pickup bundle at the hay area |
| Trees | [src/items/trees.cpp](src/items/trees.cpp) | Trunk, leaves and farm tree positions |
| Water tank | [src/items/water_tank.cpp](src/items/water_tank.cpp) | Tank, platform and supports |
| Interactive valve and hose | [src/items/water_hose.cpp](src/items/water_hose.cpp) | Rotating hand-wheel, flexible sagging hose, nozzle, water stream and trough connection |
| Poultry shed | [src/items/poultry_shed.cpp](src/items/poultry_shed.cpp) | Chicken/duck shelter |
| Farmer house | [src/items/worker_house.cpp](src/items/worker_house.cpp) | Room, roof, windows and opening door |
| Ceiling fans | [src/items/ceiling_fan.cpp](src/items/ceiling_fan.cpp) | Fan model, blades and rotation update |
| Fixed light fixtures | [src/items/light_fixtures.cpp](src/items/light_fixtures.cpp) | Point-light bulb models and aimable spotlight housing |
| Farm prop layout | [src/items/farm_props.cpp](src/items/farm_props.cpp) | Crate/sack placement, tool rack and collection pad |
| Barn farm tools | [src/items/farm_tools.cpp](src/items/farm_tools.cpp) | Shovel, kodal/hoe, rake, pitchfork, wall rack and metal buckets |
| Wooden crate | [src/items/crate.cpp](src/items/crate.cpp) | Crate model |
| Feed sack | [src/items/feed_sack.cpp](src/items/feed_sack.cpp) | Feed-sack model |

## Animals and farmer

| Item | Model + behaviour | Data/header |
|---|---|---|
| Cow and ox | [src/animals/cow.cpp](src/animals/cow.cpp) | [include/animals/cow.h](include/animals/cow.h) |
| Calves | [src/animals/calf.cpp](src/animals/calf.cpp) | [include/animals/calf.h](include/animals/calf.h) |
| Chickens, chicks, ducks and scattered grain | [src/animals/poultry.cpp](src/animals/poultry.cpp) | [include/animals/poultry.h](include/animals/poultry.h) |
| Farmer: walking, hay pickup, hose pulling, feeding and return home | [src/animals/farmer.cpp](src/animals/farmer.cpp) | [include/animals/farmer.h](include/animals/farmer.h) |

[src/animals/bovine_model.cpp](src/animals/bovine_model.cpp) contains shared cow/calf anatomy: Catmull-Rom ring meshes for the continuous head/neck and sculpted torso, separate back/belly profiles, an integrated ox shoulder hump, rounded legs, cloven hooves, ears, horns and facial details. The shorter, broader head transform also moves its attached facial details. Surface coat markings live in `shaders/texture.fs` and `shaders/gouraud.fs`; they follow the animal's local coordinates rather than floating above the skin.

## Curved items

| Item | Code file | Technique |
|---|---|---|
| Milk can | [src/curves/milk_can.cpp](src/curves/milk_can.cpp) | Cubic Bézier profile revolved around the vertical axis |
| Irrigation pipe | [src/curves/irrigation_pipe.cpp](src/curves/irrigation_pipe.cpp) | Cubic B-spline path with circular tube rings |
| Shed roof | [src/curves/curved_roof.cpp](src/curves/curved_roof.cpp) | Ruled surface |

## Shared systems (not individual objects)

| File | Responsibility |
|---|---|
| `src/main.cpp` | Keyboard/mouse input, camera/view selection, window and main loop |
| `src/simulation.cpp` | Exact 200-second, 12-chapter workday and graphics showcase: fast aerial, collision-safe entry/follow cameras, all farm tasks, Flat/Gouraud/Phong comparison, staged night lights and ray-traced rain finale |
| `src/hud_renderer.cpp`, `include/hud_renderer.h` | Code-native live-control dashboard used by the interactive project UI |
| `src/farm_scene.cpp` | Calls the farm item drawing functions in order |
| `src/entity_renderer.cpp` | Calls the animal and farmer drawing functions |
| `src/animation_system.cpp` | Calls item updates; shared day/night and shelter state; movement helpers |
| `src/curved_renderer.cpp` | Curved-item coordinator and shared GPU mesh upload/draw |
| `src/lighting_system.cpp` | Directional/five point/spot light values, spotlight aiming, distance-haze color/range and shader uniforms (no drone) |
| `include/collision_system.h` | Collision boundaries for walls, doors, gates, obstacles and live pickup-hay stock |
| `include/shared/farm_interaction.h` | Shared hay positions, safe approach points, hose/valve/trough coordinates and hose-length limit |
| `src/texture_manager.cpp` | Loads textures |
| `src/cube_renderer.cpp` | Shared cuboid drawing |
| `src/primitive_renderer.cpp` | Shared sphere, cylinder and cone drawing |
| `include/material.h` | Shared material properties |
| `include/shared/` | Shared item colours, entity root transform and curve vertex format |
| `shaders/texture.vs`, `shaders/texture.fs` | Textured Phong rendering plus derivative-based flat shading |
| `shaders/gouraud.vs`, `shaders/gouraud.fs` | Gouraud per-vertex lighting |
| `src/ray_trace_renderer.cpp`, `shaders/raytrace.vs`, `shaders/raytrace.fs` | Separate analytic ray-tracing preview: simplified objects, shadows and one reflection bounce |

The ray-traced preview uses its own analytic geometry; it does not draw the full textured item meshes.
The object files use the existing `FarmScene`, `EntityRenderer`, `AnimationSystem` and `CurvedRenderer` declarations rather than creating duplicate classes or separate programs.

## Quick explanation to the teacher

“Each scene item has its own implementation file. For example, `worker_house.cpp` builds the room, while `farmer.cpp` builds and controls the farmer. `ground.cpp` builds the ground. `milk_can.cpp` contains the Bézier equations and milk-can mesh. `main.cpp` only connects input, the scene and the rendering loop.”

Build normally with `powershell -ExecutionPolicy Bypass -File .\run_project.ps1 -BuildOnly`.
The build script recursively compiles all item folders. Visual Studio lists the same files under Items, Animals and Curves.
