

Module 1 has already been completed.

Module 1 currently contains:

- Modern OpenGL setup
- GLFW
- GLAD
- GLM
- depth testing
- perspective projection
- free camera
- mouse look
- zoom
- green farm ground
- entrance road
- farm boundary fence
- entrance pillars
- double farm gate
- smooth gate opening/closing using hierarchical transformation
- farm sign structure
- simple cow-shed placeholder
- simple barn/feed-store placeholder
- modular `renderScene()`
- reusable cube geometry
- delta-time-based animation
- `G` key gate control

Now implement:

# MODULE 2 — COMPLETE FARM ENVIRONMENT, STATIC FARM OBJECTS, AND TEXTURE MAPPING

Module 2 must extend the existing Module 1 project.

Do NOT start a completely new project.

Do NOT remove or break any working Module 1 feature.

---

# 1. PRIMARY MODULE 2 GOAL

After Module 2, the scene should no longer look like an early geometric prototype.

It should look like a recognizable, detailed **3D agro/cattle farm environment**, even though cows, calves, humans, and advanced animations have not yet been added.

When the application launches:

1. The camera starts outside the farm.
2. The textured entrance is visible.
3. The gate can still be opened using `G`.
4. The user can walk through the entrance.
5. Inside the farm, the user should see a complete farm environment containing:
   - detailed cow shed
   - detailed barn/feed store
   - grazing field
   - dirt road
   - feeding troughs
   - water area
   - hay/straw stacks
   - trees
   - wooden fencing
   - silo or water tower
   - farm equipment/decorative structures
   - static fan structures under the shed
   - textured buildings and terrain
6. The environment should look sufficiently complete that Module 3 can concentrate almost entirely on animals, humans and animation.

---

# 2. IMPORTANT DEVELOPMENT RULE

Continue from the existing Module 1 codebase.

Do NOT rewrite:

- camera system
- gate animation
- OpenGL initialization
- cube VAO/VBO
- existing input architecture
- delta time system
- basic scene positioning

unless a small correction is absolutely required.

New code should integrate into the existing architecture.

The final structure should still conceptually resemble:

```cpp
while (!glfwWindowShouldClose(window))
{
    updateDeltaTime();

    processInput(window);

    updateAnimations(deltaTime);

    renderScene();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Module 2 should mainly expand:

```cpp
renderScene();
```

and introduce proper texture/material handling.

---

# 3. COURSE RESOURCE REQUIREMENT

Carefully inspect and reuse concepts/code patterns from my provided course ZIP.

Module 2 should especially use the material from:

- Lab 1 — transformations and primitive geometry
- Lab 2 — 3D scene construction
- Lab 4 — texture mapping
- Lab 4 — `stb_image`
- Lab 4 — texture loading
- Lab 4 — UV coordinates
- Lab 4 — texture wrapping
- Lab 4 — texture filtering
- Lab 4 — textured cube/surface rendering

Use the supplied course implementation where appropriate rather than introducing unrelated libraries.

Do NOT use:

- Assimp
- Blender runtime
- OBJ loaders
- game engines
- Unity
- Unreal
- external physics engines
- ImGui
- unnecessary GUI systems

Everything should remain compatible with the supplied Modern OpenGL setup.

---

# 4. MODULE 2 SCENE TARGET

By the end of Module 2, the overall farm layout should approximately be:

```text
                    BACK OF FARM
                    Z ≈ -18

 ----------------------------------------------------------
|                                                          |
|  TREE     COW SHED                         FEED BARN      |
|          +----------------+              +----------+     |
|          |                |              |          |     |
|          |   TROUGHS      |              | FEED /   |     |
|          |                |              | STORAGE  |     |
|          +----------------+              +----------+     |
|                                                          |
|           HAY                     WATER TANK / SILO       |
|           HAY                           ||                |
|                                         ||                |
|                    GRAZING FIELD                          |
|                                                          |
|   TREE      TREE                         TREE             |
|                                                          |
|---------------------- FARM ROAD -------------------------|
|                         ||                               |
|                         ||                               |
|                         ||                               |
| FENCE     PILLAR     DOUBLE GATE      PILLAR     FENCE  |
 ----------------------------------------------------------

                        FRONT
```

This does not need to be architecturally exact, but object placement should feel intentional and realistic.

Do not randomly scatter objects.

---

# 5. PRESERVE MODULE 1 WORLD COORDINATES

Keep approximately:

```text
Farm:
X = -18 to +18
Z = -18 to +18

Ground:
Y = 0

Entrance:
Z ≈ +15.5
```

Maintain the existing positions of:

- gate
- pillars
- road
- boundary
- shed region
- barn region

unless a small adjustment improves composition.

Recommended final major areas:

```text
Cow shed:
X ≈ -12 to -4
Z ≈ -12 to -3

Grazing area:
X ≈ -3 to +8
Z ≈ -12 to +4

Barn/feed store:
X ≈ +8 to +15
Z ≈ -14 to -7

Water/silo:
X ≈ +11 to +15
Z ≈ -2 to +5

Hay:
X ≈ -10 to -4
Z ≈ +1 to +6

Entrance:
centered at X ≈ 0
Z ≈ +15.5
```

The exact values may be adjusted slightly.

---

# 6. MODULE 2 MAJOR IMPLEMENTATION PHASES

Implement Module 2 in a planned sequence.

Do not attempt every object simultaneously.

Use this order:

## Phase 2.1
Texture system

## Phase 2.2
Textured ground and road

## Phase 2.3
Detailed entrance and fence

## Phase 2.4
Detailed cow shed

## Phase 2.5
Detailed barn/feed store

## Phase 2.6
Feeding area

## Phase 2.7
Hay/straw area

## Phase 2.8
Trees and vegetation

## Phase 2.9
Water tank/silo

## Phase 2.10
Static fan structures

## Phase 2.11
Additional farm props

## Phase 2.12
Scene optimization and polish

The application should remain runnable after every phase.

---

# 7. TEXTURE SYSTEM

The most important technical addition in Module 2 is proper texture mapping.

Use:

```cpp
stb_image.h
```

from the supplied course resources.

Create or reuse a texture loading function such as:

```cpp
unsigned int loadTexture(
    const char* path,
    GLint wrapS,
    GLint wrapT,
    GLint minFilter,
    GLint magFilter
);
```

At minimum it should:

1. generate an OpenGL texture
2. load image data using `stbi_load`
3. determine RGB/RGBA format properly
4. upload using `glTexImage2D`
5. generate mipmaps
6. configure wrapping
7. configure filtering
8. free loaded image data
9. report a useful error if loading fails

Prefer:

```cpp
stbi_set_flip_vertically_on_load(true);
```

if required by the coordinate convention.

Do not repeatedly load the same texture every frame.

All textures must be loaded once during initialization.

---

# 8. TEXTURE RESOURCE MANAGEMENT

Create texture variables such as:

```cpp
unsigned int grassTexture;
unsigned int soilTexture;
unsigned int woodTexture;
unsigned int brickTexture;
unsigned int roofTexture;
unsigned int metalTexture;
unsigned int concreteTexture;
unsigned int hayTexture;
unsigned int barkTexture;
unsigned int leafTexture;
unsigned int doorTexture;
```

Load them once.

Use meaningful names.

Do not write:

```cpp
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;
```

for the entire project.

---

# 9. TEXTURE DIRECTORY

Organize texture assets clearly:

```text
AgroFarm/
│
├── textures/
│   ├── grass.jpg
│   ├── soil.jpg
│   ├── wood.jpg
│   ├── brick.jpg
│   ├── roof.jpg
│   ├── metal.jpg
│   ├── concrete.jpg
│   ├── hay.jpg
│   ├── bark.jpg
│   └── leaves.jpg
```

If existing textures are available in the supplied ZIP, reuse them where suitable.

Otherwise, use simple appropriate texture images.

Do not hard-code machine-specific absolute paths such as:

```text
C:\Users\MyName\Desktop\...
```

Use relative paths.

---

# 10. TEXTURED SHADER

Create a texture shader appropriate for Module 2.

Module 4 will later implement the full Phong lighting system.

Therefore, Module 2 should not unnecessarily implement all final lighting.

The Module 2 textured fragment shader can primarily sample:

```glsl
texture(texture1, TexCoord)
```

and optionally multiply by a basic color/tint.

For example:

```glsl
FragColor = texture(texture1, TexCoord);
```

or:

```glsl
FragColor =
    texture(texture1, TexCoord)
    * vec4(objectTint, 1.0);
```

Keep the shader architecture clean enough that Module 4 can later replace or extend it with Phong lighting.

---

# 11. VERTEX DATA REQUIREMENT

Any reusable textured primitive must have:

```text
position
texture coordinates
```

and preferably normals already prepared for Module 4.

Strong recommendation:

Even though full lighting is not implemented yet, design the textured cube vertex data as:

```text
position
normal
texture coordinate
```

for example:

```text
x y z
nx ny nz
u v
```

This prevents rebuilding all geometry in Module 4.

Use a stride such as:

```text
8 floats per vertex
```

where appropriate.

---

# 12. DO NOT CREATE DUPLICATE CUBE SYSTEMS UNNECESSARILY

Module 1 already has reusable cube geometry.

Refactor carefully if needed so that the same conceptual primitive can eventually support:

- object color
- texture
- normal
- model matrix

Do not maintain five completely independent cube VAOs unless technically necessary.

A good architecture might contain:

```cpp
drawColoredCube(...)
drawTexturedCube(...)
```

sharing sensible initialization patterns.

Do not break Module 1 while refactoring.

---

# 13. TEXTURE REPEATING

Large surfaces should NOT stretch a single texture over the entire object.

The grass field must repeat.

For example, use UV ranges such as:

```text
0 → 12
```

instead of:

```text
0 → 1
```

for a large ground plane.

Use:

```cpp
GL_REPEAT
```

for:

- grass
- soil
- brick walls
- wood fences where appropriate

This should make the scene look much more natural.

---

# 14. TEXTURE WRAPPING DEMONSTRATION

Since wrapping is part of the course material, ensure the project visibly uses or at least supports multiple wrapping modes.

At minimum use:

```cpp
GL_REPEAT
```

for repeating surfaces.

Use:

```cpp
GL_CLAMP_TO_EDGE
```

for suitable objects such as signboards or single decorative images.

Optionally use:

```cpp
GL_MIRRORED_REPEAT
```

on one suitable surface.

Do not force inappropriate wrapping merely to demonstrate it.

---

# 15. TEXTURE FILTERING

Use sensible defaults:

```cpp
GL_LINEAR_MIPMAP_LINEAR
```

or:

```cpp
GL_LINEAR_MIPMAP_NEAREST
```

for minification

and:

```cpp
GL_LINEAR
```

for magnification.

Optionally provide:

```text
N = nearest filtering
L = linear filtering
```

as a demonstration feature.

However, do not allow this optional feature to destabilize the project.

---

# 16. TEXTURED GRASS FIELD

Replace Module 1's plain green farm ground with a textured grass surface.

The ground must still occupy approximately:

```text
36 × 36
```

world units.

Use repeating grass texture.

Avoid obvious stretching.

The grass should remain slightly below roads and objects to avoid z-fighting.

Example:

```text
grass Y = 0.0
road Y = 0.01 or 0.02
```

Use appropriate depth precision.

---

# 17. DIRT FARM ROAD

Replace Module 1's plain brown road with a soil/dirt texture.

The road should begin outside the entrance and continue inward.

Make it look like a farm vehicle path.

Suggested layout:

```text
Entrance
   |
   |
   |
 central farm
```

Optionally branch slightly toward:

- cow shed
- barn

but keep the scene readable.

Use repeated soil texture.

---

# 18. ENTRANCE UPGRADE

Improve the Module 1 entrance while preserving gate animation.

Apply textures to:

### Pillars
- brick
or
- stone

### Gate
- metal
or
- wood

### Sign
- wood
or a farm-sign texture if available

### Fence
- wood

Do not change the gate hierarchy.

The existing left/right hinge animation must continue to work perfectly after texture conversion.

---

# 19. FARM SIGN

Improve the farm sign structure.

Possible sign:

```text
       _______________________
      |                       |
      |       AGRO FARM       |
      |_______________________|
             |       |
             |       |
```

If text rendering is not available through the course resources:

- use a texture image containing the words `AGRO FARM`
or
- use a plain wood sign surface

Do NOT add a complicated font-rendering framework just for this.

Texture-based signage is acceptable.

---

# 20. DETAILED COW SHED

Replace the simple Module 1 cow-shed placeholder with a proper open cattle shed.

Recommended design:

```text
        __________________________
       /_________________________/|
      /_________________________/ |
     |     |             |      | |
     |     |             |      | |
     |     |             |      | |
     |_____|_____________|______|/
```

The shed should have:

- foundation/floor
- vertical posts
- side/back walls where appropriate
- mostly open front
- roof
- beams
- interior space
- feeding area nearby
- room for future cows
- room for future fan objects
- room for future ceiling lights

The shed should be large enough to eventually contain approximately:

```text
3–4 adult cows
```

Do not place cows yet.

---

# 21. COW SHED MATERIAL PLAN

Use appropriate textures:

| Shed component | Texture |
|---|---|
| floor | concrete |
| pillars/posts | wood or metal |
| back wall | brick |
| side wall | brick/wood |
| roof | corrugated metal |
| beams | wood/metal |

Do not use the same texture on every part.

---

# 22. SHED ROOF FOR MODULE 2

For now, use a simple roof constructed from:

- scaled cubes
or
- simple slanted planar/cuboid geometry

Do NOT implement the final ruled curved roof yet.

The ruled-surface roof belongs to Module 5.

However, design the shed so Module 5 can replace or overlay the current roof cleanly.

---

# 23. ROOF ROTATION

If using sloped cuboids, rotate them correctly around the appropriate axis.

For example:

```cpp
glm::rotate(
    model,
    glm::radians(angle),
    glm::vec3(0.0f, 0.0f, 1.0f)
);
```

or another axis according to the roof direction.

Do not distort the roof using unrealistic scaling alone.

---

# 24. BARN / FEED STORE

Replace the Module 1 barn placeholder with a detailed feed-storage barn.

The barn should visually differ from the cow shed.

It should be more enclosed.

Include:

- four walls
- roof
- door opening
- optional windows
- feed-storage zone
- sacks or containers near it

Possible design:

```text
          /\
         /  \
        /____\
       | []  |
       |     |
       |  __ |
       | |  ||
       |_|__||
```

A simpler roof is acceptable if a triangular roof requires too much unnecessary custom geometry.

---

# 25. BARN TEXTURES

Use:

```text
brick / wood → walls
wood → doors
roof texture → roof
concrete → foundation
```

Make texture scale believable.

Brick should repeat.

Do not stretch one brick across an entire building.

---

# 26. FEEDING TROUGH AREA

Create one or more cattle feeding troughs near or inside the cow shed.

A trough can be constructed using:

- base
- two side walls
- two end walls

Use simple cuboids.

Example cross-section:

```text
\________/
```

If creating a slanted trough is difficult at this stage, a rectangular open container is acceptable.

Do not create advanced curved geometry yet.

Use:

```text
concrete
metal
or wood
```

texture.

Make the trough large enough for future cows to stand beside it.

---

# 27. PREPARE COW POSITIONS

Even though animals are not drawn in Module 2, reserve logical cow positions.

For example:

```cpp
glm::vec3 futureCowPositions[] = {
    glm::vec3(-7.0f, 0.0f, -6.0f),
    glm::vec3(-5.0f, 0.0f, -8.0f),
    glm::vec3( 1.0f, 0.0f, -5.0f),
    glm::vec3( 4.0f, 0.0f, -3.0f)
};
```

They do not need to be rendered yet.

But avoid filling these locations with permanent scenery that would conflict with Module 3.

---

# 28. GRAZING AREA

Keep a large open region of grass for Module 3.

Do not clutter the entire farm.

The grazing region should have enough free space for:

- walking cows
- grazing cows
- running calves

Recommended zone:

```text
X ≈ -2 to +8
Z ≈ -11 to +6
```

Small decorative items may be added around its edges.

The center should remain open.

---

# 29. HAY / STRAW AREA

Create visible hay/straw objects.

Simplest acceptable version:

- rectangular hay bales

Construct several scaled cubes.

Use hay texture.

Arrange them naturally.

For example:

```text
      [ HAY ]
 [ HAY ][ HAY ]
```

Vary orientation slightly.

Do not place every hay bale with exactly the same rotation.

Example:

```text
0°
5°
-8°
```

Keep variation subtle.

---

# 30. HAY STACK POSITION

Place hay close to:

- barn
or
- cow shed

but not in the main road.

Suggested area:

```text
X ≈ -10 to -5
Z ≈ +1 to +5
```

or another visually appropriate location.

---

# 31. WATER AREA

Create a farm water facility.

For Module 2, choose one:

### Option A
Elevated water tank

### Option B
Simple silo-like water tank

### Option C
Ground water storage tank

Do not use Bézier surfaces yet.

The tank can be constructed from simple primitives.

If using only cuboids initially, create a stylized tank.

If a cylinder primitive already exists in the course resources and can be reused safely, it may be used.

---

# 32. SILO / WATER TOWER DESIGN

Recommended visual concept:

```text
          ________
        /          \
       |            |
       |    WATER   |
       |            |
       |____________|
          |      |
          |      |
          |      |
```

For Module 2, text is optional.

Support legs can be cuboids.

The tank may remain geometric.

Module 5 can later add a more advanced curved object nearby.

---

# 33. TREES

Create multiple trees around the farm boundary and open areas.

Trees should be reusable objects.

Create:

```cpp
drawTree(...)
```

A simple tree may consist of:

### Trunk
scaled cube or cylinder

### Foliage
several primitives

If a sphere is safely available from the supplied resources, it may be used for foliage.

Otherwise, use multiple scaled cubes/pyramids temporarily.

Do not create unnecessarily complex procedural trees.

---

# 34. TREE TEXTURES

Use:

```text
bark texture → trunk
leaf/green texture → foliage
```

If foliage texturing looks poor on primitive geometry, a strong green tint with a simple texture is acceptable.

The important requirement is consistency with the course's texture-mapping methods.

---

# 35. TREE PLACEMENT

Do not create a forest.

Use approximately:

```text
5–10 trees
```

depending on performance and appearance.

Place them mainly:

- along boundaries
- near corners
- beside shed/barn
- outside entrance

Do not place trees:

- directly on the road
- through buildings
- in cow-animation paths

---

# 36. STATIC FAN STRUCTURES

Module 3 will animate the fans.

Module 2 should create the actual fan geometry.

Place approximately:

```text
2 fans
```

under the cow-shed roof.

Each fan should contain:

```text
central hub
3 or 4 blades
support rod if appropriate
```

For example:

```text
        blade
          |
blade ----O---- blade
          |
        blade
```

Use:

- metal texture
or
- dark/gray color

Do NOT rotate the fans continuously yet.

However, build each fan hierarchically so Module 3 can animate it by changing a parent angle.

Recommended architecture:

```cpp
void drawFan(
    Shader& shader,
    glm::vec3 position,
    float rotationAngle
);
```

In Module 2:

```cpp
rotationAngle = 0.0f;
```

Module 3 will update it.

---

# 37. FAN PIVOT REQUIREMENT

Construct blades relative to the fan center.

Do not hard-code every blade in world coordinates.

Use:

```text
Fan parent
   ↓
center rotation
   ↓
individual blade local transformation
```

This is essential for easy animation later.

---

# 38. FARM LIGHT FIXTURES — STRUCTURE ONLY

Create visible bulb/lamp fixtures in logical locations:

- cow shed
- barn entrance
- farm gate

However:

Do NOT implement advanced point light or spotlight behavior yet.

Module 4 will convert these fixtures into actual light sources.

For now, create geometry representing:

- hanging bulb
- lamp housing
- entrance lamp

This prepares the environment for Module 4.

---

# 39. ADDITIONAL FARM PROPS

Add a limited number of static props to make the environment believable.

Good options:

- feed sacks
- wooden crates
- buckets
- small tool box
- stacked wooden pallets
- simple benches
- milk collection platform
- small fence around water area

Do not overload the scene.

Use approximately:

```text
3–5 prop categories
```

with repeated instances.

---

# 40. FEED SACKS

A simple feed sack can initially be represented by a slightly scaled rectangular block.

Use a feed sack texture if available.

Place several near the barn.

Later curved models are optional but should not be implemented now.

---

# 41. WOODEN CRATES

Create reusable:

```cpp
drawCrate(...)
```

using cuboids.

Apply wood texture.

Place:

- near barn
- inside storage area

Vary rotation slightly for realism.

---

# 42. FARM FENCE IMPROVEMENT

Upgrade Module 1's fence with wood texture.

Keep the reusable section system.

Use repeated texture carefully.

Fence construction should still use:

```cpp
drawFenceSection(...)
```

rather than dozens of unrelated hard-coded cube calls.

---

# 43. SCENE HIERARCHY

Organize `renderScene()` logically.

For example:

```cpp
void renderScene()
{
    drawTerrain();
    drawRoads();

    drawFarmBoundary();
    drawEntrance();

    drawCowShed();
    drawBarn();

    drawFeedingArea();

    drawHayArea();

    drawWaterFacility();

    drawTrees();

    drawFans();

    drawFarmProps();
}
```

Later modules will add:

```cpp
drawAnimals();
drawHumans();
```

without restructuring everything.

---

# 44. STATIC VS DYNAMIC OBJECTS

Module 2 should establish a clear distinction.

## Static objects

Examples:

```text
ground
road
fence
pillars
shed
barn
trees
hay
troughs
tank
crates
```

## Dynamic-ready objects

Examples:

```text
gate
fans
future cows
future calves
future humans
```

Gate remains animated from Module 1.

Fans remain static in Module 2 but must be animation-ready.

---

# 45. TEXTURED DRAWING HELPER

Create a reusable function such as:

```cpp
void drawTexturedCube(
    Shader& shader,
    unsigned int texture,
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale,
    glm::vec2 textureScale
);
```

or a parent-matrix-capable equivalent.

Better architecture:

```cpp
void drawTexturedCube(
    Shader& shader,
    unsigned int texture,
    const glm::mat4& parent,
    glm::vec3 localPosition,
    glm::vec3 localRotation,
    glm::vec3 localScale,
    glm::vec2 uvScale
);
```

This will become very useful later.

---

# 46. MODEL MATRIX ORDER

Maintain correct transformation ordering.

Conceptually:

```text
Parent
  × Translation
  × Rotation
  × Scale
```

Do not scale parent coordinate translations accidentally.

For hierarchical objects such as:

- gate
- fans

transform local components relative to their parent.

---

# 47. TEXTURE TRANSFORM / UV SCALE

Where useful, allow repeat scaling independently from object dimensions.

For example:

```text
Large barn wall:
UV repeat = 4 × 2

Grass:
UV repeat = 12 × 12

Road:
UV repeat = 2 × 10
```

Do not rely solely on model scaling to determine texture repetition.

---

# 48. AVOID Z-FIGHTING

Be careful with overlapping surfaces.

For example:

```text
grass ground Y = 0.000
road Y = 0.015
shed floor Y = 0.025
```

or use suitable geometry thickness.

Do not place coplanar surfaces directly on one another.

---

# 49. CAMERA COLLISION

A full collision system is NOT required in Module 2.

The camera may pass through objects if necessary.

Do not waste substantial time implementing physics.

However, object spacing should make normal navigation easy.

---

# 50. KEEP THE CAMERA CONTROLS

Preserve:

```text
W = forward
S = backward
A = left
D = right
Q = down
E = up
Mouse = look
Scroll = zoom
G = gate
ESC = exit
```

Do not change existing controls without a strong reason.

---

# 51. ADD OPTIONAL TEXTURE CONTROL

If stable, optionally add:

```text
T = texture ON/OFF
```

When textures are off, objects may display simple colors.

This can be useful for demonstrating the effect of texture mapping.

However:

This is optional.

Do not sacrifice stability for this feature.

---

# 52. CONSOLE INSTRUCTIONS

Update the startup controls.

Example:

```text
=============================================
3D AGRO FARM - MODULE 2
=============================================
W/A/S/D : Move Camera
Q/E     : Down / Up
Mouse   : Look Around
Scroll  : Zoom
G       : Open / Close Gate
T       : Texture Toggle [if implemented]
ESC     : Exit
=============================================
MODULE 2:
Detailed Environment + Texture Mapping
=============================================
```

---

# 53. TEXTURE LOADING ERRORS

If texture loading fails:

- print the actual failed path
- avoid silent failure
- use a fallback color if practical

Example:

```cpp
std::cerr
    << "Failed to load texture: "
    << path
    << std::endl;
```

This will make debugging much easier.

---

# 54. PERFORMANCE

Do not create new:

- VAOs
- VBOs
- EBOs
- textures

inside the render loop.

Create them during initialization.

The render loop should primarily:

- bind shader
- bind texture
- update uniforms
- bind VAO
- issue draw calls

---

# 55. REUSE GEOMETRY

One cube mesh should generate many objects through transformations.

Examples:

```text
one cube
 ↓
fence post
gate bar
barn wall
shed post
road
hay bale
crate
fan blade
trough wall
```

This is a major graphics concept.

Do not create separate geometry buffers for every cube-shaped object.

---

# 56. FUTURE LIGHTING COMPATIBILITY

Although advanced lighting belongs to Module 4, prepare geometry for it.

Textured vertex data should ideally include normals.

Future Module 4 will need:

```glsl
FragPos
Normal
TexCoord
```

Therefore, structure the vertex shader to make eventual expansion easy.

Do not create a texture architecture that must be completely discarded in Module 4.

---

# 57. FUTURE ANIMAL COMPATIBILITY

Module 3 will add:

- adult cows
- calves
- workers
- animations

Therefore:

- preserve open grazing area
- preserve walking paths
- leave space beside troughs
- leave space under shed
- avoid filling every empty area with decorative objects

The environment should support movement.

---

# 58. FUTURE CURVED-SURFACE COMPATIBILITY

Module 5 will add:

- Bézier milk can
- spline-based object
- ruled surface roof
- other advanced curved geometry

Therefore:

Do not use advanced Bézier/B-spline/ruled-surface code in Module 2.

Reserve logical locations for these later objects.

For example:

```text
Milk collection area near shed:
keep a small open platform

Cow-shed roof:
current simple roof can later be replaced

Water facility:
leave nearby space for advanced curved container
```

---

# 59. NO ANIMAL MODELS YET

Do NOT implement:

```text
cow
calf
cow head
cow legs
cow tail
grazing animation
walking animation
running calf
```

These belong to Module 3.

Even if a cow texture is available, do not use it yet.

---

# 60. NO HUMAN MODELS YET

Do NOT add:

```text
workers
human walking
human arms
human legs
```

These belong to Module 3.

---

# 61. NO FAN ANIMATION YET

Create complete fan geometry.

But:

```cpp
fanAngle = 0.0f;
```

for Module 2.

Animation will be implemented in Module 3.

---

# 62. NO ADVANCED LIGHTING YET

Do NOT implement the final:

```text
directional sunlight
multiple point lights
spotlight
attenuation
emissive bulbs
ambient toggle
diffuse toggle
specular toggle
day/night
```

These belong to Module 4.

The Module 2 scene should remain clearly visible using a simple texture/color shader.

---

# 63. NO CURVE REQUIREMENTS YET

Do NOT implement:

```text
Bezier surface
B-spline
ruled surface
surface of revolution
advanced curved roof
```

These belong to Module 5.

---

# 64. NO FOUR VIEWPORTS YET

Keep only the primary perspective camera in Module 2.

Bird's-eye and four-view systems belong to Module 5.

---

# 65. VISUAL QUALITY TARGET

After Module 2, I should be able to enter the farm and visually identify, without explanation:

- grass field
- road
- entrance
- fence
- cow shed
- feed barn
- trough
- hay
- trees
- water structure
- fans
- storage props

The scene should not look like a random collection of cubes.

Use:

- proportion
- placement
- texture
- repetition
- architectural grouping

to communicate each object.

---

# 66. SCALE CONSISTENCY

Use believable relative sizes.

For example:

```text
Fence:
approximately 1–1.5 world units high

Gate:
approximately 2–3 units high

Entrance pillars:
approximately 4–5 units high

Cow shed:
approximately 4–5 units high

Barn:
approximately 5–7 units high

Tree:
approximately 4–8 units high

Trough:
approximately 0.5–1 unit high
```

Exact values may vary.

Avoid huge hay bales or tiny buildings.

---

# 67. ENVIRONMENT DETAIL WITHOUT CLUTTER

Use the principle:

```text
foreground = entrance
middle ground = road / grazing / feeding area
background = shed / barn / water facility
edges = trees / fences
```

This creates visual depth.

Do not fill every location equally.

---

# 68. RECOMMENDED OBJECT COUNT

A sensible Module 2 target is:

```text
1 entrance complex
1 complete boundary
1 main road
1 cow shed
1 barn
2–3 feeding troughs
5–8 hay bales
5–10 trees
1 water tower/silo
2 static fans
3–6 crates/feed sacks
several lamp fixtures
```

Do not create hundreds of unique objects.

---

# 69. MODULAR FUNCTIONS

By the end of Module 2, code should contain functions conceptually similar to:

```cpp
void loadFarmTextures();

void drawGround(...);
void drawRoad(...);

void drawFarmBoundary(...);
void drawFenceSection(...);

void drawEntrance(...);
void drawGate(...);

void drawCowShed(...);
void drawBarn(...);

void drawTrough(...);
void drawFeedingArea(...);

void drawHayBale(...);
void drawHayArea(...);

void drawTree(...);
void drawTrees(...);

void drawWaterFacility(...);

void drawFan(...);
void drawShedFans(...);

void drawCrate(...);
void drawFeedSack(...);
void drawFarmProps(...);

void renderScene(...);
```

Exact names may differ.

The important requirement is clear modular structure.

---

# 70. POSSIBLE HEADER ORGANIZATION

If needed, gradually organize files as:

```text
AgroFarm/
│
├── main.cpp
├── shader.h
├── camera.h
│
├── primitives/
│   └── cube.h
│
├── scene/
│   ├── farmEnvironment.h
│   ├── farmBuildings.h
│   └── farmProps.h
│
├── shaders/
│   ├── basicColor.vs
│   ├── basicColor.fs
│   ├── texture.vs
│   └── texture.fs
│
└── textures/
    ├── grass.jpg
    ├── soil.jpg
    ├── wood.jpg
    ├── brick.jpg
    ├── roof.jpg
    ├── concrete.jpg
    ├── metal.jpg
    ├── hay.jpg
    ├── bark.jpg
    └── leaves.jpg
```

Do not split files excessively if that creates unnecessary complexity.

Correctness is more important than artificial abstraction.

---

# 71. TEXTURE SHADER DESIGN FOR FUTURE MODULE 4

A recommended Module 2 vertex shader output is already:

```glsl
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
```

even if `FragPos` and `Normal` are not heavily used yet.

The vertex shader can calculate:

```glsl
FragPos =
    vec3(model * vec4(aPos, 1.0));

Normal =
    mat3(transpose(inverse(model))) * aNormal;

TexCoord = aTexCoord;
```

This makes Module 4 easier.

But do not implement full Phong illumination yet.

---

# 72. IMPORTANT SHADER SAFETY

If any supplied shader uses an accumulator such as:

```glsl
vec3 result;
```

before performing:

```glsl
result += ...
```

initialize it correctly:

```glsl
vec3 result = vec3(0.0);
```

Do not retain undefined shader behavior.

---

# 73. NORMAL MATRIX PREPARATION

For non-uniformly scaled objects, future lighting will need:

```glsl
mat3(transpose(inverse(model)))
```

for normals.

Prepare for this in the vertex shader architecture.

Module 2 may not yet visibly use it.

---

# 74. OPTIONAL SIMPLE AMBIENT DISPLAY

If completely unlit texture rendering makes the scene difficult to interpret, a very simple constant brightness/tint is acceptable.

For example:

```glsl
vec3 texColor =
    texture(texture1, TexCoord).rgb;

vec3 finalColor =
    texColor * 0.95;
```

Do NOT confuse this with implementing full Phong lighting.

---

# 75. MODULE 2 RENDER ORDER

A reasonable conceptual order is:

```cpp
drawGround();
drawRoad();

drawBoundary();
drawEntrance();

drawCowShed();
drawBarn();

drawFeedingArea();

drawHayArea();

drawWaterFacility();

drawTrees();

drawStaticFans();

drawFarmProps();
```

Opaque object ordering is not usually critical with depth testing, but logical organization improves debugging.

---

# 76. TEST OBJECTS INDIVIDUALLY

During implementation, verify each major object separately.

Example development sequence:

### Test A
Grass texture fills ground properly.

### Test B
Road texture repeats correctly.

### Test C
Fence texture works.

### Test D
Gate still opens.

### Test E
Shed appears correctly.

### Test F
Barn appears correctly.

### Test G
Trees/hay/tank appear correctly.

Do not wait until the entire farm is added to begin debugging.

---

# 77. CAMERA TESTING

Inspect each object from several directions.

Ensure:

- roof faces are not missing
- texture orientation is not obviously inverted
- fence does not disappear unexpectedly
- gate hinges remain correct
- objects are not below ground
- large structures do not clip immediately

---

# 78. FACE CULLING

Do not enable face culling unless all cube/mesh vertex winding is correct.

If course code does not consistently use it, depth testing alone is acceptable.

Avoid introducing disappearing geometry just for optimization.

---

# 79. BLENDING

Transparent vegetation is not required in Module 2.

Avoid introducing alpha-blended grass sprites unless necessary.

Keep vegetation mostly opaque/simple to maintain stability.

---

# 80. SKY

Keep the existing sky-colored clear color.

Do not implement a complex skybox unless it is already easy and explicitly supported.

A skybox is not required for Module 2.

Use a clear color such as light blue.

---

# 81. BACKGROUND COMPOSITION

The farm should look good from the initial entrance camera.

The user should be able to see glimpses of:

- shed roof
- barn roof
- trees

behind the gate.

This gives visual motivation to open the gate and explore.

Do not hide all farm elements behind a tall wall.

---

# 82. ROAD ALIGNMENT

Ensure the road visually aligns with the gate opening.

Do not place the gate at:

```text
X = 0
```

while road center is noticeably offset unless intentionally designed.

The road should guide the viewer through the entrance.

---

# 83. SHED INTERIOR SPACE

Keep enough room under the shed for future:

```text
cows
worker
fans
lights
feeding area
```

Do not fill the shed interior with permanent boxes.

---

# 84. BARN INTERIOR/DOOR

The barn does not need a fully modeled interior.

However, provide a visible entrance/door.

A dark recess or open section is sufficient.

Do not build unnecessary rooms.

---

# 85. WATER STRUCTURE COLLISION WITH FUTURE CURVES

Remember that Module 5 will add a Bézier-generated farm object.

Therefore, the Module 2 water structure should not consume every visually suitable area.

Leave a small nearby platform available.

---

# 86. MODULE 2 CONTROL SUMMARY

Required:

```text
W/A/S/D = movement
Q/E = vertical movement
Mouse = look
Scroll = zoom
G = gate
ESC = exit
```

Optional:

```text
T = textures on/off
N = nearest filtering
L = linear filtering
```

Do not use keys reserved for later features if avoidable.

Suggested later reservation:

```text
F = fan
C = cow animation
R = calf running
1/2/3 = lights
4 = day/night
5/6/7 = lighting components
B = bird's eye
V = viewports
```

---

# 87. MODULE 2 DEMONSTRATION SEQUENCE

When presenting Module 2, this sequence should work:

### Step 1 — Start
Camera begins outside entrance.

Visible:

- textured fence
- textured pillars
- gate
- sign
- grass
- road
- distant farm buildings

### Step 2 — Gate
Press `G`.

Gate smoothly opens using Module 1 animation.

### Step 3 — Enter
Press `W`.

Move through the gate.

### Step 4 — Ground
Show repeating grass texture and dirt road.

### Step 5 — Cow shed
Walk toward cow shed.

Show:

- detailed structure
- roof
- posts
- floor
- troughs
- static fan structures

### Step 6 — Barn
Walk toward feed barn.

Show:

- walls
- door
- roof
- crates/feed sacks

### Step 7 — Farm props
Show:

- hay
- water structure
- trees
- fence
- troughs

### Step 8 — Texture concepts
If implemented, demonstrate wrapping/filtering or texture toggle.

At no point should cows/humans appear yet.

---

# 88. MODULE 2 ACCEPTANCE CHECKLIST

Module 2 is complete only when:

- Module 1 still compiles.
- Module 1 camera still works.
- Module 1 gate still works.
- Gate texture does not break animation.
- Texture loading works.
- Texture paths are relative.
- Grass texture repeats.
- Road texture repeats correctly.
- Fence is textured.
- Entrance pillars are textured.
- Gate is textured.
- Sign is improved.
- Detailed cow shed replaces placeholder.
- Detailed barn replaces placeholder.
- Feeding troughs exist.
- Hay area exists.
- Trees exist.
- Water/silo structure exists.
- Two static fans exist.
- Fan components use parent transforms.
- Farm props exist.
- Grazing area remains mostly open.
- No z-fighting is obvious.
- No VAO/VBO/texture is recreated every frame.
- Geometry is modular.
- Textured geometry includes normals if practical.
- Scene remains navigable.
- No animals have been added.
- No humans have been added.
- No advanced lighting has been added.
- No Bézier/spline/ruled surfaces have been added.
- No four-view mode has been added.

---

# 89. WHAT MODULE 2 SHOULD LOOK LIKE WHEN FINISHED

The final Module 2 scene should approximately communicate:

```text
                           FARM BACK AREA

         TREE       TEXTURED COW SHED
                        _________
                       /________/|
                      |         ||
                      | TROUGHS ||
                      |_________||

                                      TEXTURED BARN
                                         /\
                                        /  \
                                       /____\
                                      |      |
                            CRATES    | FEED |
                                      |______|

        HAY BALES

                                    WATER TANK

              ===============================
                    OPEN GRAZING FIELD
              ===============================

      TREE                               TREE

                     DIRT ROAD
                        ||
                        ||
                        ||
 ---------------------------------------------------
 WOOD FENCE     BRICK     GATE     BRICK   WOOD FENCE
                PILLAR             PILLAR
 ---------------------------------------------------

                       CAMERA
```

The user should immediately feel:

> “This is now an actual farm environment.”

Module 3 will then bring it to life.

---

# 90. WHAT I WANT FROM YOU

Now implement **MODULE 2 only**, continuing directly from the existing completed Module 1.

Before editing code:

1. inspect Module 1 carefully;
2. inspect the Lab 4 texture examples from my supplied resources;
3. identify the existing reusable cube implementation;
4. preserve camera/gate architecture;
5. decide which geometry must be extended with normals and texture coordinates.

Then implement Module 2 in the planned phases.

Do not give only snippets or pseudocode.

Provide the complete compile-ready implementation required to upgrade Module 1 into Module 2.

For every created or modified file, clearly identify:

```text
filename
```

and provide its complete content.

Do not write:

```text
same as previous
existing code here
rest unchanged
```

for files that must actually be modified.

At the end, provide:

## A. Final project file structure

Show all relevant:

- `.cpp`
- `.h`
- shader
- texture

files.

## B. Texture asset list

State exactly which image files are required and what each is used for.

## C. Controls

List all current Module 2 controls.

## D. Scene object list

List every object that should be visible.

## E. Test sequence

Give a short ordered test procedure.

## F. Expected result

Describe exactly what I should see after launching Module 2.

## G. Module 3 readiness check

Confirm that the project architecture now has appropriate empty/open locations and reusable transformation helpers for:

- cows
- calves
- humans
- fan rotation

but do NOT implement those Module 3 features yet.

The final priority is:

**preserve Module 1 + create a visually detailed farm + correctly demonstrate texture mapping + prepare the architecture for animated animals and humans in Module 3.**