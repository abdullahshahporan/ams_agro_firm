I am continuing my **CSE 4208 Computer Graphics project in C++ using Modern OpenGL**.

The final project is:

# Interactive 3D Agro Farm Simulation

Modules 1–4 are already complete.

Now implement the final stage:

# MODULE 5 — CURVED GEOMETRY, BÉZIER/SPLINE/RULED SURFACES, MULTIPLE VIEWPORTS, ADVANCED CAMERA MODES, AND FINAL INTEGRATION

This is the final module.

Do NOT start a new project.

Extend the completed Module 4 project directly.

Do NOT remove, simplify, or break any previous functionality.

---

# 1. EXISTING PROJECT STATE

## Module 1

Already includes:

- Modern OpenGL
- GLFW
- GLAD
- GLM
- depth testing
- model/view/projection transformations
- free camera
- mouse look
- zoom
- farm terrain
- entrance
- boundary fence
- animated double gate
- modular rendering
- delta-time architecture

## Module 2

Already includes:

- texture mapping
- textured grass
- textured road
- textured gate/fence
- detailed cow shed
- barn/feed store
- feeding trough
- hay
- trees
- water/silo structures
- farm props
- static farm architecture

## Module 3

Already includes:

- adult cows
- calves
- humans
- hierarchical modeling
- cow walking
- cow grazing
- cow feeding
- tail/head movement
- running calves
- worker walking
- fan animation
- animation-state architecture

## Module 4

Already includes:

- textured Phong shading
- correct normals
- directional sunlight
- point lights
- attenuation
- entrance spotlight
- emissive bulbs
- material shininess
- ambient/diffuse/specular controls
- day/night mode
- lighting on animated objects

Module 5 must preserve everything above.

---

# 2. PRIMARY MODULE 5 OBJECTIVE

The final project must now demonstrate the remaining advanced graphics topics from the supplied course materials.

Module 5 should add:

1. Bézier curve/surface based farm object.
2. B-spline or spline-based curved geometry.
3. Ruled surface.
4. Surface-of-revolution style geometry if supported by the supplied Lab 5 implementation.
5. Bird’s-eye farm view.
6. Multiple camera/view modes.
7. Four simultaneous viewports.
8. Perspective and orthographic projection where appropriate.
9. Phong/Gouraud comparison if practical and relevant to supplied resources.
10. Custom transformation functions if required by the course assignment.
11. Final UI/control organization.
12. Final scene cleanup.
13. Performance optimization.
14. Final demonstration flow.
15. Final project architecture suitable for submission.

The goal is not to add random features.

Every advanced feature should have a logical role inside the agro farm.

---

# 3. COURSE RESOURCE REQUIREMENT

Carefully inspect the supplied Lab 5 resources before implementing this module.

Pay special attention to:

- Bézier curve generation
- control points
- surface generation
- surface of revolution
- spline curves
- ruled surfaces
- camera functions
- projection/view transformations
- multiple viewports if present in earlier labs
- Phong/Gouraud shader examples
- custom transformation requirements from assignment instructions

Reuse course-compatible logic.

Do not introduce:

- Blender runtime
- Assimp
- external spline libraries
- OpenCascade
- game engines
- geometry-processing frameworks

Implement the required mathematics using C++, GLM, and Modern OpenGL.

---

# 4. MODULE 5 IMPLEMENTATION ORDER

Implement in this order:

## Phase 5.1
Audit existing `renderScene()` for multi-camera compatibility.

## Phase 5.2
Add Bézier-based farm object.

## Phase 5.3
Add spline-based farm object.

## Phase 5.4
Add ruled-surface cow-shed roof.

## Phase 5.5
Integrate textures/normals/Phong lighting with curved geometry.

## Phase 5.6
Add bird’s-eye camera.

## Phase 5.7
Add front/side/top cameras.

## Phase 5.8
Add orthographic projection for technical views.

## Phase 5.9
Add four-viewport mode.

## Phase 5.10
Optional Phong/Gouraud comparison.

## Phase 5.11
Custom matrix functions if required.

## Phase 5.12
Final controls and presentation modes.

## Phase 5.13
Performance optimization.

## Phase 5.14
Final full-project testing.

Do not begin with four viewports before confirming curved geometry works.

---

# 5. IMPORTANT RULE — PRESERVE ONE CENTRAL SCENE

There must still be one main scene function.

Conceptually:

```cpp
void renderScene(
    Shader& shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& viewPosition
);
```

or an equivalent clean design.

Do NOT create separate functions such as:

```cpp
renderPerspectiveFarm();
renderTopFarm();
renderSideFarm();
renderFrontFarm();
```

that duplicate all farm geometry.

The same farm must be rendered from multiple cameras.

---

# 6. MODULE 5 CURVED OBJECT PLAN

Use the advanced geometry meaningfully.

Recommended assignment:

```text
Bézier / Surface of Revolution
→ Traditional milk can

Spline Curve
→ Irrigation pipe / curved water pipe

Ruled Surface
→ Curved cow-shed roof
```

This gives each mathematical technique a clear farm-related purpose.

Do not create floating mathematical demonstrations disconnected from the farm.

---

# 7. BÉZIER OBJECT — TRADITIONAL MILK CAN

Create a traditional milk container near:

- cow shed
- feeding area
- milk collection platform

This should be the primary Bézier object.

Use the supplied Lab 5 Bézier/surface generation method if appropriate.

---

# 8. MILK CAN VISUAL DESIGN

The object should approximately resemble:

```text
          ______
         /      \
        /        \
        |        |
        |        |
       /          \
      /            \
      |            |
      |            |
      |            |
      |____________|
```

A milk can is an ideal **surface of revolution** object.

Define one side profile in 2D and rotate it around the vertical axis.

---

# 9. BÉZIER CURVE FORMULA

For a cubic Bézier segment:

\[
B(t)
=
(1-t)^3P_0
+
3(1-t)^2tP_1
+
3(1-t)t^2P_2
+
t^3P_3
\]

where:

\[
0 \leq t \leq 1
\]

Use enough samples to produce a smooth profile.

Example:

```text
20–50 samples per segment
```

depending on performance.

---

# 10. MULTIPLE BÉZIER SEGMENTS

A milk can profile will likely require multiple curve segments.

For example:

```text
Segment 1
bottom/body

Segment 2
upper shoulder

Segment 3
neck

Segment 4
rim
```

Join them smoothly.

The exact control points depend on final scale.

---

# 11. CONTROL POINT STORAGE

Do not scatter control points as unexplained numbers throughout rendering code.

Use a structure such as:

```cpp
std::vector<glm::vec3> milkCanControlPoints;
```

or:

```cpp
struct BezierSegment
{
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
};
```

This makes the geometry understandable.

---

# 12. SURFACE OF REVOLUTION

If the supplied Lab 5 code uses a function conceptually similar to:

```cpp
hollowBezier(...)
```

or rotates Bézier points around an axis, reuse/adapt that approach.

For each profile point:

```text
radius = x-coordinate
height = y-coordinate
```

Rotate around Y:

\[
x = r\cos\theta
\]

\[
z = r\sin\theta
\]

\[
y = h
\]

with:

\[
0\le \theta \le 2\pi
\]

---

# 13. REVOLUTION MESH RESOLUTION

Use reasonable resolution.

For example:

```text
profile samples:
30–60

angular segments:
24–48
```

Do not generate hundreds of thousands of unnecessary vertices.

---

# 14. MILK CAN TRIANGULATION

Connect neighboring profile rings.

For each:

```text
profile row i
and
profile row i+1
```

connect angular vertices into triangles.

Example:

```text
v00 ----- v01
 |      / |
 |    /   |
 |  /     |
v10 ----- v11
```

Triangles:

```text
v00, v10, v11

v00, v11, v01
```

Use EBO indices if practical.

---

# 15. MILK CAN NORMALS

The Bézier milk can must participate in Module 4 Phong lighting.

Do not render it with no normals.

Calculate smooth normals.

Possible methods:

### Method A
Use neighboring surface derivatives.

### Method B
Calculate triangle normals and average per vertex.

### Method C
Use radial/profile normals based on surface-of-revolution mathematics.

Choose a stable implementation.

The final object should shade smoothly.

---

# 16. MILK CAN TEXTURE/MATERIAL

Use:

```text
metal texture
```

or metallic-looking gray texture.

Suggested material:

```text
shininess = 48–64
```

The object should show strong specular highlights.

This is an excellent object for Module 4 lighting demonstration.

---

# 17. MILK CAN POSITION

Place it logically.

For example:

```text
near cow shed
or
near milk collection area
```

Do not put it in the center of the road.

Use approximately human/cow-realistic scale.

---

# 18. OPTIONAL MILK CAN HANDLES

Handles are optional.

If added:

- use small curved/spline geometry
or
- simple thin cuboids

Do not let handles delay the core Bézier surface.

---

# 19. SPLINE OBJECT

Create a farm object using a spline.

Recommended:

# Curved Irrigation Pipe

The pipe can run from:

```text
water tank
toward
grazing field
```

This is more meaningful than drawing an arbitrary spline in the air.

---

# 20. B-SPLINE FORMULA

For a uniform cubic B-spline segment, you may use:

\[
P(u)
=
\frac{1}{6}
\left[
(1-u)^3P_0
+
(3u^3-6u^2+4)P_1
+
(-3u^3+3u^2+3u+1)P_2
+
u^3P_3
\right]
\]

where:

\[
0\leq u\leq1
\]

If the supplied Lab 5 uses a different spline formulation, follow the course implementation.

Do not substitute a mathematically unrelated curve.

---

# 21. SPLINE CONTROL POINTS

Design an irrigation route such as:

```text
Water Tank
   |
   |____
        \
         \
          \____ Grazing Area
```

Use approximately:

```text
5–8 control points
```

for a smooth curve.

Do not use excessive control points.

---

# 22. SPLINE VISUALIZATION

The final spline should represent an actual object, not just a mathematical line.

Good implementation options:

### Option A
Generate a thin tube along the spline.

### Option B
Render a thick line if tube generation is too complex.

### Option C
Use repeated small cylinders/segments along the spline.

Option A is strongest.

Option C is acceptable.

---

# 23. SPLINE PIPE TUBE

If implementing a tube:

At each sampled spline position:

1. calculate tangent;
2. create a local frame;
3. generate a small circular ring;
4. connect consecutive rings.

Suggested:

```text
pipe radial segments:
8–16

spline samples:
50–100
```

Keep geometry manageable.

---

# 24. SPLINE TANGENT

Estimate tangent using:

\[
T(u)
\approx
P(u+\Delta u)
-
P(u-\Delta u)
\]

then normalize it.

Or analytically differentiate the spline if convenient.

Do not use one constant orientation for the entire pipe.

---

# 25. PIPE NORMALS

If generating a tube, normals should point outward from the centerline.

The pipe must receive Phong lighting.

Use a low-to-medium metal/plastic shininess depending on material.

---

# 26. PIPE MATERIAL

Suggested:

```text
dark plastic irrigation pipe
```

or:

```text
metal pipe
```

Use something visually distinguishable from grass.

---

# 27. RULED SURFACE

Use the cow-shed roof as the primary ruled surface.

This is particularly appropriate because the roof already exists from earlier modules.

Replace or upgrade the temporary simple Module 2 roof.

---

# 28. RULED SURFACE FORMULA

Given two curves:

\[
C_1(u)
\]

and:

\[
C_2(u)
\]

define:

\[
S(u,v)
=
(1-v)C_1(u)
+
vC_2(u)
\]

where:

\[
0\leq u\leq1
\]

and:

\[
0\leq v\leq1
\]

This creates a surface joining the two curves.

---

# 29. COW-SHED ROOF DESIGN

Use two corresponding curved edge profiles.

Example:

```text
Front curved roof edge

      ______
   __/      \__

Back curved roof edge

      ______
   __/      \__
```

Join them across the shed depth.

The result should resemble a curved metal shelter.

---

# 30. DO NOT KEEP BOTH ROOFS OVERLAPPING

If replacing the existing simple shed roof:

remove or disable the old roof geometry.

Do not render:

```text
old flat roof
+
new ruled roof
```

at the same location.

This would cause z-fighting and visual confusion.

---

# 31. RULED SURFACE SAMPLING

Use enough:

```text
u samples
v samples
```

for smooth appearance.

Starting point:

```text
u = 20–40
v = 8–20
```

Tune according to shape.

---

# 32. RULED SURFACE TRIANGULATION

For grid vertices:

```text
S(i,j)
S(i+1,j)
S(i,j+1)
S(i+1,j+1)
```

generate two triangles per cell.

Use an EBO if practical.

---

# 33. RULED ROOF NORMALS

Calculate proper normals.

Possible method:

\[
N
=
\frac{
\partial S/\partial u
\times
\partial S/\partial v
}{
\left|
\partial S/\partial u
\times
\partial S/\partial v
\right|
}
\]

or average triangle normals.

Do not assign all roof vertices the same upward normal if the roof is curved.

---

# 34. ROOF TEXTURE

Apply:

```text
corrugated metal
```

or existing roof texture.

Make texture coordinates follow the surface reasonably.

Texture mapping does not need to be perfect architectural UV mapping.

It should not appear randomly stretched.

---

# 35. ROOF MATERIAL

Suggested:

```text
shininess:
24–48
```

depending on whether the roof is painted/metallic.

At night, point lights underneath should illuminate portions of it.

---

# 36. ADVANCED CURVE FILE ORGANIZATION

Create reusable files if helpful:

```text
curves/
├── bezierSurface.h
├── spline.h
└── ruledSurface.h
```

or:

```text
curvedGeometry.h
```

Do not put hundreds of geometry-generation lines directly inside `main.cpp`.

---

# 37. CURVED MESH RESOURCE LIFETIME

Generate curved geometry:

```text
once during initialization
```

or only when its control points change.

Do NOT regenerate the milk can, spline pipe, or roof every frame if the geometry is static.

Create:

- VAO
- VBO
- EBO

once.

---

# 38. CURVED OBJECT DRAW FUNCTION

Use functions such as:

```cpp
drawMilkCan(...);
drawIrrigationPipe(...);
drawRuledRoof(...);
```

Drawing should only:

- bind shader
- set model/material
- bind texture
- bind VAO
- draw

not regenerate the mesh.

---

# 39. INTEGRATE CURVED OBJECTS INTO `renderScene()`

The central scene should now include:

```cpp
drawGround();
drawRoad();

drawBoundary();
drawEntrance();

drawCowShed();
drawRuledRoof();

drawBarn();

drawFeedingArea();
drawHayArea();

drawWaterFacility();
drawIrrigationPipe();

drawMilkCan();

drawTrees();
drawProps();

drawAnimals();
drawHumans();
drawFans();
drawLightFixtures();
```

Do not render curved objects in a disconnected test function outside the farm.

---

# 40. BIRD'S-EYE VIEW

Now add a dedicated bird’s-eye camera.

Use:

```text
B = Bird's-eye view toggle
```

The bird’s-eye camera should show nearly the entire farm.

---

# 41. BIRD'S-EYE CAMERA POSITION

A reasonable starting position:

```cpp
glm::vec3(
    0.0f,
    32.0f,
    2.0f
);
```

or approximately:

```text
Y = 30–40
```

depending on scene scale.

Look toward the farm center:

```cpp
glm::vec3(
    0.0f,
    0.0f,
    0.0f
);
```

Avoid exact look-direction degeneracy if camera up becomes parallel to viewing direction.

---

# 42. BIRD'S-EYE UP VECTOR

If looking almost perfectly downward, standard:

```cpp
glm::vec3(0,1,0)
```

can become problematic because it may align with view direction.

Use an appropriate alternative such as:

```cpp
glm::vec3(0,0,-1)
```

for top-down `lookAt` if needed.

Verify matrix stability.

---

# 43. BIRD'S-EYE PROJECTION

Two good options:

### Option A
Perspective bird’s-eye.

### Option B
Orthographic bird’s-eye.

Use:

```text
orthographic
```

for the technical top view if the course requires orthographic projection.

This is strongly recommended.

---

# 44. ORTHOGRAPHIC PROJECTION

For top/front/side technical views, use:

```cpp
glm::ortho(
    left,
    right,
    bottom,
    top,
    nearPlane,
    farPlane
);
```

Example conceptual bounds:

```text
-20 to +20
```

or values adjusted to farm scale.

---

# 45. FREE CAMERA VS BIRD'S-EYE MODE

Preserve the existing free perspective camera.

Use an enum:

```cpp
enum class CameraMode
{
    Free,
    BirdEye
};
```

or equivalent.

Do not permanently replace the Module 1 free camera.

---

# 46. BIRD'S-EYE KEY BEHAVIOR

Press:

```text
B
```

from normal mode:

```text
Free Camera
→ Bird's-eye
```

Press again:

```text
Bird's-eye
→ Free Camera
```

The original free-camera position/orientation should preferably be preserved.

---

# 47. NORMAL CAMERA CONTROLS IN BIRD'S-EYE

It is acceptable to disable mouse movement while bird’s-eye mode is active.

Do not let user mouse movement accidentally destroy the fixed top view unless intentionally designed.

---

# 48. MULTIPLE VIEW MODE

Now add:

```text
V = Toggle Four-Viewport Mode
```

When disabled:

```text
one full-screen main view
```

When enabled:

```text
four simultaneous farm views
```

---

# 49. FOUR-VIEW LAYOUT

Use:

```text
+----------------------+----------------------+
|                      |                      |
|    Perspective       |     Bird's Eye       |
|      Camera          |       / Top          |
|                      |                      |
+----------------------+----------------------+
|                      |                      |
|       Front          |        Side          |
|    Orthographic      |    Orthographic      |
|                      |                      |
+----------------------+----------------------+
```

Recommended:

### Top-left
Free perspective camera.

### Top-right
Bird’s-eye/top orthographic.

### Bottom-left
Front orthographic.

### Bottom-right
Side orthographic.

This gives the strongest technical demonstration.

---

# 50. `glViewport()` USAGE

For window:

```text
width × height
```

use:

```cpp
int halfW = width / 2;
int halfH = height / 2;
```

Example:

```cpp
glViewport(
    0,
    halfH,
    halfW,
    halfH
);
```

for top-left.

Other quadrants must use correct origins.

Remember OpenGL viewport Y starts from the lower-left.

---

# 51. FOUR VIEWPORT COORDINATES

Conceptually:

```text
Top Left:
x = 0
y = H/2

Top Right:
x = W/2
y = H/2

Bottom Left:
x = 0
y = 0

Bottom Right:
x = W/2
y = 0
```

Each:

```text
width = W/2
height = H/2
```

---

# 52. CLEAR BUFFER ONLY ONCE

For four-view mode:

Do NOT call:

```cpp
glClear(...)
```

before every viewport.

Normally clear the framebuffer once before all four views.

Then change:

```cpp
glViewport(...)
```

and render each view.

---

# 53. DEPTH BUFFER WITH MULTIPLE VIEWS

Because all four viewports share the same depth buffer:

Rendering into non-overlapping viewport rectangles is normally fine after one clear.

If artifacts appear due to scissor/state handling, investigate carefully.

Do not blindly clear color between each viewport.

---

# 54. MULTI-VIEW RENDER FUNCTION

Create something like:

```cpp
void renderFourViews()
{
    renderPerspectiveViewport();

    renderTopViewport();

    renderFrontViewport();

    renderSideViewport();
}
```

But these functions should only:

1. set viewport;
2. calculate view;
3. calculate projection;
4. send camera position;
5. call the same `renderScene()`.

Do not duplicate object drawing.

---

# 55. PER-VIEW LIGHTING

Module 4 specular lighting depends on:

```text
viewPos
```

Therefore, before rendering each viewport:

set the correct camera position.

Example:

```cpp
setupLighting(
    shader,
    perspectiveCameraPosition
);
```

then render.

For top view:

```cpp
setupLighting(
    shader,
    topCameraPosition
);
```

This prevents incorrect specular reflections.

---

# 56. DIRECTIONAL/POINT LIGHT POSITIONS MUST NOT CHANGE PER CAMERA

The farm lights remain fixed.

Only:

```text
viewPos
```

changes with each camera.

Do not rotate the sun or move farm bulbs when changing view.

---

# 57. FRONT CAMERA

Create a fixed camera facing the farm from the front.

Possible:

```cpp
frontEye =
    glm::vec3(
        0.0f,
        8.0f,
        35.0f
    );
```

looking toward:

```cpp
glm::vec3(
    0.0f,
    5.0f,
    0.0f
);
```

Tune to include full farm.

---

# 58. SIDE CAMERA

Possible:

```cpp
sideEye =
    glm::vec3(
        35.0f,
        8.0f,
        0.0f
    );
```

looking toward farm center.

Use orthographic projection.

---

# 59. TOP CAMERA

Possible:

```cpp
topEye =
    glm::vec3(
        0.0f,
        40.0f,
        0.1f
    );
```

Use a safe up vector.

Use orthographic projection.

---

# 60. ORTHOGRAPHIC VIEW BOUNDS

Make farm fit naturally.

Example:

```cpp
glm::ortho(
    -22.0f,
     22.0f,
    -22.0f,
     22.0f,
     0.1f,
     100.0f
);
```

Adjust for viewport aspect ratio.

Do not distort the farm.

---

# 61. ORTHOGRAPHIC ASPECT RATIO

Because each viewport may not be square, adjust orthographic bounds according to:

```text
viewport width / viewport height
```

Do not force a square projection if the viewport is rectangular.

---

# 62. PERSPECTIVE VIEWPORT ASPECT

For four-view perspective:

Use:

```text
half width / half height
```

not the full-window aspect ratio.

Otherwise the perspective quadrant will be distorted.

---

# 63. WINDOW RESIZE SUPPORT

Use current framebuffer size.

Do not hard-code:

```text
1200 × 800
```

for viewport calculations after the window is resized.

Update:

```cpp
framebufferWidth
framebufferHeight
```

through callback or `glfwGetFramebufferSize()`.

---

# 64. RETURN TO FULL VIEWPORT

When four-view mode is disabled:

restore:

```cpp
glViewport(
    0,
    0,
    width,
    height
);
```

Otherwise the full-screen mode may still render into one quadrant.

---

# 65. OPTIONAL VIEW BORDER

Optionally draw thin lines dividing the four viewports.

This is not necessary.

Do not add complicated UI merely for borders.

---

# 66. PHONG/GOURAUD COMPARISON

If supported by the course resources, add:

```text
P = Toggle Phong/Gouraud
```

This is a valuable final comparison feature.

If it would require destabilizing the entire project, prioritize required Module 5 curved/view features first.

---

# 67. PHONG MODE

Use the complete Module 4 fragment-based Phong shader.

This remains the default.

---

# 68. GOURAUD MODE

For Gouraud:

- calculate lighting in vertex shader;
- interpolate resulting color/intensity across the primitive;
- preserve texture usage if practical.

Do not pretend that changing material values is Gouraud shading.

The actual lighting calculation location must differ.

---

# 69. PHONG/GOURAUD VISUAL DEMONSTRATION

Good objects:

```text
milk can
sphere-based cow body/head
water tank
```

A smooth curved surface should show the difference better than a large flat wall.

---

# 70. PHONG/GOURAUD CONTROL

Use:

```text
P = Phong / Gouraud
```

Console should display current shading mode if practical.

Example:

```text
Shading Mode: PHONG
```

or:

```text
Shading Mode: GOURAUD
```

---

# 71. DO NOT BREAK LIGHT CONTROLS

Both shading modes should respect:

```text
1 = directional
2 = point
3 = spotlight
4 = day/night
5 = ambient
6 = diffuse
7 = specular
```

If Gouraud mode does not support emission identically, document the difference, but try to preserve it.

---

# 72. CUSTOM TRANSFORMATION FUNCTIONS

The supplied course assignments may require one or more custom GLM-equivalent functions.

Earlier assignment variants appear to require functions such as:

```text
custom projection
custom rotation
custom lookAt
```

For final-project safety, implement your own versions if this is consistent with the course requirements.

Suggested:

```cpp
glm::mat4 myPerspective(...);

glm::mat4 myLookAt(...);

glm::mat4 myRotate(...);
```

Do not replace all GLM operations blindly until they are tested.

---

# 73. CUSTOM PERSPECTIVE

For perspective:

\[
f
=
\frac{1}
{\tan(FOV/2)}
\]

A standard OpenGL matrix is:

\[
P =
\begin{bmatrix}
f/a & 0 & 0 & 0\\
0 & f & 0 & 0\\
0 & 0 & \frac{f+n}{n-f} & \frac{2fn}{n-f}\\
0 & 0 & -1 & 0
\end{bmatrix}
\]

where:

```text
a = aspect ratio
n = near plane
f = far plane
```

Be careful not to confuse:

```text
focal factor f
```

with:

```text
far plane
```

in code naming.

---

# 74. CUSTOM LOOKAT

Use:

\[
F =
normalize(center-eye)
\]

\[
S =
normalize(F\times up)
\]

\[
U =
S\times F
\]

Then construct the OpenGL view matrix.

Compare output against:

```cpp
glm::lookAt(...)
```

during testing.

---

# 75. CUSTOM ROTATE

Use Rodrigues' rotation formula.

For normalized axis:

\[
\mathbf{u}
=
(x,y,z)
\]

rotation matrix:

\[
R =
I\cos\theta
+
(1-\cos\theta)
uu^T
+
[u]_\times\sin\theta
\]

Use it to create:

```cpp
myRotate(
    angle,
    axis
);
```

Compare against:

```cpp
glm::rotate(...)
```

before deploying.

---

# 76. OPTIONAL TRANSFORMATION TOGGLE

If useful for demonstration:

```text
K = Use Custom/GLM Transformation
```

is possible.

But it is not necessary unless required.

Correct custom implementation matters more than adding a toggle.

---

# 77. FINAL CAMERA CONTROL PLAN

Preserve:

```text
W/A/S/D
Q/E
Mouse
Scroll
```

for free camera.

Add:

```text
B = Bird's-eye toggle
V = Four-view toggle
```

---

# 78. FINAL COMPLETE CONTROL PLAN

Recommended final controls:

```text
=====================================================
              INTERACTIVE 3D AGRO FARM
=====================================================

CAMERA
W/A/S/D : Move
Q/E     : Down / Up
Mouse   : Look Around
Scroll  : Zoom
B       : Bird's-Eye View ON/OFF
V       : Four-Viewport Mode ON/OFF

FARM
G       : Open / Close Gate

ANIMATION
F       : Fans ON/OFF
C       : Adult Cow Animation ON/OFF
R       : Calf Running ON/OFF
H       : Grazing / Feeding Animation ON/OFF
M       : Worker Animation ON/OFF

LIGHTING
1       : Directional Light ON/OFF
2       : Point Lights ON/OFF
3       : Entrance Spotlight ON/OFF
4       : Day / Night
5       : Ambient ON/OFF
6       : Diffuse ON/OFF
7       : Specular ON/OFF

SHADING
P       : Phong / Gouraud [if implemented]

ESC     : Exit
=====================================================
```

---

# 79. KEY-CONFLICT CHECK

Before assigning keys, inspect all previous controls.

Do not assign:

```text
B
V
P
```

to an existing unrelated function.

If conflict exists, choose another key and document it.

---

# 80. CURVED OBJECT TOGGLE — OPTIONAL

A debugging/demo toggle can be added:

```text
Y = Show/Hide Curve Objects
```

This is optional.

Do not add excessive controls.

---

# 81. CONTROL POINT VISUALIZATION — OPTIONAL

During development, it can be useful to draw Bézier/spline control points.

For final submission, this can optionally be toggled with:

```text
K
```

or disabled.

Do not leave large debugging spheres visible by default.

---

# 82. FINAL SCENE LAYOUT

By Module 5 the farm should conceptually contain:

```text
                        BACK

 ---------------------------------------------------------
|                                                         |
| TREE      CURVED COW SHED                FEED BARN      |
|          /-------------\                  /------\       |
|         |  FAN   FAN    |                |      |        |
|         | Cow     Cow   |                | Feed |        |
|         |_______________|                |______|        |
|                                                         |
|    HAY             MILK CAN                             |
|                     (Bezier)             WATER TANK     |
|                                            |            |
|                                            | PIPE       |
|                                            |~~~~~~~     |
|                                                         |
|             Cow Walking                                |
|                     Calf Running                       |
|        Cow Grazing                                     |
|                              Worker                    |
|                                                         |
|---------------------- ROAD -----------------------------|
|                                                         |
|          Pillar      Gate      Pillar                  |
 ---------------------------------------------------------

                         FRONT
```

This should be a complete scene, not merely a feature checklist.

---

# 83. FINAL VISUAL STORY

The project should visually communicate:

### Outside

User sees the entrance and gate.

### Entering

Gate opens.

### Farm activity

Cows walk/graze/feed.

Calves run.

Workers move.

Fans rotate.

### Environment

Detailed textured structures and vegetation.

### Day

Sunlight dominates.

### Night

Farm lighting dominates.

### Advanced geometry

Curved roof, milk can and irrigation system visibly demonstrate advanced modeling.

### Advanced viewing

User can switch between:

- free camera
- bird’s-eye
- four technical views

---

# 84. FINAL CURVE PLACEMENT MUST NOT OBSTRUCT ANIMATION

Before placing:

```text
milk can
irrigation pipe
```

inspect:

- cow walking path
- calf ellipse
- human route

Do not create new curved geometry through animation paths.

---

# 85. FINAL COLLISION REQUIREMENT

A physics-based collision system is still NOT required.

Camera/animal collisions may remain predefined/simple.

Do not use the final module to add unnecessary physics.

---

# 86. FINAL PERFORMANCE AUDIT

Check for:

- repeated VAO creation
- repeated VBO creation
- repeated texture loading
- repeated curved mesh generation
- excessive shader switching
- unnecessary uniform updates
- duplicate scene geometry

Correct obvious inefficiencies.

---

# 87. STATIC GEOMETRY GENERATION

These should be created once:

```text
cube mesh
sphere mesh
cone mesh
milk can mesh
spline pipe mesh
ruled roof mesh
```

Do not regenerate static meshes each frame.

---

# 88. DYNAMIC TRANSFORMATIONS ONLY

Objects such as:

```text
cow
calf
worker
gate
fan
```

should animate primarily by changing matrices/state.

Their base mesh should remain reusable.

---

# 89. OPENGL CLEANUP

At shutdown, delete major OpenGL resources if the project architecture supports it:

```cpp
glDeleteVertexArrays(...);
glDeleteBuffers(...);
glDeleteTextures(...);
```

This is good practice.

Do not make cleanup so complicated that it destabilizes the application.

---

# 90. FINAL SHADER COMPILE CHECK

Verify:

```text
Phong vertex shader
Phong fragment shader
Gouraud shaders if implemented
emissive shader if separate
```

all compile without warnings/errors significant to functionality.

---

# 91. FINAL TEXTURE CHECK

Verify all relative paths.

No texture path should depend on one specific computer username.

Use project-relative paths.

---

# 92. FINAL CAMERA TEST

Test:

```text
Free perspective
Bird's-eye
Front orthographic
Side orthographic
Top orthographic
Four viewports
```

Check clipping and composition.

---

# 93. FINAL LIGHTING TEST IN FOUR VIEWPORTS

Lighting must remain consistent across all views.

Expected:

```text
same light positions
same day/night state
same animation state
different viewPos
```

Do not render one viewport in day mode and another accidentally in night mode.

---

# 94. ANIMATIONS IN FOUR VIEWPORTS

Animations should remain synchronized because the same scene state is rendered four times in one frame.

Important:

Do NOT update animation inside:

```cpp
renderScene()
```

Otherwise the animals may advance four times per frame in four-view mode.

Animation must be updated once BEFORE rendering all viewports.

---

# 95. CRITICAL MULTI-VIEW RULE

Correct:

```cpp
updateAnimations(deltaTime);

renderView1();
renderView2();
renderView3();
renderView4();
```

Incorrect:

```cpp
renderView1()
{
    updateAnimations();
}

renderView2()
{
    updateAnimations();
}
```

This would make animation speed depend on the number of viewports.

---

# 96. SAME RULE FOR GATE/FANS

Gate, cows, calves, humans and fans must update once per frame.

Drawing them multiple times should not change state.

---

# 97. FINAL FRAME STRUCTURE

A good final frame architecture is:

```cpp
while (!glfwWindowShouldClose(window))
{
    updateDeltaTime();

    processInput(window);

    updateAnimations(deltaTime);

    updateLightingState();

    glClear(...);

    if (fourViewMode)
    {
        renderFourViews();
    }
    else
    {
        renderSingleView();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

This is the recommended high-level design.

---

# 98. `renderSingleView()`

Conceptually:

```cpp
void renderSingleView()
{
    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );

    if (birdEyeMode)
    {
        renderBirdEyeView();
    }
    else
    {
        renderFreeCameraView();
    }
}
```

---

# 99. `renderOneView()`

Create a reusable helper:

```cpp
void renderOneView(
    int x,
    int y,
    int width,
    int height,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& eyePosition
);
```

Inside:

1. set viewport;
2. activate shader;
3. set view/projection;
4. setup lights with `eyePosition`;
5. call `renderScene()`.

This will reduce duplication.

---

# 100. PHONG/GOURAUD WITH MULTIPLE VIEWS

If shading toggle exists:

all four viewports should use the selected shading mode.

Do not render:

```text
Phong in one
Gouraud in another
```

unless you intentionally design a dedicated comparison mode.

A global toggle is simpler.

---

# 101. OPTIONAL SIDE-BY-SIDE SHADING COMPARISON

As an optional extra:

Two viewports could compare:

```text
Phong
vs
Gouraud
```

but only implement this if required.

The standard four-view camera layout is more important.

---

# 102. FINAL MATERIAL AUDIT

Check:

```text
grass
soil
wood
brick
metal
cow
human clothing
hay
milk can
irrigation pipe
ruled roof
```

have appropriate material parameters.

Do not let new curved objects use default shiny plastic values.

---

# 103. FINAL NORMAL AUDIT

Check curved geometry for:

- inverted normals
- black patches
- sharp unintended seams
- broken specular highlights

If entire inside/outside appears reversed, inspect triangle winding.

---

# 104. OPTIONAL DOUBLE-SIDED ROOF

If the cow-shed roof needs visibility from below and face culling is enabled:

either:

- model both surfaces;
- disable culling for that object;
- use correct double-sided rendering strategy.

Do not let the roof disappear when viewed from inside the shed.

---

# 105. FINAL PRESENTATION MODE

Optionally add:

```text
0 = Reset Camera
```

A reset key is useful.

It could restore:

```text
camera outside entrance
day mode
single perspective view
```

Do not make this mandatory if key conflicts exist.

---

# 106. RECOMMENDED RESET STATE

Possible:

```text
camera:
outside gate

gate:
closed

day mode:
ON

four-view:
OFF

bird-eye:
OFF

animations:
ON

lights:
default

shading:
Phong
```

This is useful before presenting.

---

# 107. OPTIONAL CAMERA PRESETS

If stable, provide:

```text
8 = Entrance View
9 = Farm Interior View
```

but this is not required.

Do not overload controls.

---

# 108. FINAL STARTUP CONSOLE

Print:

```text
========================================================
           INTERACTIVE 3D AGRO FARM
                  FINAL PROJECT
========================================================

CAMERA
W/A/S/D : Move
Q/E     : Down / Up
Mouse   : Look Around
Scroll  : Zoom
B       : Bird's-Eye View
V       : Four-Viewport Mode

FARM
G       : Open / Close Gate

ANIMATION
F       : Fans ON/OFF
C       : Adult Cow Animation ON/OFF
R       : Calf Running ON/OFF
H       : Grazing / Feeding ON/OFF
M       : Worker Animation ON/OFF

LIGHTING
1       : Directional Light ON/OFF
2       : Point Lights ON/OFF
3       : Entrance Spotlight ON/OFF
4       : Day / Night
5       : Ambient ON/OFF
6       : Diffuse ON/OFF
7       : Specular ON/OFF

SHADING
P       : Phong / Gouraud

ESC     : Exit
========================================================
```

Only print `P` if the Gouraud toggle is actually implemented.

---

# 109. FINAL DEMONSTRATION SEQUENCE

The finished project should be demonstrated in a deliberate order.

## Part 1 — Entrance

Launch application.

Camera starts outside gate.

Show:

- textures
- fence
- sign
- entrance
- farm visible in distance

Press:

```text
G
```

Gate opens smoothly.

---

## Part 2 — Enter farm

Move through gate.

Show:

- grass
- road
- buildings
- trees
- props

---

## Part 3 — Animals

Show:

```text
walking cow
grazing cow
feeding cow
idle cow
running calf
```

Explain hierarchical transformations.

---

## Part 4 — Humans/Fans

Show:

- worker movement
- arm/leg movement
- fan rotation

Toggle fan using:

```text
F
```

---

## Part 5 — Curved Geometry

Move near cow shed.

Show:

### Ruled-surface roof

Explain:

\[
S(u,v)
=
(1-v)C_1(u)+vC_2(u)
\]

Then show:

### Bézier milk can

Explain:

- control points
- cubic Bézier profile
- surface of revolution

Then show:

### Spline irrigation pipe

Explain:

- B-spline control points
- generated path/tube

---

## Part 6 — Lighting

Show day mode.

Toggle:

```text
1
5
6
7
```

Explain directional, ambient, diffuse and specular.

---

## Part 7 — Night Mode

Press:

```text
4
```

Show:

- dark sky
- point lights
- emissive bulbs
- entrance spotlight

Toggle:

```text
2
3
```

---

## Part 8 — Bird's-Eye

Press:

```text
B
```

Show entire farm layout.

---

## Part 9 — Four Viewports

Press:

```text
V
```

Show simultaneously:

- perspective
- top
- front
- side

Explain perspective vs orthographic projection.

---

## Part 10 — Shading Comparison

If implemented:

Press:

```text
P
```

Compare Phong and Gouraud on:

- milk can
- cow
- water tank

---

# 110. FINAL VIVA CONCEPTS

The final project should allow me to explain:

### VAO

Stores vertex attribute configuration.

### VBO

Stores vertex data.

### EBO

Stores indexed vertex references.

### Model matrix

Moves object from local space to world space.

### View matrix

Transforms world relative to camera.

### Projection matrix

Maps camera coordinates into clip space.

### Perspective

Distant objects appear smaller.

### Orthographic

Object size does not depend on depth.

### Hierarchical modeling

Child transformations inherit parent matrices.

### Bézier curve

Controlled polynomial curve using control points.

### B-spline

Piecewise smooth curve with local control.

### Ruled surface

Surface interpolated between two boundary curves.

### Directional light

Position-independent parallel rays.

### Point light

Radiates from position and attenuates with distance.

### Spotlight

Point-like source limited by direction/cutoff.

### Ambient

Base illumination.

### Diffuse

Depends on angle between light and normal.

### Specular

View-dependent highlight.

### Phong shading

Lighting per fragment.

### Gouraud shading

Lighting per vertex.

---

# 111. FINAL ACCEPTANCE CHECKLIST — EXISTING FEATURES

Do not consider the project complete unless all previous features still work:

- Free camera.
- Mouse look.
- Zoom.
- Gate.
- Textures.
- Farm environment.
- Cow shed.
- Barn.
- Troughs.
- Trees.
- Hay.
- Props.
- Cows.
- Calves.
- Humans.
- Fans.
- Animal animation.
- Human animation.
- Day/night.
- Directional light.
- Point lights.
- Spotlight.
- Ambient.
- Diffuse.
- Specular.
- Emissive bulbs.

---

# 112. FINAL ACCEPTANCE CHECKLIST — MODULE 5 FEATURES

Module 5 must additionally include:

- Bézier-based object.
- Milk can visibly appears.
- Surface of revolution works.
- Bézier object has normals.
- Bézier object receives lighting.
- Bézier object uses appropriate material/texture.
- Spline object exists.
- Irrigation pipe visually follows spline.
- Spline geometry receives lighting.
- Ruled surface exists.
- Cow-shed roof uses ruled-surface geometry.
- Old overlapping roof is removed.
- Curved roof has correct normals.
- Curved roof has texture.
- Bird’s-eye mode works.
- Bird’s-eye view covers farm.
- Orthographic projection is used in technical views.
- Front view works.
- Side view works.
- Top view works.
- Four-view mode works.
- Perspective viewport has correct aspect ratio.
- Orthographic views are not distorted.
- Window resize is handled.
- Animations update only once per frame.
- Four-view rendering does not accelerate animation.
- Light positions remain consistent between views.
- View position changes correctly for specular lighting.
- Full-screen viewport restores correctly.
- Phong/Gouraud toggle works if implemented.
- No static curved geometry is regenerated every frame.
- Final project remains stable.

---

# 113. FINAL CURVE TEST

Inspect the Bézier milk can closely.

Check:

- smooth profile
- no missing triangles
- no seam gap at 0°/360°
- correct normals
- no obvious twisted triangles
- texture orientation acceptable
- specular highlights smooth

---

# 114. SPLINE TEST

Inspect irrigation pipe.

Check:

- curve is smooth
- tube/segments follow path
- no sudden orientation flips
- pipe does not pass through buildings
- texture/material works
- lighting works

---

# 115. RULED SURFACE TEST

Inspect roof from:

- outside
- underneath
- front
- side
- bird's-eye

Check:

- roof spans correct shed width/depth
- no severe gaps
- smooth curve
- proper triangle orientation
- texture remains visible

---

# 116. FOUR-VIEW ANIMATION TEST

Enable:

```text
V
```

Watch one running calf.

Its position should be identical in all four views at the same moment.

If each viewport shows a slightly different animation time, animation is being updated during rendering and must be corrected.

---

# 117. FOUR-VIEW LIGHT TEST

Switch to night while four-view mode is active.

All views must show:

- same bulb states
- same spotlight state
- same day/night state

Only camera-dependent specular appearance may differ.

---

# 118. VIEWPORT RESIZE TEST

Resize the application window.

Confirm:

- quadrants remain aligned
- no large empty area
- perspective aspect remains correct
- technical views remain readable

---

# 119. FINAL PERFORMANCE TARGET

The project should remain interactively smooth on the course/lab computer.

Prioritize:

- reusable geometry
- reasonable curve resolution
- limited point-light count
- no per-frame mesh generation

Do not chase extremely high polygon counts.

---

# 120. CURVE RESOLUTION TUNING

If performance is low:

reduce:

```text
Bezier profile samples
angular revolution segments
spline samples
pipe radial segments
ruled surface samples
```

before removing features.

The visual difference between 32 and 128 angular segments may be small at project scale.

---

# 121. FINAL FILE ORGANIZATION

A reasonable final structure could be:

```text
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
├── curves/
│   ├── bezierSurface.h
│   ├── spline.h
│   └── ruledSurface.h
│
├── models/
│   ├── cow.h
│   ├── human.h
│   ├── fan.h
│   └── farmCurvedObjects.h
│
├── scene/
│   ├── farmEnvironment.h
│   ├── farmBuildings.h
│   └── farmProps.h
│
├── shaders/
│   ├── phongTexture.vs
│   ├── phongTexture.fs
│   ├── gouraudTexture.vs
│   ├── gouraudTexture.fs
│   ├── emissive.vs
│   └── emissive.fs
│
└── textures/
    ├── grass.jpg
    ├── soil.jpg
    ├── wood.jpg
    ├── brick.jpg
    ├── roof.jpg
    ├── metal.jpg
    ├── concrete.jpg
    ├── hay.jpg
    ├── bark.jpg
    ├── leaves.jpg
    └── ...
```

Adapt to current Module 4 organization.

Do not restructure working files merely to match this example.

---

# 122. FINAL CODE QUALITY

The final code must:

- compile without missing references;
- avoid pseudocode in required implementation;
- use modular functions;
- keep update and rendering separate;
- avoid giant monolithic `main.cpp`;
- avoid duplicated farm rendering;
- use consistent naming;
- use correct local/world transformations;
- use correct normals;
- preserve texture paths;
- maintain clean camera logic;
- maintain clean lighting setup;
- maintain clean animation state;
- initialize all OpenGL resources correctly.

---

# 123. FINAL `main.cpp` RESPONSIBILITY

`main.cpp` should primarily control:

- window initialization
- input
- deltaTime
- animation updating
- view mode
- lighting mode
- clearing
- rendering
- swap/poll

It should not contain the complete detailed geometry for:

- cow
- human
- milk can
- roof
- spline pipe

if those can reasonably be modularized.

---

# 124. FINAL `renderScene()` REQUIREMENT

`renderScene()` should not:

- update animations;
- change game states;
- modify gate angle;
- move cows;
- advance spline samples;
- modify camera;
- swap buffers.

It should only render the current state.

This is critical for four-view correctness.

---

# 125. FINAL `updateAnimations()` REQUIREMENT

It should be called once per frame.

Conceptually:

```cpp
void updateAnimations(float dt)
{
    updateGate(dt);
    updateCows(dt);
    updateCalves(dt);
    updateWorkers(dt);
    updateFans(dt);
}
```

Static curved geometry should not be updated here.

---

# 126. FINAL VIEW STATE

Use clean state such as:

```cpp
bool fourViewMode = false;
bool birdEyeMode = false;
```

or enums.

Do not create contradictory states such as:

```text
frontView = true
sideView = true
birdEye = true
```

unless intentional.

---

# 127. OPTIONAL CAMERA ENUM

A cleaner approach:

```cpp
enum class MainViewMode
{
    Free,
    BirdEye
};
```

and:

```cpp
bool multiViewport = false;
```

When `multiViewport == true`, four-view mode overrides single-view selection.

---

# 128. FINAL SCENE RESET — OPTIONAL BUT RECOMMENDED

If adding a reset key, reset:

```text
camera
gate
time of day
lighting toggles
animation toggles
viewport mode
shading mode
```

to a stable presentation state.

Do not reset object geometry.

---

# 129. FINAL PROJECT RESULT

When complete, the project should feel like one coherent graphical application.

Not:

```text
Lab 1 demo
+
Lab 2 demo
+
Lab 3 demo
+
Lab 4 demo
+
Lab 5 demo
```

Instead:

```text
ONE AGRO FARM
```

where each course concept contributes naturally to the same environment.

---

# 130. EXPECTED FINAL EXPERIENCE

The final user experience should be:

1. Program opens outside a textured farm entrance.
2. User presses `G`.
3. Gate opens.
4. User walks inside.
5. Cows and calves are already behaving independently.
6. Workers move.
7. Fans rotate.
8. Farm environment contains textured structures and vegetation.
9. User sees a curved cow-shed roof.
10. User sees a Bézier-generated milk can.
11. User sees a spline irrigation pipe.
12. User can switch day/night.
13. Artificial lights illuminate the farm at night.
14. User can isolate ambient/diffuse/specular components.
15. User can enter bird’s-eye view.
16. User can activate four viewports.
17. The same animated scene appears consistently in every viewport.
18. Optional Phong/Gouraud toggle demonstrates shading differences.

---

# 131. WHAT I WANT FROM YOU

Now implement **MODULE 5 only**, continuing from the completed Module 4 project.

Before coding:

1. inspect the current Module 4 project;
2. inspect supplied Lab 5 Bézier code;
3. inspect supplied spline/curve material;
4. inspect ruled-surface examples/instructions;
5. inspect current cow-shed roof implementation;
6. inspect the central `renderScene()`;
7. inspect current lighting setup;
8. inspect camera architecture;
9. inspect all animation update locations;
10. confirm animation is not currently updated inside rendering functions.

Then implement Module 5 systematically.

---

# 132. IMPLEMENTATION OUTPUT REQUIREMENT

Do not provide only theory or snippets.

Provide the complete compile-ready changes necessary to transform Module 4 into the final Module 5 project.

For every new or modified file:

- state filename clearly;
- provide complete required implementation;
- ensure declarations/definitions match;
- ensure shader uniforms match CPU code;
- ensure generated curve meshes use the correct vertex format.

Do not leave required work as:

```text
// TODO
// add curve here
// same code as before
// implement multi-view yourself
```

---

# 133. FINAL RESPONSE REQUIREMENTS

After implementation provide:

## A. Files changed

List all added/modified:

- `.cpp`
- `.h`
- shader files

## B. Bézier implementation

Explain:

- chosen farm object
- control points
- sampling
- surface generation
- normals
- indices
- material

## C. Spline implementation

Explain:

- control points
- curve equation
- pipe generation
- tangent/orientation handling

## D. Ruled surface implementation

Explain:

- two boundary curves
- \(S(u,v)\)
- mesh generation
- roof replacement
- normal generation

## E. Camera architecture

Explain:

- free camera
- bird’s-eye camera
- front camera
- side camera
- top camera

## F. Projection architecture

State which views use:

- perspective
- orthographic

## G. Four-view architecture

Explain:

- viewport coordinates
- aspect ratios
- single `renderScene()`
- one animation update per frame

## H. Shading modes

If implemented, explain:

- Phong
- Gouraud
- toggle key

## I. Final controls

Provide the complete keyboard/mouse map.

## J. Final file structure

Show the complete final source structure.

## K. Testing checklist

Give a detailed ordered test procedure.

## L. Final demonstration sequence

Give the exact sequence I should follow when presenting the project.

## M. Viva preparation

List the major graphics concepts demonstrated by each farm feature.

---

# 134. FINAL PRIORITY

The final Module 5 implementation must demonstrate:

**Bézier curves/surfaces + surface of revolution + spline geometry + ruled surfaces + perspective projection + orthographic projection + bird’s-eye camera + multiple viewports + reusable scene rendering + correct multi-camera lighting + stable integration with textures, animation and Phong lighting.**

Most importantly:

> Do not treat Module 5 as a separate mathematical demonstration. All advanced geometry and viewing techniques must be integrated naturally into the already completed Interactive 3D Agro Farm.

The final result should look and behave like one finished graphics project rather than five separate laboratory exercises.