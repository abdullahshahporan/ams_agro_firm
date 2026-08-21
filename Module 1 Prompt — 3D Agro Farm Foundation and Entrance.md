I am developing a Computer Graphics project in **C++ using Modern OpenGL** for my **CSE 4208 Graphics Lab**. I have provided the course resources containing Lab 1–Lab 5 examples.

The final project will be an **Interactive 3D Agro Farm Simulation**, but it will be developed progressively in five modules.

For now, implement **ONLY MODULE 1: Farm Foundation, Entrance, Camera, and Animated Gate**.

Do not implement the later modules yet.

---

# 1. COURSE RESOURCE REQUIREMENT

Study and follow the coding style and OpenGL concepts available in my supplied course resources.

Relevant resources include:

- Lab 1 — OpenGL primitives and transformations
- Lab 2 — 3D drawing, view/projection, depth testing, camera
- Lab 3 — later used for lighting
- Lab 4 — later used for textures
- Lab 5 — contains a better free-camera implementation and will later be used for curved objects

For Module 1:

- Use Modern OpenGL.
- Use GLFW.
- Use GLAD.
- Use GLM.
- Use VAO/VBO/EBO appropriately.
- Enable depth testing.
- Use model, view, and projection matrices.
- Use transformation matrices for all scene objects.
- Prefer the free-camera structure from the supplied Lab 5 `camera.h`.
- Keep the implementation consistent with the libraries/setup already supplied with the course materials.

Do not introduce unnecessary frameworks, game engines, model-loading libraries, physics engines, ImGui, Assimp, or other third-party dependencies.

---

# 2. FINAL PROJECT CONTEXT

The final scene will eventually contain:

- agro farm entrance
- animated double gate
- green grass field
- road
- farm boundary fence
- cow shed
- barn/feed store
- feeding trough
- trees
- hay
- water tank/silo
- adult cows
- calves
- farm workers
- ceiling fans
- lights
- animal animations
- human animations
- textures
- Phong lighting
- day/night mode
- Bézier/spline/ruled surfaces
- bird's-eye view
- multiple viewports

However, **DO NOT implement all of these now**.

Module 1 must establish the coordinate system, scene architecture, camera system and entrance area on which all later modules will be built.

---

# 3. MODULE 1 VISIBLE RESULT

When I compile and run Module 1, I must NOT see an empty test window or only one cube.

I should immediately see a recognizable early-stage agro farm.

The initial camera should be positioned **outside the farm entrance**, facing toward the farm.

The scene should visibly contain:

1. a large green farm ground
2. a central dirt-colored entrance road
3. front farm boundary fence
4. left/right side boundary fences
5. two strong entrance pillars
6. a double farm gate
7. a signboard structure above or near the entrance
8. simple fence posts/rails
9. a few simple background objects indicating where the farm buildings will eventually be
10. a simple cow-shed placeholder in the background
11. a simple barn/feed-store placeholder in the background
12. a clear sky/background color

The shed and barn in Module 1 should remain intentionally simple geometric structures. They will be replaced/improved in Module 2.

There should be **no cow, calf or human model yet**.

---

# 4. WORLD COORDINATE PLAN

Use a consistent coordinate system so later modules can be added without repositioning everything.

Use approximately:

```text
X axis = left/right
Y axis = vertical/up
Z axis = forward/backward
```

Let the farm occupy roughly:

```text
X = -18 to +18
Z = -18 to +18
```

Ground level:

```text
Y = 0
```

The farm entrance should be at approximately:

```text
Z = +15.5
```

The back of the farm should extend toward:

```text
Z = -18
```

The camera starts outside the farm at approximately:

```cpp
glm::vec3(0.0f, 3.0f, 25.0f)
```

and initially looks toward the farm/negative Z direction.

Use these approximate major locations:

```text
Entrance gate:
(0, 0, +15.5)

Simple cow-shed placeholder:
(-10, 0, -7)

Simple barn/feed-store placeholder:
(+10, 0, -10)

Central road:
from entrance toward central farm

Main field:
inside the entire farm boundary
```

Small changes to coordinates are allowed if needed for proportions.

---

# 5. GREEN FARM GROUND

Create a large flat farm ground approximately:

```text
36 × 36 OpenGL world units
```

covering:

```text
X = -18 to +18
Z = -18 to +18
```

Use either a very thin scaled cube or a proper plane.

For Module 1, use a natural green color.

Do NOT add the grass texture yet. Texture mapping belongs to Module 2.

The ground must be large enough that when I enter the farm, the world does not immediately end.

---

# 6. ENTRANCE ROAD

Create a road from outside the gate toward the middle of the farm.

The road should be approximately:

```text
width = 4–5 units
```

It should start slightly outside the entrance and extend inward.

Use a soil/dirt-like brown color for Module 1.

The road can be a very thin rectangular cuboid positioned slightly above the grass plane to avoid z-fighting.

Example concept:

```text
          FARM INTERIOR

              ||
              ||
              ||
              ||
           ROAD
              ||
              ||
        ===== GATE =====

          OUTSIDE
```

Do not use textures yet.

---

# 7. FARM BOUNDARY

Create a visible boundary around the farm using repeated fence components.

Do not construct the entire fence as one huge cube.

Create reusable functions/components such as:

```cpp
drawFencePost(...)
drawFenceSection(...)
drawFarmBoundary(...)
```

A simple fence section can consist of:

- vertical posts
- two or three horizontal rails

Use repeated translation transformations to place multiple sections.

Create:

- front-left fence
- front-right fence
- left-side fence
- right-side fence
- optionally a simpler back fence

Leave a proper opening in the front center for the entrance gate.

The fence should not block the gate opening.

Use a wood/brown color in Module 1.

---

# 8. ENTRANCE PILLARS

Create two entrance pillars positioned on the two sides of the gate opening.

Suggested positions:

```text
Left pillar:
X ≈ -4.3

Right pillar:
X ≈ +4.3

Z ≈ +15.5
```

The pillars should be approximately:

```text
height = 4–5 units
```

Use multiple cuboids if necessary so they look more interesting than plain boxes.

For example:

```text
      _____
     |_____|
     |     |
     |     |
     |     |
     |_____|
```

Give the pillars a stone/brick-like color for now.

Actual brick texture will be added in Module 2.

---

# 9. DOUBLE FARM GATE

This is the most important object in Module 1.

Create a proper **double gate**.

The gate should have:

- one left gate leaf
- one right gate leaf
- vertical bars
- horizontal supporting bars
- appropriate proportions
- separate hinge locations

Do not create each gate leaf as one featureless cube.

Construct the leaf using multiple thin cuboids so that it visibly resembles a metal/wood farm gate.

Example:

```text
PILLAR                                PILLAR
  ██                                    ██
  ██  | | | | | |      | | | | | |    ██
  ██  | | | | | |      | | | | | |    ██
  ██  ===========      ===========      ██
  ██  | | | | | |      | | | | | |    ██
```

Use hierarchical transformation so that each complete gate leaf rotates around its **hinge**.

Do NOT rotate the gate around the center of the leaf.

---

# 10. GATE PIVOT TRANSFORMATION

For each gate leaf, apply transformations in the correct hierarchical order.

Conceptually:

```text
Translate to hinge
        ↓
Rotate around Y-axis
        ↓
Translate local gate geometry away from hinge
        ↓
Draw all gate bars
```

For the left gate:

```cpp
glm::mat4 leftGateParent = glm::mat4(1.0f);

leftGateParent =
    glm::translate(
        leftGateParent,
        leftHingePosition
    );

leftGateParent =
    glm::rotate(
        leftGateParent,
        glm::radians(leftGateAngle),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
```

Then every bar of the left gate must be drawn relative to `leftGateParent`.

Do the corresponding mirrored transformation for the right gate.

When closed, both gate leaves should meet near the center.

When opened, they should swing inward toward the farm rather than rotating through their own centers.

---

# 11. GATE ANIMATION

Pressing:

```text
G
```

must toggle the gate between:

```text
OPEN
and
CLOSED
```

The gate must open smoothly.

Do NOT instantly change from:

```text
0 degrees → 90 degrees
```

Use an animation state variable such as:

```cpp
bool gateOpen = false;
float gateAngle = 0.0f;
```

and update using `deltaTime`.

For example:

```cpp
if (gateOpen)
    gateAngle += gateSpeed * deltaTime;
else
    gateAngle -= gateSpeed * deltaTime;
```

Clamp the result approximately to:

```text
0° ≤ gateAngle ≤ 90°
```

Use a reasonable speed such as:

```text
50–70 degrees/second
```

The animation must be frame-rate independent.

Do not use hard-coded frame increments such as:

```cpp
gateAngle += 0.01f;
```

---

# 12. INPUT HANDLING FOR GATE

Use a key callback or another proper single-press mechanism for toggling.

Do NOT repeatedly toggle the boolean every frame while `G` remains held.

Correct behavior:

```text
Press G once
→ gate starts opening.

Press G again
→ gate starts closing.
```

Use:

```cpp
GLFW_PRESS
```

event logic for toggle-type commands.

Continuous camera movement can remain inside `processInput()`.

---

# 13. CAMERA SYSTEM

Use a proper free 3D camera based on the supplied Lab 5 camera approach.

Required controls:

```text
W = move forward
S = move backward
A = move left
D = move right

Mouse = look around

Mouse wheel = zoom

Q = move downward
E = move upward
```

The camera must use `deltaTime`.

Initial camera:

```cpp
glm::vec3 cameraPosition(
    0.0f,
    3.0f,
    25.0f
);
```

Initial direction should face toward the farm.

The user should be able to:

1. see the farm entrance;
2. press `G`;
3. watch the gate open;
4. move through the gate;
5. walk along the road;
6. inspect the simple farm area.

Do not add bird's-eye/multiple-view mode yet. Those belong to Module 5.

---

# 14. PERSPECTIVE PROJECTION

Use perspective projection.

Use reasonable values similar to:

```cpp
FOV = camera.Zoom
aspect = SCR_WIDTH / SCR_HEIGHT
near = 0.1f
far = 100.0f or greater
```

Handle window resizing correctly.

Enable:

```cpp
glEnable(GL_DEPTH_TEST);
```

All objects must occlude correctly.

---

# 15. SIMPLE BACKGROUND COW SHED PLACEHOLDER

Inside the farm, create a very simple cow-shed shape around approximately:

```text
(-10, 0, -7)
```

This is NOT the final cow shed.

It only establishes the visual composition.

Use simple geometry:

- four/six posts
- basic walls or partial walls
- simple flat/sloped roof

Keep it relatively open so it resembles a shed rather than a solid house.

Do not add:

- fans
- lights
- textures
- cows

yet.

Those belong to later modules.

---

# 16. SIMPLE BARN / FEED-STORE PLACEHOLDER

Create a simple barn/feed-store structure approximately:

```text
(+10, 0, -10)
```

Use:

- rectangular main body
- simple roof
- visible doorway/opening

It must appear clearly different from the cow shed.

Again, this is only the Module 1 placeholder.

Detailed construction and textures will be completed in Module 2.

---

# 17. FARM SIGN STRUCTURE

Create a visible farm signboard around the entrance.

It can be above the gate or positioned beside the entrance.

For Module 1 it is acceptable to create:

- two supports
- one rectangular signboard

Do not introduce a font-rendering library merely to write text.

If text cannot be created using the existing OpenGL resources, leave the signboard as a clear geometric sign surface that can receive a texture in Module 2.

Avoid adding unnecessary external dependencies.

---

# 18. BASIC COLORS

Module 1 should use vertex/object colors rather than textures.

Use approximately natural colors:

```text
Grass:
green

Road:
earthy brown

Fence:
wood brown

Gate:
dark gray / dark brown

Pillars:
light gray / brick-like red

Cow shed:
brown/gray

Barn:
dark red/brown

Roof:
dark gray

Sign:
wood color
```

Do not spend time creating realistic materials yet.

The purpose of Module 1 is geometry, transformations, scene layout and camera.

---

# 19. LIGHTING FOR MODULE 1

Do NOT implement the final lighting system yet.

Module 4 will contain:

- directional sunlight
- point lights
- spotlight
- emissive lights
- Phong lighting
- attenuation
- day/night mode
- ambient/diffuse/specular toggles

For Module 1, use a simple color shader that makes all geometry clearly visible.

Do not make Module 1 dependent on advanced lighting code.

---

# 20. NO TEXTURES IN MODULE 1

Do not implement grass, wood, brick or metal texture mapping yet.

Texture mapping belongs to Module 2.

However, organize the program so that later shader/texture support can be added without rewriting the entire scene.

---

# 21. NO ANIMALS OR HUMANS YET

Do NOT create:

- cows
- calves
- humans
- animal walking
- grazing
- running
- human animation

in Module 1.

These will be implemented with hierarchical modeling in Module 3.

The only main animation required in Module 1 is:

```text
animated entrance gate
```

---

# 22. PROGRAM ARCHITECTURE

Do not write the entire project as one huge render loop.

Organize the code so later modules can be inserted cleanly.

Use functions approximately like:

```cpp
void processInput(GLFWwindow* window);
void updateAnimations(float deltaTime);

void renderScene(Shader& shader);

void drawGround(Shader& shader);
void drawRoad(Shader& shader);

void drawFarmBoundary(Shader& shader);
void drawFenceSection(...);

void drawEntrance(Shader& shader);
void drawPillar(...);

void drawGate(Shader& shader);
void drawGateLeaf(...);

void drawFarmSign(Shader& shader);

void drawCowShedPlaceholder(Shader& shader);
void drawBarnPlaceholder(Shader& shader);
```

You may improve these function signatures where appropriate.

The important requirement is modularity.

---

# 23. SCENE RENDERING STRUCTURE

The main loop should conceptually remain clean:

```cpp
while (!glfwWindowShouldClose(window))
{
    // calculate deltaTime

    // input

    // update animations

    // clear screen/depth buffer

    // create projection
    // obtain camera view matrix

    // activate shader

    // render complete scene

    // swap buffers
    // poll events
}
```

Use:

```cpp
renderScene(shader);
```

as the central scene drawing function.

Later modules will add objects inside this same function.

---

# 24. REUSABLE CUBE/PRIMITIVE

Since most Module 1 objects can be constructed from cuboids, use a reusable cube mesh.

The cube should be initialized only once.

Do NOT generate a new VAO/VBO every time a fence post, gate bar or building block is drawn.

Use one cube VAO and render it repeatedly with different model matrices.

For example:

```text
same cube geometry
       ↓
different translation
different rotation
different scale
different color
```

This principle should be used for:

- ground if using thin cube
- road
- fence posts
- fence rails
- gate bars
- entrance pillars
- building walls
- roofs/signboard as appropriate

---

# 25. TRANSFORMATION HELPER

Create a convenient reusable drawing function such as conceptually:

```cpp
drawCube(
    shader,
    position,
    rotation,
    scale,
    color
);
```

or an equivalent model-matrix-based function.

It should:

1. create a model matrix;
2. apply translation;
3. apply required rotations;
4. apply scale;
5. send the model matrix to the shader;
6. send object color;
7. draw the existing cube VAO.

For hierarchical objects such as the gate, support receiving a parent matrix.

For example:

```cpp
drawCubeWithParent(
    shader,
    parentMatrix,
    localPosition,
    localRotation,
    localScale,
    color
);
```

This will later be extremely useful for cows, calves, humans and fans.

---

# 26. DELTA TIME

Create:

```cpp
float deltaTime = 0.0f;
float lastFrame = 0.0f;
```

and calculate it correctly every frame.

Use it for:

- camera movement
- gate animation

Later modules will also use the same value for:

- cows
- calves
- humans
- fans

Do not create separate incorrect timing systems.

---

# 27. FILE ORGANIZATION

Keep the code ready for future expansion.

A suitable Module 1 structure can be:

```text
AgroFarm/
│
├── main.cpp
├── shader.h
├── camera.h
│
├── shaders/
│   ├── basicColor.vs
│   └── basicColor.fs
│
└── include/
    └── existing OpenGL/course dependencies
```

If it is cleaner, an additional file such as:

```text
farmScene.h
```

or

```text
primitives.h
```

can be introduced.

Do not over-engineer the project with unnecessary classes at this stage.

---

# 28. DO NOT BREAK THE COURSE SETUP

Use the OpenGL setup compatible with the files I supplied.

Do not assume CMake if the course project is Visual Studio based.

Do not change GLFW/GLAD/GLM versions unnecessarily.

Do not rewrite working course dependency files.

Do not require me to install a large new dependency merely to run Module 1.

---

# 29. WINDOW

Use a reasonable window size such as:

```text
1200 × 800
```

or:

```text
1280 × 720
```

Window title:

```text
Interactive 3D Agro Farm - Module 1
```

Set a natural light-blue sky-like clear color.

---

# 30. CONSOLE INSTRUCTIONS

When the application starts, print something similar to:

```text
========================================
3D AGRO FARM - MODULE 1
========================================
W/A/S/D : Move Camera
Mouse   : Look Around
Q/E     : Move Down/Up
Scroll  : Zoom
G       : Open/Close Farm Gate
ESC     : Exit
========================================
```

This makes testing easier.

---

# 31. MODULE 1 SCENE COMPOSITION

The scene should visually communicate depth.

When the program starts, the user should see approximately:

```text
                 distant farm buildings

             shed                 barn

       =================================
              green farm land

                    road
                     |
                     |
                     |
 Fence ------------ | ------------- Fence

       Pillar     Double Gate     Pillar
          |         |  |            |
          |         |  |            |

                 Camera
              outside farm
```

Do not place every object on the same depth plane.

The entrance should dominate the foreground while farm structures remain visible deeper inside the scene.

---

# 32. VISUAL PROPORTIONS

Avoid obviously unrealistic proportions.

For example:

- entrance pillar taller than gate
- gate roughly human/farm-vehicle scale
- fence lower than entrance pillar
- road wide enough for a farm vehicle
- barn larger than the gate
- shed broad and low
- camera around human eye/standing height when exploring

Exact real-world scale is not mandatory, but relative proportions should feel believable.

---

# 33. IMPORTANT CODE QUALITY RULES

The code must:

- compile;
- contain no pseudocode in place of implementation;
- contain no undefined functions;
- contain no missing variables;
- avoid duplicated geometry initialization;
- avoid creating buffers every frame;
- use meaningful names;
- use comments for important transformations;
- use `deltaTime`;
- use correct matrix order;
- use hierarchical gate rotation;
- keep rendering separate from animation updating;
- leave a clean structure for Modules 2–5.

---

# 34. MODULE 1 MUST NOT INCLUDE

Do NOT prematurely add:

```text
textures
Phong lighting
Gouraud shading
directional light
point lights
spotlights
day/night
cows
calves
humans
fan animation
Bezier surfaces
B-spline surfaces
ruled surfaces
four viewports
bird's-eye mode
OBJ models
Assimp
physics
collision system
GUI framework
```

Those will be added systematically in later modules.

Module 1 must remain stable and understandable.

---

# 35. FUTURE COMPATIBILITY

Design Module 1 knowing that later I will add:

### Module 2
Detailed farm environment and textures.

### Module 3
Cows, calves, humans and hierarchical animation.

### Module 4
Phong lighting and day/night environment.

### Module 5
Bézier/spline/ruled surfaces, multiple views and final polish.

Therefore, avoid architecture that would require deleting and rewriting Module 1 later.

Module 1 should be the permanent foundation of the final project.

---

# 36. EXPECTED MODULE 1 DEMONSTRATION

After implementation, the following exact demonstration should work:

### Step 1
Run the program.

The camera is outside the farm entrance.

### Step 2
I can see:

- green farm ground;
- entrance road;
- fence;
- two entrance pillars;
- closed double gate;
- sign structure;
- simple shed in distance;
- simple barn in distance.

### Step 3
Move the mouse.

The camera orientation changes naturally.

### Step 4
Use `W/A/S/D`.

The camera moves through the 3D environment.

### Step 5
Press:

```text
G
```

The left and right gates smoothly rotate around their own hinges and open inward.

### Step 6
Press `W`.

The camera can travel through the newly opened entrance and into the farm.

### Step 7
Press:

```text
G
```

again.

The gates smoothly close.

The gate animation must remain correct even when viewed from another camera angle.

---

# 37. ACCEPTANCE CHECKLIST

Do not consider Module 1 finished unless all of these are working:

- OpenGL window works.
- Depth testing works.
- Perspective works.
- Free camera works.
- Mouse look works.
- Zoom works.
- Green farm ground exists.
- Road exists.
- Farm boundary exists.
- Entrance pillars exist.
- Double gate visibly resembles a gate.
- Gate leaves meet correctly while closed.
- Left leaf rotates around left hinge.
- Right leaf rotates around right hinge.
- Gate animation is smooth.
- Animation uses deltaTime.
- `G` toggles correctly on individual presses.
- Simple cow-shed placeholder is visible.
- Simple barn placeholder is visible.
- User can enter the farm using the camera.
- Scene functions are modular.
- Reusable cube geometry is initialized once.
- No later-module features have been unnecessarily implemented.

---

# 38. WHAT I WANT FROM YOU

Now implement **Module 1 only**.

First inspect the supplied course code carefully, especially its:

- shader handling;
- camera handling;
- OpenGL initialization;
- VAO/VBO structure;
- model/view/projection conventions.

Then provide the complete implementation.

Do not give only an explanation or isolated snippets.

Give me the **complete compile-ready Module 1 code**, file by file, including any shader files that must be created or modified.

For every file, clearly state its filename.

Make sure all files are mutually consistent.

Do not omit code by writing phrases such as:

```text
// existing code here
// same as before
// implement later
```

unless the omitted portion truly belongs to Modules 2–5 and is not required for Module 1 to compile.

At the end, briefly provide:

1. the resulting file structure;
2. controls;
3. which supplied course files/concepts were reused;
4. exactly what I should see when Module 1 runs;
5. a short checklist for testing Module 1 before proceeding to Module 2.

The priority is **correctness, modularity, course compatibility, and a visually recognizable first portion of the final Agro Farm scene**.