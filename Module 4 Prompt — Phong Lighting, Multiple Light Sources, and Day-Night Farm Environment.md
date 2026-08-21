I am continuing my **CSE 4208 Computer Graphics project in C++ using Modern OpenGL**.

The final project is:

# Interactive 3D Agro Farm Simulation

Modules 1, 2, and 3 are already completed.

Now implement:

# MODULE 4 — REALISTIC LIGHTING, MULTIPLE LIGHT SOURCES, PHONG SHADING, AND DAY/NIGHT SYSTEM

Do NOT start a new OpenGL project.

Extend the existing Module 3 project directly.

Do NOT remove or break any existing environment, texture, camera, gate, animal, human, or fan functionality.

---

# 1. CURRENT PROJECT STATE

## Module 1 already provides

- GLFW
- GLAD
- GLM
- Modern OpenGL
- model/view/projection matrices
- perspective projection
- depth testing
- free camera
- mouse look
- zoom
- farm entrance
- road
- boundary fence
- double gate
- smooth gate animation
- reusable primitive rendering
- delta time

## Module 2 already provides

- texture mapping
- `stb_image`
- textured grass
- textured dirt road
- textured fence
- textured entrance
- detailed cow shed
- detailed feed barn
- feeding troughs
- hay
- trees
- water/silo structure
- crates/feed sacks
- fan geometry
- lamp/bulb fixture geometry
- textured farm props

## Module 3 already provides

- adult cows
- calves
- farm workers
- hierarchical animal modeling
- articulated cow head
- articulated cow legs
- tail movement
- walking cow
- grazing cow
- feeding cow
- idle cow
- running calves
- human walking
- human limb animation
- rotating fans
- independent animation states
- delta-time animation

Now upgrade the entire scene with proper lighting.

---

# 2. PRIMARY MODULE 4 GOAL

After Module 4, the farm must visibly react to realistic light.

The scene should include:

1. **Directional light** representing sunlight.
2. **Point lights** representing bulbs inside the cow shed and barn.
3. **Spotlight** around the farm entrance.
4. **Emissive bulb/lamp objects**.
5. Ambient lighting.
6. Diffuse lighting.
7. Specular lighting.
8. Distance attenuation for point lights.
9. Spotlight cutoff.
10. Material shininess.
11. Day mode.
12. Night mode.
13. Individual light-type toggles.
14. Ambient/diffuse/specular component toggles.
15. Correct lighting on textured static and animated objects.

The goal is not photorealism.

The goal is to clearly demonstrate the lighting concepts taught in the course.

---

# 3. COURSE RESOURCE REQUIREMENT

Carefully inspect and reuse the lighting concepts and code patterns from my supplied course resources, especially:

- Lab 3 lighting examples
- Phong lighting
- ambient component
- diffuse component
- specular component
- directional light
- point light
- spotlight
- attenuation
- light/material properties
- Gouraud/Phong examples where applicable
- Lab 4 textured shader
- Lab 5 attenuation examples if useful

Do not introduce a completely unrelated physically based rendering system.

Do NOT implement:

- PBR
- HDR
- deferred shading
- shadow mapping
- SSAO
- bloom
- ray tracing

unless specifically required later.

Keep the solution consistent with the CSE 4208 syllabus.

---

# 4. IMPORTANT MODULE 4 DESIGN PRINCIPLE

Module 4 should primarily change the **rendering/shader pipeline**, not rebuild the farm.

All existing objects should continue to exist:

```text id="bq03ym"
Farm
├── entrance
├── gate
├── fence
├── grass
├── road
├── shed
├── barn
├── hay
├── trees
├── water structure
├── props
├── cows
├── calves
├── workers
└── fans
```

The main goal is:

```text id="1gofok"
existing scene
      +
proper normals
      +
Phong material
      +
multiple lights
      =
Module 4
```

---

# 5. MODULE 4 IMPLEMENTATION PLAN

Implement in this order.

## Phase 4.1
Audit normals and existing vertex formats.

## Phase 4.2
Create unified Phong textured shader.

## Phase 4.3
Implement material structure.

## Phase 4.4
Implement directional sunlight.

## Phase 4.5
Implement point lights.

## Phase 4.6
Add attenuation.

## Phase 4.7
Implement entrance spotlight.

## Phase 4.8
Implement emissive bulb objects.

## Phase 4.9
Add ambient/diffuse/specular toggles.

## Phase 4.10
Add light-type toggles.

## Phase 4.11
Implement day/night system.

## Phase 4.12
Tune materials for farm surfaces.

## Phase 4.13
Test lighting on animated animals.

## Phase 4.14
Test scene performance and finalize.

Do not implement everything simultaneously.

---

# 6. FIRST STEP — AUDIT NORMALS

Before implementing lighting, inspect all currently used geometry.

Lighting requires correct vertex normals.

Check:

- cube
- ground plane
- sphere
- cone
- tree geometry
- cow components
- human components
- fan
- shed/barn geometry
- props

Every lit mesh should ideally provide:

```text id="ao4a7k"
position
normal
texture coordinate
```

A common layout is:

```text id="kc22m3"
8 floats per vertex

x y z
nx ny nz
u v
```

Do not calculate all normals in the fragment shader.

Provide proper vertex normals.

---

# 7. NORMAL MATRIX

Because many farm objects are non-uniformly scaled, do NOT transform normals using only:

```glsl id="b7vd76"
mat3(model) * aNormal
```

Use:

```glsl id="wmw773"
mat3(transpose(inverse(model))) * aNormal
```

For example:

```glsl id="gjrvv5"
Normal =
    normalize(
        mat3(transpose(inverse(model)))
        * aNormal
    );
```

This is especially important for:

- flattened cubes
- long fence rails
- cow body components
- fan blades
- stretched barn walls

---

# 8. PHONG LIGHTING PIPELINE

Use **Phong shading** as the primary Module 4 rendering method.

Lighting calculation should occur in the fragment shader.

The vertex shader should pass:

```glsl id="75i2k6"
FragPos
Normal
TexCoord
```

to the fragment shader.

Conceptually:

```glsl id="q8vsgk"
FragPos =
    vec3(model * vec4(aPos, 1.0));

Normal =
    mat3(transpose(inverse(model))) * aNormal;

TexCoord =
    aTexCoord;
```

---

# 9. PHONG COMPONENTS

The lighting model must include:

\[
I = I_{ambient} + I_{diffuse} + I_{specular}
\]

For textured surfaces:

```text id="v0zo0y"
texture color
×
lighting contribution
```

or another valid Phong-texture formulation.

Do not simply add a white light value to the texture without proper component calculation.

---

# 10. AMBIENT COMPONENT

Use:

\[
I_a = L_a \times M_a
\]

or texture-modulated equivalent.

Conceptually:

```glsl id="71k0ws"
vec3 ambient =
    light.ambient *
    materialColor;
```

For textured objects:

```glsl id="aqsr8n"
vec3 ambient =
    light.ambient *
    textureColor;
```

The ambient contribution should prevent completely black unlit surfaces during day mode.

---

# 11. DIFFUSE COMPONENT

Use Lambertian diffuse lighting.

Calculate:

\[
D =
\max(
N \cdot L,
0
)
\]

Conceptually:

```glsl id="av7c3i"
vec3 norm =
    normalize(Normal);

vec3 lightDir =
    normalize(lightPosition - FragPos);

float diff =
    max(
        dot(norm, lightDir),
        0.0
    );
```

For directional light, use the negative light direction appropriately.

---

# 12. SPECULAR COMPONENT

Use view direction:

```glsl id="m51kme"
vec3 viewDir =
    normalize(viewPos - FragPos);
```

Use reflected light:

```glsl id="m66wno"
vec3 reflectDir =
    reflect(-lightDir, norm);
```

Then:

```glsl id="20k5pb"
float spec =
    pow(
        max(
            dot(viewDir, reflectDir),
            0.0
        ),
        shininess
    );
```

---

# 13. MATERIAL SHININESS

Do not give every farm object identical shininess.

Different surfaces must behave differently.

Suggested starting values:

| Material | Shininess |
|---|---:|
| grass | 2–4 |
| soil | 2 |
| brick | 4–8 |
| wood | 8 |
| hay | 2–4 |
| concrete | 4 |
| cow skin | 8–16 |
| human clothes | 4–8 |
| painted gate | 24–32 |
| fan metal | 32 |
| water tank metal | 48–64 |
| shiny equipment | 64 |

Tune values visually.

---

# 14. MATERIAL STRUCTURE

Use a reusable material representation.

For example:

```cpp id="exkdpr"
struct Material
{
    float shininess;

    glm::vec3 specularColor;

    float emissionStrength;
};
```

or a shader structure such as:

```glsl id="x5u6g7"
struct Material
{
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
```

Use architecture compatible with the existing texture system.

---

# 15. TEXTURE + PHONG INTEGRATION

Do not discard Module 2 textures.

The final fragment color should still use the object texture.

Conceptually:

```glsl id="dya5a0"
vec3 texColor =
    texture(
        material.diffuse,
        TexCoord
    ).rgb;
```

Then compute:

```glsl id="4usofu"
ambient
diffuse
specular
```

and produce the final color.

---

# 16. IMPORTANT SHADER INITIALIZATION

Never write:

```glsl id="vzy5ji"
vec3 result;
```

and then:

```glsl id="iftoqa"
result += ...
```

Initialize it:

```glsl id="qw8mdu"
vec3 result =
    vec3(0.0);
```

This avoids undefined shader behavior.

---

# 17. LIGHT TYPES REQUIRED

Use three major light types:

```text id="40nwvo"
Directional Light
Point Light
Spotlight
```

Each should have a clearly different purpose in the farm.

---

# 18. DIRECTIONAL LIGHT = SUN

Use one directional light representing sunlight.

A directional light has no physical position for shading purposes.

All rays have the same direction.

Recommended initial direction:

```cpp id="zrshf3"
glm::vec3(
    -0.3f,
    -1.0f,
    -0.25f
);
```

Tune according to the scene.

---

# 19. DIRECTIONAL LIGHT STRUCTURE

Use something like:

```glsl id="h1p0lb"
struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

Possible day values:

```text id="8m70oz"
ambient ≈ 0.20–0.35
diffuse ≈ 0.7–1.0
specular ≈ 0.4–0.8
```

These are starting ranges, not mandatory exact values.

---

# 20. DIRECTIONAL LIGHT FUNCTION

Implement lighting cleanly:

```glsl id="gptosy"
vec3 CalcDirLight(
    DirLight light,
    vec3 normal,
    vec3 viewDir,
    vec3 textureColor,
    Material material
);
```

Do not put every lighting equation directly inside `main()` if separate functions improve clarity.

---

# 21. DIRECTIONAL LIGHT TOGGLE

Use:

```text id="8ev98o"
1 = Directional sunlight ON/OFF
```

Use single-key-press toggle behavior.

When disabled, the sun contribution should become zero.

Do not destroy the light object.

Use a boolean/uniform.

---

# 22. POINT LIGHTS

Use point lights for farm bulbs.

Recommended locations:

### Point Light 1
Inside cow shed.

### Point Light 2
Inside cow shed or near feeding area.

### Point Light 3
Barn/feed-store entrance/interior.

Use approximately:

```text id="57ah91"
2–3 point lights
```

Three is a good target.

---

# 23. POINT LIGHT STRUCTURE

Use:

```glsl id="ueq5fw"
struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

This structure should match the course examples.

---

# 24. POINT LIGHT ATTENUATION

Implement:

\[
Attenuation =
\frac{1}
{k_c + k_l d + k_q d^2}
\]

where:

```text id="fmrwam"
d =
distance between
light position and fragment position
```

Conceptually:

```glsl id="shzc82"
float distance =
    length(
        light.position - FragPos
    );

float attenuation =
    1.0 /
    (
        light.constant +
        light.linear * distance +
        light.quadratic *
        distance *
        distance
    );
```

Then multiply:

```glsl id="3tuclf"
ambient
diffuse
specular
```

by attenuation where appropriate.

---

# 25. STARTING ATTENUATION VALUES

A reasonable starting set is:

```text id="6qimqr"
constant  = 1.0
linear    = 0.09
quadratic = 0.032
```

These are standard examples, but the farm scale may require tuning.

Do not blindly keep them if the lights illuminate too much or too little.

---

# 26. POINT LIGHT COLORS

Farm shed bulbs should usually be warm white.

For example:

```text id="4mqg4r"
diffuse:
approximately warm white

specular:
white

ambient:
small warm contribution
```

Do not make every point light bright red/green/blue unless intentionally demonstrating colored lighting.

The farm should remain visually believable.

---

# 27. POINT LIGHT FIXTURES

Module 2 already created lamp/bulb fixture geometry.

Align point-light world positions with those visible fixtures.

Do not place invisible point lights in arbitrary locations.

When the user sees a glowing bulb, its light should appear to originate from that location.

---

# 28. POINT LIGHT TOGGLE

Use:

```text id="ow6frn"
2 = Point Lights ON/OFF
```

One toggle may control all farm point lights.

Optional advanced behavior:

```text id="iyaiw5"
additional keys for individual bulbs
```

is not necessary.

Keep the control simple.

---

# 29. SPOTLIGHT

Use a spotlight around the farm entrance.

Good location:

```text id="vz0o9g"
above gate
```

Direction:

```text id="zxwkq3"
downward
+
slightly toward road
```

At night this spotlight should visibly illuminate the entrance and road.

---

# 30. SPOTLIGHT STRUCTURE

Use something like:

```glsl id="f2tr3b"
struct SpotLight
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

Use inner and outer cutoffs for a soft edge.

---

# 31. SPOTLIGHT CUTOFF

Use:

```cpp id="90egsz"
glm::cos(
    glm::radians(15.0f)
)
```

for inner cutoff and something like:

```cpp id="7hsn58"
glm::cos(
    glm::radians(22.0f)
)
```

for outer cutoff.

Tune according to entrance scale.

---

# 32. SPOTLIGHT SOFT EDGE

Calculate:

```glsl id="4ya5rr"
float theta =
    dot(
        lightDir,
        normalize(-light.direction)
    );

float epsilon =
    light.cutOff -
    light.outerCutOff;

float intensity =
    clamp(
        (theta - light.outerCutOff)
        / epsilon,
        0.0,
        1.0
    );
```

Then multiply diffuse/specular light by `intensity`.

This gives a better spotlight than a hard cutoff.

---

# 33. SPOTLIGHT ATTENUATION

The entrance spotlight should also use distance attenuation.

Reuse:

```text id="gn7ncm"
constant
linear
quadratic
```

similar to a point light.

---

# 34. SPOTLIGHT TOGGLE

Use:

```text id="465x2u"
3 = Entrance Spotlight ON/OFF
```

When toggled off:

- the visible lamp geometry may remain
- emitted/glowing appearance should decrease or turn off
- lighting contribution becomes zero

---

# 35. EMISSIVE OBJECTS

Visible bulbs should appear bright.

A bulb should not look like a dark object that somehow illuminates the room.

Implement an emissive component for lamp geometry.

Possible approaches:

### Approach A

Separate emissive shader.

### Approach B

Same shader with:

```glsl id="5bs5tt"
uniform bool useEmission;
uniform vec3 emissionColor;
uniform float emissionStrength;
```

Both are acceptable.

Use whichever integrates cleanly.

---

# 36. EMISSION FORMULA

Conceptually:

```glsl id="yohw8h"
vec3 emission =
    emissionColor *
    emissionStrength;
```

Then:

```glsl id="z2zxe8"
finalColor =
    lightingResult +
    emission;
```

Do not add emission to all objects.

Use it only for:

- bulbs
- entrance lamp
- perhaps small indicator lights if present

---

# 37. BULB ON/OFF STATE

Emissive bulb state should correspond to the relevant light state.

For example:

```text id="kxz7t1"
Point lights OFF
→ shed bulbs no longer strongly glow.

Point lights ON
→ bulb surfaces appear bright.
```

Likewise for the entrance spotlight fixture.

---

# 38. LIGHT COMPONENT TOGGLES

The course resources emphasize lighting components.

Provide:

```text id="5e4kx9"
5 = Ambient ON/OFF
6 = Diffuse ON/OFF
7 = Specular ON/OFF
```

These toggles should affect the calculations globally.

---

# 39. AMBIENT TOGGLE IMPLEMENTATION

Use uniform:

```glsl id="c8elfb"
uniform bool ambientEnabled;
```

Conceptually:

```glsl id="adfoyn"
if (!ambientEnabled)
{
    ambient =
        vec3(0.0);
}
```

or multiply by:

```glsl id="700enl"
float(ambientEnabled)
```

depending on GLSL style/version.

---

# 40. DIFFUSE TOGGLE

Use:

```glsl id="nijrwp"
uniform bool diffuseEnabled;
```

When disabled:

- direct surface illumination disappears
- ambient/specular may remain

This should be visually demonstrable.

---

# 41. SPECULAR TOGGLE

Use:

```glsl id="d6k2w0"
uniform bool specularEnabled;
```

When disabled:

- shiny highlights disappear
- other lighting remains

Use a metal object such as:

```text id="sa277w"
gate
fan
water tank
```

to demonstrate the difference.

---

# 42. GLOBAL LIGHTING CONTROL STATE

Maintain clean CPU-side variables:

```cpp id="w4gq75"
bool directionalLightOn = true;
bool pointLightsOn = true;
bool spotLightOn = true;

bool ambientOn = true;
bool diffuseOn = true;
bool specularOn = true;
```

Do not scatter unrelated integers across the code.

---

# 43. DAY/NIGHT MODE

Implement:

```text id="qo1rez"
4 = Day / Night toggle
```

This must visibly change the entire environment.

Do not simply change the sky color while keeping identical light intensities.

---

# 44. DAY MODE

During day:

### Sky
Light blue.

### Directional sun
Strong.

### Point lights
May be low/off by default.

### Entrance spotlight
May be low/off by default.

### Ambient
Moderate.

The farm should be naturally visible primarily because of sunlight.

---

# 45. NIGHT MODE

During night:

### Sky
Very dark blue / nearly black.

### Directional light
Very weak moon-like illumination or effectively off.

### Ambient
Low but non-zero.

### Point lights
Strongly visible.

### Entrance spotlight
Strongly visible.

### Emissive bulbs
Bright.

This should make the lighting system visually impressive.

---

# 46. NIGHT MODE SHOULD NOT BE PURE BLACK

Do not make the entire farm invisible outside bulb radii.

Use a small environmental ambient term such as:

```text id="nq94fu"
0.03–0.10
```

depending on visual result.

The user should still understand scene geometry.

---

# 47. DAY/NIGHT STATE

Use:

```cpp id="jvjzrj"
bool nightMode = false;
```

or:

```cpp id="sqhkzh"
enum class TimeOfDay
{
    Day,
    Night
};
```

Both are acceptable.

---

# 48. DAY/NIGHT FUNCTION

Create something like:

```cpp id="6zopdm"
void updateLightingForTimeOfDay();
```

or set uniforms inside:

```cpp id="qbe7yr"
setupLights(...)
```

Avoid dozens of unrelated `if (nightMode)` blocks scattered through object-drawing functions.

---

# 49. CLEAR COLOR

Change sky/background using:

```cpp id="8dwzmx"
glClearColor(...)
```

### Day example

```text id="x27i4s"
light blue
```

### Night example

```text id="6gq5z4"
dark navy blue
```

No skybox is required.

---

# 50. OPTIONAL GRADUAL DAY/NIGHT TRANSITION

A smooth transition is optional.

For example:

```text id="hgqxl2"
Day
→ sunset-like interpolation
→ Night
```

However, do not implement this unless all required lighting works first.

A direct toggle is perfectly acceptable.

---

# 51. LIGHT POSITION VISUALIZATION

Optional debugging feature:

Draw small bright cubes/spheres where point lights are located.

This can help development.

For final presentation, use the actual bulb geometry instead.

Do not leave large debug cubes floating inside the farm.

---

# 52. LIGHTING FUNCTION ARCHITECTURE

A good fragment shader architecture is:

```glsl id="7jcku8"
vec3 result =
    vec3(0.0);

result +=
    CalcDirLight(...);

for (...)
{
    result +=
        CalcPointLight(...);
}

result +=
    CalcSpotLight(...);

result +=
    emission;
```

Each function should return zero when its light type is disabled or alternatively be skipped based on uniforms.

---

# 53. NUMBER OF POINT LIGHTS

Use a compile-time constant if appropriate:

```glsl id="6xu79n"
#define NR_POINT_LIGHTS 3
```

or:

```glsl id="jbz7jw"
const int NR_POINT_LIGHTS = 3;
```

Keep the count small.

There is no need for dozens of farm lights.

---

# 54. POINT LIGHT LOCATIONS

Determine positions from the actual Module 3 scene.

Do not invent coordinates before inspecting the current shed and barn layout.

Suggested conceptual positions:

```text id="wc1z17"
Light 1:
left side of cow shed ceiling

Light 2:
right side of cow shed ceiling

Light 3:
barn entrance/interior
```

The actual values must align with the existing building geometry.

---

# 55. ANIMATED OBJECT LIGHTING

Lighting must correctly follow animated geometry.

For example:

- moving cow normals change with leg/head transformations
- moving worker receives light at new world position
- rotating fan receives specular changes
- opening gate receives proper shading as its angle changes

Do not calculate static world-space normals that ignore model transformations.

---

# 56. COW LIGHTING

Cow surfaces should show:

- soft diffuse light
- relatively low specular
- clear form under directional light
- visible illumination near shed bulbs at night

Do not give cows mirror-like highlights.

Suggested shininess:

```text id="sdqqcc"
8–16
```

---

# 57. CALF LIGHTING

Use similar material settings to cows.

No special shader is needed.

The calf should inherit the same lighting system automatically.

---

# 58. HUMAN LIGHTING

Farm workers should receive:

- sunlight
- point light
- spotlight if near entrance

Clothing should have low-to-medium shininess.

Skin/head may use slightly different material settings if supported.

Do not require separate complex human shaders.

---

# 59. FAN LIGHTING

Fan blades are ideal for demonstrating specular highlights.

Use:

```text id="tsuw4r"
shininess ≈ 32–64
```

depending on appearance.

As fans rotate, the highlights may visibly change.

---

# 60. GATE LIGHTING

Metal/painted gate can use:

```text id="tz20ft"
shininess ≈ 24–32
```

At night, the entrance spotlight should create visible highlights on the gate.

This makes the gate area an excellent presentation location.

---

# 61. GRASS LIGHTING

Grass should have:

```text id="d0evxs"
very low specular
```

For example:

```text id="jo32n3"
shininess ≈ 2
specularColor low
```

Avoid plastic-looking grass.

---

# 62. SOIL LIGHTING

Soil should also have:

```text id="9s0zvv"
little or almost no specular
```

Diffuse and ambient should dominate.

---

# 63. WOOD LIGHTING

Fence, crates and wood structures:

```text id="a22lxw"
shininess ≈ 8
```

Moderate diffuse.

Low specular.

---

# 64. BRICK AND CONCRETE

Brick:

```text id="s7vq52"
shininess ≈ 4–8
```

Concrete:

```text id="tkfp44"
shininess ≈ 4
```

Do not make masonry shiny.

---

# 65. WATER TANK / SILO

If metal:

```text id="jho8jj"
shininess ≈ 48–64
```

This is another good object for demonstrating specular light.

---

# 66. HAY

Hay should be almost matte.

Use:

```text id="ey98jh"
shininess ≈ 2–4
```

---

# 67. SCENE-LEVEL LIGHT SETUP

Create a centralized function such as:

```cpp id="tswwq5"
void setupLighting(
    Shader& shader,
    const Camera& camera
);
```

This should send:

- camera position
- directional light
- point-light properties
- spotlight properties
- component toggles
- day/night values

Do not send identical light uniforms separately from every drawing function.

---

# 68. VIEW POSITION

Phong specular requires camera position.

Send:

```cpp id="cf24w6"
shader.setVec3(
    "viewPos",
    camera.Position
);
```

or equivalent.

Update it every frame because camera position changes.

---

# 69. MATERIAL SETUP FUNCTION

Create a helper such as:

```cpp id="8fg9gu"
void applyMaterial(
    Shader& shader,
    const Material& material
);
```

or integrate material properties into textured drawing functions.

Avoid repeated manual uniform-setting everywhere.

---

# 70. TEXTURED DRAW HELPER UPGRADE

The Module 2 helper may currently look like:

```cpp id="m9cgnj"
drawTexturedCube(...)
```

Upgrade it so it can accept material information.

For example:

```cpp id="d3m5hd"
void drawTexturedCube(
    Shader& shader,
    unsigned int texture,
    const glm::mat4& parent,
    glm::vec3 localPosition,
    glm::vec3 localRotation,
    glm::vec3 localScale,
    const Material& material
);
```

Do not rebuild geometry.

Only add necessary material uniforms.

---

# 71. MATERIAL PRESETS

Create reusable presets.

Example:

```cpp id="f39d7v"
Material grassMaterial;
Material soilMaterial;
Material woodMaterial;
Material brickMaterial;
Material metalMaterial;
Material cowMaterial;
Material clothMaterial;
```

This is cleaner than repeatedly writing:

```cpp id="jud943"
setFloat("shininess", 8.0f);
```

throughout the scene.

---

# 72. SHADER FILE PLAN

A good Module 4 structure may include:

```text id="bfqfoa"
shaders/
├── phongTexture.vs
├── phongTexture.fs
├── emissive.vs
└── emissive.fs
```

However, if a single Phong shader supports emissive material cleanly, a separate emissive shader is optional.

Do not create unnecessary duplicate shaders.

---

# 73. VERTEX SHADER REQUIREMENTS

The Phong texture vertex shader should accept:

```glsl id="3i8kop"
layout (location = 0)
in vec3 aPos;

layout (location = 1)
in vec3 aNormal;

layout (location = 2)
in vec2 aTexCoord;
```

and use:

```glsl id="fhc7ga"
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
```

---

# 74. FRAGMENT SHADER INPUT

Use:

```glsl id="qq33jy"
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
```

Output:

```glsl id="7ue79r"
out vec4 FragColor;
```

---

# 75. LIGHT ENABLE UNIFORMS

Suggested uniforms:

```glsl id="ch5jno"
uniform bool directionalEnabled;
uniform bool pointLightsEnabled;
uniform bool spotLightEnabled;

uniform bool ambientEnabled;
uniform bool diffuseEnabled;
uniform bool specularEnabled;
```

This makes presentation controls straightforward.

---

# 76. NIGHT/DAY LOGIC LOCATION

Do not hard-code:

```glsl id="9lhqo6"
if (nightMode)
```

through dozens of object shaders unless necessary.

Prefer CPU-side selection of light colors/intensities.

For example:

```cpp id="bjfmxy"
if (nightMode)
{
    setupNightLights(shader);
}
else
{
    setupDayLights(shader);
}
```

---

# 77. DAY LIGHT SETTINGS

A starting configuration might be:

```text id="j1mi4p"
Sun ambient:
moderate

Sun diffuse:
strong

Sun specular:
moderate

Point light brightness:
low or off

Spotlight:
low or off

Sky:
light blue
```

---

# 78. NIGHT LIGHT SETTINGS

A starting configuration might be:

```text id="pbs6xy"
Directional/moon ambient:
very weak

Directional diffuse:
weak

Point lights:
strong

Spotlight:
strong

Emission:
strong

Sky:
dark blue
```

---

# 79. DAY/NIGHT VS MANUAL TOGGLES

Define clear behavior.

Recommended:

### Day/night mode sets default intensity values.

Manual keys:

```text id="3isgjb"
1
2
3
```

still determine whether each light type contributes.

Example:

```text id="2utz8d"
Night mode
+
PointLightsEnabled = false

→ night scene without shed bulbs.
```

This is useful for demonstrating the system.

---

# 80. DO NOT MAKE KEY 4 OVERRIDE USER TOGGLES UNNECESSARILY

When switching day/night, it is acceptable to set default light states.

But a cleaner approach is:

```text id="fgwvdf"
day/night controls intensities
manual toggles control enabled/disabled state
```

This keeps behavior predictable.

---

# 81. PRESERVE MODULE 3 ANIMATION CONTROLS

Keep:

```text id="s0gnnn"
G = Gate
F = Fans
C = Cow animation
R = Calf running
H = Grazing
M = Worker movement
```

Do not break them.

---

# 82. COMPLETE MODULE 4 CONTROL PLAN

Use:

```text id="9r8mar"
CAMERA
W/A/S/D : Move
Q/E     : Down / Up
Mouse   : Look
Scroll  : Zoom

FARM / ANIMATION
G       : Gate Open/Close
F       : Fan ON/OFF
C       : Cow Animation ON/OFF
R       : Calf Running ON/OFF
H       : Grazing Animation ON/OFF
M       : Worker Animation ON/OFF

LIGHTING
1       : Directional Light ON/OFF
2       : Point Lights ON/OFF
3       : Spotlight ON/OFF
4       : Day/Night Toggle
5       : Ambient Component ON/OFF
6       : Diffuse Component ON/OFF
7       : Specular Component ON/OFF

ESC     : Exit
```

Reserve:

```text id="dnyq1b"
B
V
P
```

for Module 5 if needed.

---

# 83. KEY CALLBACK

Use single-press callback for all toggles.

For example:

```cpp id="19e43m"
if (
    key == GLFW_KEY_1 &&
    action == GLFW_PRESS
)
{
    directionalLightOn =
        !directionalLightOn;
}
```

Apply similarly to:

```text id="s8a99e"
2
3
4
5
6
7
```

---

# 84. DO NOT USE CONTINUOUS INPUT FOR TOGGLES

Do not write:

```cpp id="2f0xkc"
if (
    glfwGetKey(window, GLFW_KEY_4)
    == GLFW_PRESS
)
{
    nightMode = !nightMode;
}
```

inside the continuous movement function without key-state protection.

It will flicker rapidly.

---

# 85. RENDER LOOP STRUCTURE

The main loop should remain conceptually:

```cpp id="z2p015"
while (!glfwWindowShouldClose(window))
{
    updateDeltaTime();

    processInput(window);

    updateAnimations(deltaTime);

    updateEnvironmentState();

    clearBuffers();

    setupCamera();

    setupLighting(phongShader);

    renderScene(phongShader);

    renderEmissiveObjects();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Exact architecture may vary.

---

# 86. KEEP `renderScene()` CENTRALIZED

Do not duplicate the entire farm separately for:

```text id="at598g"
day mode
night mode
```

The same scene must be rendered.

Only lighting uniforms should change.

This is also necessary for Module 5 multiple viewports.

---

# 87. RENDER FUNCTIONS MUST REMAIN CAMERA-INDEPENDENT

Individual functions such as:

```cpp id="sbbzcq"
drawCow(...)
drawBarn(...)
drawFan(...)
```

should not decide:

- day/night
- viewport
- camera
- projection

They should draw their own geometry using currently active shader uniforms.

This maintains clean architecture.

---

# 88. SHADER SWITCHING

Minimize unnecessary shader switching.

Prefer one main textured Phong shader for most farm objects.

Use a separate emissive shader only if it significantly simplifies the lamp rendering.

Do not use a separate shader for every object type.

---

# 89. TEXTURE BINDING

Lighting must not break texture binding.

Before drawing a textured object:

```cpp id="mtilwh"
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, textureID);
```

and ensure sampler uniform is correct.

Do not accidentally bind point-light data to texture units or otherwise mix systems.

---

# 90. LIGHTING ON NON-TEXTURED OBJECTS

If some Module 3 body parts use solid colors instead of textures, handle them cleanly.

Possible options:

### Option A
Use a 1×1 solid-color texture.

### Option B
Use the same shader with:

```glsl id="8j3gkk"
uniform bool useTexture;
uniform vec3 baseColor;
```

Then:

```glsl id="6rbgin"
vec3 surfaceColor =
    useTexture
    ? texture(...).rgb
    : baseColor;
```

This is a clean solution.

---

# 91. DO NOT MAINTAIN TWO COMPLETELY DIFFERENT LIGHTING SYSTEMS

Avoid:

```text id="du7z2f"
textured objects → one lighting implementation
colored objects → unrelated lighting implementation
```

Use a unified lighting model if practical.

---

# 92. LIGHTING ON SPHERES

Ensure sphere normals point outward.

Do not use constant normals for all sphere vertices.

The cow head/human head should visibly shade smoothly.

---

# 93. LIGHTING ON CONES

Cone side normals must be meaningful.

Horn lighting should not appear flat or broken.

The cone base can have a downward/upward normal depending on orientation.

---

# 94. NORMAL NORMALIZATION

Normalize interpolated normals in the fragment shader:

```glsl id="px83pb"
vec3 norm =
    normalize(Normal);
```

Do not assume interpolation preserves unit length.

---

# 95. DIRECTIONAL LIGHT DIRECTION

Be consistent about whether:

```text id="4sejwk"
light.direction
```

means:

```text id="nfev4y"
direction light travels
```

or:

```text id="gtmdb8"
direction from fragment toward light
```

Typical implementation:

```glsl id="8cbdft"
vec3 lightDir =
    normalize(-light.direction);
```

Document the convention.

---

# 96. SPECULAR REFLECTION

Do not compute specular when:

```text id="9zcpbo"
dot(normal, lightDir) < 0
```

if it produces unrealistic backside highlights.

A simple guard is acceptable:

```glsl id="tafsx9"
if (diff > 0.0)
{
    calculate specular;
}
```

---

# 97. OPTIONAL BLINN-PHONG

Do not switch to Blinn-Phong unless course material explicitly supports it and it simplifies the work.

Use traditional Phong reflection if that matches the slides/resources.

---

# 98. GAMMA CORRECTION

Gamma correction is not required.

Do not add additional complexity unless already part of the course framework.

---

# 99. SHADOWS

Do NOT implement shadow mapping in Module 4 unless explicitly required by my assignment.

The focus is:

```text id="vh51ef"
light types
+
Phong components
+
attenuation
+
emission
+
day/night
```

Shadows are outside this module.

---

# 100. FOG

Do not add fog unless completely optional and independent.

It is not required.

---

# 101. NIGHT VISUAL TARGET

Night mode should clearly show:

```text id="7ivkj6"
dark environment

        glowing shed bulbs
               ↓
        illuminated cows

bright barn light

entrance spotlight
        ↓
bright road area

metal highlights on:
gate
fans
water tank
```

This should be one of the strongest presentation moments.

---

# 102. DAY VISUAL TARGET

Day mode should clearly show:

```text id="gj5ng7"
bright sky
sunlit grass
lit barn walls
cow body form
trees
gate
shed
workers
```

Artificial lights should be visually less dominant.

---

# 103. PRESENTATION LIGHTING SEQUENCE

During the presentation, demonstrate in this order:

## Step 1
Show normal day mode.

## Step 2
Press:

```text id="oczlw4"
1
```

Turn sunlight off.

Explain directional light.

## Step 3
Turn sunlight back on.

## Step 4
Press:

```text id="51vb3u"
5
```

Disable ambient.

Observe dark shadow-facing surfaces.

## Step 5
Enable ambient.

## Step 6
Press:

```text id="f3llxi"
6
```

Disable diffuse.

Show the loss of surface form.

## Step 7
Enable diffuse.

## Step 8
Move close to metal gate/tank.

Press:

```text id="k3l7ko"
7
```

Toggle specular.

Show highlights disappearing/appearing.

## Step 9
Press:

```text id="ru5n9u"
4
```

Switch to night.

## Step 10
Show glowing shed bulbs.

## Step 11
Press:

```text id="46n51l"
2
```

Turn point lights off/on.

## Step 12
Move to entrance.

Press:

```text id="8jovve"
3
```

Turn entrance spotlight off/on.

This demonstrates nearly the entire lighting syllabus.

---

# 104. LIGHTING MUST WORK WHILE ANIMATIONS RUN

Do not freeze Module 3 animations in Module 4.

At night, the user should be able to see:

- walking cow moving through changing light
- calves entering/leaving point-light ranges
- workers walking near barn lights
- rotating fan highlights
- gate opening under spotlight

This integration is important.

---

# 105. POINT LIGHT DISTANCE DEMONSTRATION

The attenuation should be obvious.

A cow close to the shed bulb should be brighter than a cow far across the field.

If the entire farm is equally bright from one point bulb, attenuation is incorrect or poorly tuned.

---

# 106. SPOTLIGHT DEMONSTRATION

The entrance spotlight should illuminate a cone-shaped region.

Objects outside the cone should receive little/no spotlight contribution.

Do not simply make it behave like another point light.

---

# 107. LIGHT FIXTURE ALIGNMENT TEST

Check the point light positions visually.

Bulb geometry:

```text id="voxrjr"
●
```

should coincide with:

```text id="y7nznf"
light.position
```

not be offset by several units.

---

# 108. EMISSION VS ACTUAL LIGHTING

Remember:

```text id="4tzcvv"
Emission
```

makes an object itself appear bright.

It does NOT automatically illuminate nearby surfaces.

Nearby illumination must still come from:

```text id="4ahcqw"
PointLight
or
SpotLight
```

This distinction should be correctly implemented.

---

# 109. MODULE 4 FILE ORGANIZATION

A possible structure after Module 4:

```text id="9qo2jz"
AgroFarm/
│
├── main.cpp
├── shader.h
├── camera.h
│
├── lighting/
│   ├── lights.h
│   └── materials.h
│
├── primitives/
│   ├── cube.h
│   ├── sphere.h
│   └── cone.h
│
├── models/
│   ├── cow.h
│   ├── human.h
│   └── fan.h
│
├── scene/
│   ├── farmEnvironment.h
│   ├── farmBuildings.h
│   └── farmProps.h
│
├── shaders/
│   ├── phongTexture.vs
│   ├── phongTexture.fs
│   ├── emissive.vs
│   └── emissive.fs
│
└── textures/
```

Do not force this exact structure if Module 3 is organized differently.

Preserve working code.

---

# 110. LIGHT DATA STRUCTURES — CPU SIDE

If useful, create:

```cpp id="7ekdmx"
struct DirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
```

and:

```cpp id="8ngjf8"
struct PointLightData
{
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
```

and corresponding spotlight data.

This is cleaner than using dozens of unrelated global variables.

---

# 111. DO NOT OVER-ENGINEER LIGHT CLASSES

A simple struct + shader uniform setup function is sufficient.

Do not spend excessive time creating a complex inheritance hierarchy such as:

```text id="9t82kb"
BaseLight
→ PhysicalLight
→ OmniLight
→ AreaLight
```

That is unnecessary for this project.

---

# 112. CENTRAL LIGHT INITIALIZATION

Create:

```cpp id="1d1h85"
void initializeLights();
```

Set:

- positions
- directions
- attenuation
- default colors

once.

Dynamic intensity/day-night values can update each frame.

---

# 113. CENTRAL LIGHT UNIFORM SETUP

Create:

```cpp id="812kfo"
void sendLightUniforms(
    Shader& shader
);
```

or equivalent.

It should set:

- directional light
- point-light array
- spotlight
- enabled toggles
- component toggles

Avoid duplicate uniform code in `main.cpp`.

---

# 114. OPTIONAL LIGHT POSITION CONSTANTS

Store farm fixture coordinates in meaningful variables.

For example:

```cpp id="zmn190"
glm::vec3 shedLight1Position;
glm::vec3 shedLight2Position;
glm::vec3 barnLightPosition;
glm::vec3 gateSpotPosition;
```

Do not repeatedly hard-code coordinates in both:

- lamp drawing
- shader setup

Ideally use the same position variable for both.

---

# 115. ENTRANCE SPOTLIGHT DIRECTION

Calculate or store:

```cpp id="cxsw5x"
glm::vec3 gateSpotDirection;
```

It should point from the lamp toward the entrance road.

Example concept:

```text id="9fuz9c"
       LAMP
        \
         \
          \ light cone
           \
           ROAD
```

---

# 116. OPTIONAL CAMERA FLASHLIGHT

Do NOT replace the required entrance spotlight with a camera flashlight.

A camera flashlight may be an optional extra feature.

The required spotlight must be part of the farm environment.

---

# 117. MATERIAL SPECULAR COLOR

Metal:

```text id="tvvkqu"
high specular value
```

Grass:

```text id="25ccif"
low specular value
```

This should be part of material setup.

Do not control shininess alone while every material uses the same white specular intensity.

---

# 118. OBJECT TINT

If the Module 2 shader supports texture tinting, preserve it.

Lighting should apply to:

```text id="dfvwes"
texture × tint
```

where needed.

This may be useful for cows/worker clothing.

---

# 119. PERFORMANCE

Module 4 should remain smooth.

Do not:

- create shader programs every frame
- reload textures every frame
- query uniform locations excessively if shader wrapper already caches or manages them
- recreate light objects every draw call
- regenerate meshes

The number of point lights is small, so forward Phong shading is sufficient.

---

# 120. OPENGL ERROR CHECKING

During development, check for:

```text id="e2je16"
shader compilation errors
shader linking errors
uniform naming mismatch
texture unit mismatch
```

Print useful error messages.

---

# 121. GLSL VERSION

Use the GLSL version compatible with the existing course setup.

Do not arbitrarily change to a newer unsupported GLSL version.

For example, if existing project uses:

```glsl id="vkaxdk"
#version 330 core
```

continue with it unless there is a reason not to.

---

# 122. UNIFORM NAMING CONSISTENCY

CPU:

```cpp id="86bw8i"
shader.setVec3(
    "dirLight.direction",
    ...
);
```

must exactly match shader:

```glsl id="9824js"
uniform DirLight dirLight;
```

Check every nested structure name carefully.

---

# 123. POINT LIGHT ARRAY UNIFORMS

For multiple point lights, construct names carefully.

Conceptually:

```cpp id="sq22cf"
for (int i = 0; i < NR_POINT_LIGHTS; ++i)
{
    std::string prefix =
        "pointLights[" +
        std::to_string(i) +
        "].";

    shader.setVec3(
        prefix + "position",
        pointLights[i].position
    );
}
```

Set all required fields.

---

# 124. DO NOT FORGET ATTENUATION UNIFORMS

For each point light send:

```text id="78u3wm"
constant
linear
quadratic
```

A common bug is declaring them in GLSL but never assigning CPU values.

---

# 125. CAMERA MOVEMENT TEST

Lighting should not change incorrectly when the camera moves, except specular highlights.

If diffuse/ambient lighting appears attached to the camera, something is wrong with coordinate spaces.

Use consistent world-space calculations.

---

# 126. WORLD SPACE CONSISTENCY

Use:

```text id="w5wdag"
FragPos = world space
light positions = world space
camera position = world space
normal = transformed to world space
```

Do not mix view-space and world-space quantities.

---

# 127. MOVING OBJECT MODEL MATRICES

For every animal part:

```text id="x15yc8"
model matrix
```

changes according to hierarchical animation.

The vertex shader should automatically transform:

- position
- normal

using that model matrix.

No special animated-lighting code should be needed.

---

# 128. FAN NORMAL TRANSFORMATION

Because fan blades rotate:

```text id="h9h9m1"
model matrix changes
```

so normals should rotate accordingly.

This produces changing specular highlights.

---

# 129. DAY/NIGHT AND TEXTURE COLOR

Do not completely replace texture colors in night mode.

The textures should still be visible under artificial lighting.

Night mode should change illumination, not swap every texture to black.

---

# 130. NIGHT SKY

Use a clear color such as:

```text id="8422me"
dark blue
```

rather than pure black if it gives better depth.

Exact color is flexible.

---

# 131. DAY SKY

Use a light sky blue.

Do not create an expensive sky system unless needed.

---

# 132. OPTIONAL SUN/MOON VISUAL OBJECT

You may create a small decorative sun/moon object.

This is optional.

It is NOT the actual directional-light mechanism.

The directional light remains mathematically positionless.

Do not let this extra object complicate the module.

---

# 133. NO MODULE 5 CURVES YET

Do NOT implement:

- Bézier milk can
- spline object
- ruled-surface roof
- surface of revolution

yet.

Those belong to Module 5.

---

# 134. NO MULTI-VIEW YET

Do NOT implement:

- bird's-eye mode
- four viewports
- front/side/top view

yet.

Those belong to Module 5.

---

# 135. PHONG VS GOURAUD COMPARISON

Keep **Phong shading** as Module 4's required renderer.

If the final project later needs Phong/Gouraud comparison, implement the actual toggle in Module 5.

Do not complicate Module 4 unless the current assignment specifically requires both immediately.

---

# 136. MODULE 4 EXPECTED DAY SCENE

During day:

```text id="my367h"
                   SUNLIGHT
                 ↘ ↓ ↓ ↓ ↙

         TREE        COW SHED
                       FAN

     Cow      Cow              BARN
         Calf

              Worker

       bright green field

                 road

        gate / entrance
```

Directional light should dominate.

---

# 137. MODULE 4 EXPECTED NIGHT SCENE

During night:

```text id="tpg4ds"
                 DARK SKY


            ● shed light
           /|\
          / | \
        cow cow cow


                          ● barn light


                         worker


                ▼ spotlight cone
               / \
              /   \
           entrance road
              gate
```

Artificial lighting should dominate.

---

# 138. MODULE 4 TESTING PLAN

Perform tests in this order.

## Test 1 — Normals

Render using directional light.

Inspect:

- cubes
- spheres
- cows
- trees

Look for inverted or dark faces.

---

## Test 2 — Ambient only

Disable:

```text id="nszfrv"
diffuse
specular
```

Keep ambient.

Scene should remain uniformly dim.

---

## Test 3 — Diffuse

Enable diffuse.

Object shapes should become much clearer.

---

## Test 4 — Specular

Inspect:

- gate
- water tank
- fan

Toggle specular.

Highlights should visibly change.

---

## Test 5 — Directional light

Disable sunlight.

Day scene should become substantially darker.

---

## Test 6 — Point lights

Switch to night.

Disable point lights.

Shed/barn illumination should disappear.

Enable again.

---

## Test 7 — Attenuation

Walk from a shed bulb into the grazing field.

Light intensity should decrease with distance.

---

## Test 8 — Spotlight

Stand near entrance at night.

Toggle spotlight.

Only the cone-shaped entrance region should change strongly.

---

## Test 9 — Emission

Disable point light.

Compare bulb surface appearance.

When lamp state is off, strong glow should disappear.

---

## Test 10 — Animation + lighting

Run:

- cow walking
- calf
- workers
- fans
- gate

while night lighting is active.

Check for broken normals/matrices.

---

# 139. MODULE 4 ACCEPTANCE CHECKLIST

Do not consider Module 4 complete unless:

- Modules 1–3 still work.
- All textures still render.
- Gate still animates.
- Cows still animate.
- Calves still run.
- Humans still animate.
- Fans still rotate.
- Main Phong shader compiles.
- Vertex normals are correct.
- Normal matrix is used.
- Directional light exists.
- Directional light affects full farm.
- Point lights exist.
- Point lights have attenuation.
- Point lights correspond to visible bulbs.
- Entrance spotlight exists.
- Spotlight uses cutoff.
- Spotlight has soft edge if practical.
- Spotlight uses attenuation.
- Emissive lamp objects exist.
- Ambient contribution works.
- Diffuse contribution works.
- Specular contribution works.
- `1` toggles directional light.
- `2` toggles point lights.
- `3` toggles spotlight.
- `4` toggles day/night.
- `5` toggles ambient.
- `6` toggles diffuse.
- `7` toggles specular.
- Day sky differs from night sky.
- Artificial lights dominate at night.
- Sun dominates during day.
- Metal objects have stronger specular than grass.
- Animated objects receive correct lighting.
- No geometry buffers are recreated every frame.
- No Module 5 advanced curve/view features are added prematurely.

---

# 140. STARTUP CONSOLE OUTPUT

Update the application output to something like:

```text id="lip50e"
====================================================
          INTERACTIVE 3D AGRO FARM
                  MODULE 4
====================================================

CAMERA
W/A/S/D : Move
Q/E     : Down / Up
Mouse   : Look Around
Scroll  : Zoom

FARM
G       : Open / Close Gate

ANIMATION
F       : Fans ON/OFF
C       : Adult Cow Animation ON/OFF
R       : Calf Running ON/OFF
H       : Grazing Animation ON/OFF
M       : Worker Animation ON/OFF

LIGHTING
1       : Directional Light ON/OFF
2       : Point Lights ON/OFF
3       : Entrance Spotlight ON/OFF
4       : Day / Night
5       : Ambient ON/OFF
6       : Diffuse ON/OFF
7       : Specular ON/OFF

ESC     : Exit
====================================================
```

---

# 141. CODE QUALITY REQUIREMENTS

Module 4 code must:

- compile
- preserve previous modules
- use clear names
- avoid duplicated lighting equations
- centralize light setup
- use proper normals
- use proper coordinate spaces
- use proper normal transformation
- initialize shader accumulators
- avoid excessive globals when structures are cleaner
- keep rendering modular
- keep light controls understandable
- remain prepared for Module 5 multi-view rendering

---

# 142. IMPORTANT MODULE 5 COMPATIBILITY

Module 5 will render the scene from several cameras/viewports.

Therefore, Module 4 must NOT make lighting dependent on one fixed camera.

The only camera-dependent light value should normally be:

```text id="ivjbbl"
viewPos
```

for specular reflection.

When Module 5 uses another viewport/camera, it should simply send the correct camera position before rendering.

---

# 143. LIGHTING SETUP MUST SUPPORT MULTIPLE RENDERS

Design:

```cpp id="0zj830"
setupLighting(
    shader,
    cameraPosition
);
```

so Module 5 can call:

```text id="wn4ner"
setupLighting for perspective camera
renderScene

setupLighting for bird's-eye camera
renderScene

setupLighting for front camera
renderScene

...
```

without rewriting light logic.

---

# 144. DO NOT CLEAR OR SWAP INSIDE OBJECT FUNCTIONS

Functions such as:

```cpp id="8g25mw"
drawCow()
drawBarn()
drawLights()
```

must not call:

```text id="s1yp3j"
glClear
glfwSwapBuffers
glViewport
```

This remains essential for Module 5.

---

# 145. WHAT I WANT FROM YOU

Now implement **MODULE 4 only**, continuing directly from the completed Module 3 project.

Before coding:

1. inspect the existing Module 3 project;
2. inspect Lab 3 lighting code from the supplied course resources;
3. inspect the current Module 2/3 texture shaders;
4. verify all primitive vertex normals;
5. locate the existing lamp fixtures;
6. locate actual shed/barn/gate coordinates;
7. inspect all current animation transforms;
8. determine the best way to integrate Phong lighting without breaking the existing architecture.

Then implement Module 4 systematically.

---

# 146. IMPLEMENTATION OUTPUT REQUIREMENT

Do not give only isolated snippets.

Provide the complete compile-ready implementation required to upgrade Module 3 into Module 4.

For every created or modified file:

- clearly state filename;
- provide complete required content;
- ensure all uniform names match;
- ensure shader files compile;
- ensure CPU-side structures match GLSL structures.

Do not leave required logic as:

```text id="2exknq"
// TODO lighting
// same as before
// add shader code here
```

---

# 147. FINAL RESPONSE REQUIREMENTS

At the end of implementation provide:

## A. Files changed

List all:

- `.cpp`
- `.h`
- shader files

that were added or modified.

## B. Lighting architecture

Explain briefly:

```text id="rkruzt"
Directional light
Point-light array
Spotlight
Emission
Material
Day/night state
```

## C. Exact light positions

State the final coordinates used for:

- shed point light 1
- shed point light 2
- barn point light
- entrance spotlight

## D. Material presets

List the shininess/specular values used for major farm materials.

## E. Controls

Provide the complete current keyboard controls.

## F. Day mode behavior

Explain what lights dominate.

## G. Night mode behavior

Explain what lights dominate.

## H. Testing procedure

Give the ordered test sequence.

## I. Expected visual result

Describe exactly how the completed farm should look during:

- day
- night

## J. Module 5 readiness

Confirm that:

- `renderScene()` remains centralized;
- lighting can be reused with multiple cameras;
- all geometry supplies normals;
- curves can be added later;
- multiple viewports can be added without rewriting Module 4.

Do NOT implement Module 5 yet.

---

# FINAL MODULE 4 PRIORITY

The final Module 4 project should demonstrate clearly:

**Phong shading + ambient light + diffuse light + specular light + directional sunlight + point lights + distance attenuation + spotlight cutoff + emissive fixtures + material properties + day/night transformation + lighting on animated textured objects.**

The visual goal is:

> During the day, the complete animated agro farm is illuminated naturally by sunlight. At night, the environment becomes dark and the farm's bulbs and entrance spotlight become the dominant visible light sources, while cows, calves, workers, fans, buildings, grass, gate, and other textured objects continue to render and animate correctly under the new lighting system.

Stability and correct graphics concepts are more important than unnecessary advanced effects.