# AMS Agro Farm — Final Module 5

The final module integrates the textured and illuminated farm with static
curved GPU meshes, collision-aware exploration, bird's-eye and four-viewport
camera modes, and a day/night animal routine. The livestock consists of one
milch cow, one ox, two calves, chickens, ducks, a nesting hen and chicks.

## Build and run

### VS Code Run button

1. In VS Code, open either the `ams_agro_farm` folder or its `Graphics project`
   parent workspace; configurations for both layouts are included.
2. Open `main.cpp`.
3. Click the top-right triangle. Both **Run C/C++ File** and **Run Code** are
   mapped to the complete project build-and-run command rather than a
   single-file `gcc main.cpp` build.

The workspace configuration calls `run_project.ps1`, which compiles every
project source file and then launches the OpenGL application. The project-local
MinGW GLFW import library and runtime DLL are already included.

You can also choose **Run > Start Debugging** and select
`Run AMS Agro Farm`; it uses the same build task with GDB.

### Visual Studio

1. Keep `ams_agro_farm` beside the supplied `CSE 4208 - Graphics` folder.
2. Open `AMSAgroFarm.sln` in Visual Studio 2022.
3. Select `Debug | x64`.
4. Build the solution and choose **Start Without Debugging**.

The project uses relative paths to the course GLAD/GLFW/GLM and Lab 4
`stb_image` files. The post-build step copies both `shaders` and `textures`
beside the executable.

The supplied `glfw3.lib` is a Visual Studio debug library (`MSVCRTD`), so the
solution intentionally exposes the compatible `Debug | x64` configuration.

## Controls

- `W`, `A`, `S`, `D`: move through the scene
- Mouse: look around
- `Q`, `E`: move down/up
- Mouse wheel: zoom
- `B`: toggle full-screen orthographic bird's-eye view
- `V`: toggle four simultaneous camera views
- `G`: smoothly open or close both gate leaves
- `O`: smoothly open or close both cattle-stall gates
- `T`: toggle texture sampling on/off for demonstration
- `C`: pause/resume adult cattle idle/body animation
- `R`: pause/resume both running calves
- `H`: pause/resume grazing/feeding head movement
- `M`: advance the worker task: go to cows, feed, then go home
- `K`: send the worker directly home
- `L`: recall/release calves, chickens and ducks to/from their shelters
- `F`: pause/resume both shed fans
- `1`: toggle directional sunlight
- `2`: toggle all three point lights
- `3`: toggle the entrance spotlight
- `4` (or numpad `4`): switch between day and night; the window title confirms
  the active mode and whether the night lamps are enabled
- `5`: toggle the ambient lighting component
- `6`: toggle the diffuse lighting component
- `7`: toggle the specular lighting component
- `Esc`: close the application

## Texture assets

- `grass.png`: repeating main field and entrance verge
- `soil.png`: repeating dirt road and forecourt
- `wood.png`: fences, sign, beams, frames, crates and tool rack
- `brick.png`: entrance pillars, shed walls and barn walls
- `roof.png`: corrugated shed/barn roofs and tank cap
- `metal.png`: gate, structural posts, fans and water tower
- `concrete.png`: floors, troughs, foundations and reserved platform
- `hay.png`: hay bales and feed-sack approximations
- `bark.png`: tree trunks
- `leaves.png`: opaque geometric tree canopies

All textures are loaded once at startup, use mipmaps and relative paths, and
fall back to a visible checker if loading fails. `GL_REPEAT`,
`GL_MIRRORED_REPEAT`, and `GL_CLAMP_TO_EDGE` are all represented.

## Visible final scene

- textured 36 x 36 grass farm and aligned dirt road
- repeated wooden boundary fence
- brick/concrete entrance pillars, textured animated gate and farm sign
- detailed open cow shed with floor, walls, posts, beams and sloped roof
- two tied indoor cows, divided stalls, two fodder troughs and visible ropes
- two independently rotating hierarchical fans and aligned emissive fixtures
- enclosed feed barn with doorway, windows and sloped roof
- seven varied hay bales, three crates and three feed sacks
- eight reusable trees
- elevated geometric water tank with supports and ladder
- exactly two adult cattle: one feeding milch cow and one ox
- two calves running smoothly on separate elliptical paths
- one command-driven worker and a dedicated worker house
- daytime free-running calves, chickens and ducks
- a small open-front poultry shed with a seated hen and chicks
- night shelter routine: all animals stop and move inside their shelters
- daytime shelter command with smooth return/release navigation
- animated rear stall gates for inspecting the milch cow and ox
- cubic Bézier surface-of-revolution milk can on the collection platform
- cubic uniform B-spline irrigation pipe beside the water facility
- sampled ruled-surface cow-shed roof replacing the old planar roof
- collision-aware camera movement around walls, fences, trees and equipment
- bird's-eye, front, side and top orthographic technical cameras
- four-view mode with one perspective and three orthographic views
- tool rack and an empty milk/curve-object platform for a later module

All moving parts use delta time. Model roots control world translation/yaw,
while legs, heads, tails, arms and fan blades animate around local joint pivots.
All primitive and curved meshes are generated once with positions, normals,
UVs and indexed triangles. Animation updates once before rendering, so enabling
four views does not multiply movement speed.

## Quick test

1. Confirm the console reports that all textures loaded and the day scene is sunlit.
2. Inspect the entrance and press `T` twice to compare textured and tint-only rendering.
3. Press `G` and verify both leaves open inward around their pillars.
4. Walk through the gate and inspect texture repetition on grass, road, fences and walls.
5. Press `M` repeatedly: the worker walks to the shed, waits, feeds, then returns home; test `K` during the sequence.
6. Press `L`, wait for the mobile animals to reach their shelters, then release them with `L` again.
7. Check that cow legs, heads and tails and worker arms/legs stay attached at their pivots.
8. Visit the shed; verify the fan mounts remain fixed while only the rotors turn.
9. Enter the shed and verify two tied cows, green fodder, bulbs and fans are visible.
10. Press `4`; verify the sky darkens and artificial fixtures dominate the night scene.
11. At night test `1`, `2`, and `3`, then independently test Phong components with `5`, `6`, and `7`.
12. Inspect the gate, fan and water facility for stronger metal highlights than grass/hay.
13. Inspect the Bézier milk can, spline pipe and curved ruled roof.
14. Press `B` and confirm the whole farm appears in an undistorted top view.
15. Press `V` and compare free, top, front and side views simultaneously.
16. Open the gate and verify the free camera cannot pass through closed walls,
    fences, trees, troughs or other listed obstacles.
17. During day confirm calves and mobile poultry move outside; press `4` and
    confirm they stop and relocate to the cow/poultry shelters.
18. With the entrance gate closed, walk toward it at full speed and confirm the
    swept collision prevents passing through; open it and use the clear center gap.
19. Press `O`, enter the opened rear stall gates, and inspect both cattle from behind.
