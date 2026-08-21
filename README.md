# AMS Agro Farm — Module 2

Module 2 extends the working Module 1 entrance and camera into a detailed,
textured cattle-farm environment. It uses Modern OpenGL 3.3, GLFW, GLAD, GLM,
and the course Lab 4 `stb_image` implementation.

## Build and run

### VS Code Run button

1. In VS Code, open the `ams_agro_farm` folder itself as the workspace.
2. Open `main.cpp`.
3. Click the top-right triangle and choose **Run Code**. Subsequent clicks use
   the same project build-and-run command.

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
- `G`: smoothly open or close both gate leaves
- `T`: toggle texture sampling on/off for demonstration
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

## Visible Module 2 scene

- textured 36 x 36 grass farm and aligned dirt road
- repeated wooden boundary fence
- brick/concrete entrance pillars, textured animated gate and farm sign
- detailed open cow shed with floor, walls, posts, beams and sloped roof
- two feeding troughs, two hierarchical static fans and three lamp fixtures
- enclosed feed barn with doorway, windows and sloped roof
- seven varied hay bales, three crates and three feed sacks
- eight reusable trees
- elevated geometric water tank with supports and ladder
- tool rack and an empty milk/curve-object platform for a later module

The grazing center and reserved cow positions remain open. No animals, humans,
fan animation, advanced lighting, curved surfaces, or multiple views are part
of Module 2.

## Quick test

1. Confirm the console reports that all textures loaded.
2. Inspect the entrance and press `T` twice to compare textured and tint-only rendering.
3. Press `G` and verify both leaves open inward around their pillars.
4. Walk through the gate and inspect texture repetition on grass, road, fences and walls.
5. Visit the open shed; verify the two troughs, two stationary fans and fixtures.
6. Visit the barn; verify its real door opening, windows, crates and feed sacks.
7. Inspect the hay, trees and elevated water facility from several directions.
