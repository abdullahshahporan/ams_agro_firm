I am continuing my **CSE 4208 Computer Graphics project in C++ using Modern OpenGL**.

The final project is:

# Interactive 3D Agro Farm Simulation

Modules 1 and 2 have already been completed.

---

# 1. EXISTING PROJECT STATE

## Module 1 already contains

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
- farm boundary
- entrance pillars
- double farm gate
- smooth hierarchical gate animation
- reusable cube geometry
- modular `renderScene()`
- `deltaTime`
- keyboard/mouse controls

## Module 2 already contains

- textured grass
- textured soil road
- textured entrance
- textured fence
- detailed cow shed
- detailed feed barn
- feeding troughs
- hay bales
- trees
- water/silo structure
- crates/feed sacks
- static fan geometry
- lamp fixtures
- texture loading using `stb_image`
- reusable textured primitives
- scene layout prepared for animals

Now implement:

# MODULE 3 — ANIMALS, HUMANS, HIERARCHICAL MODELING, AND FARM ANIMATION

Do not start a new project.

Extend the completed Module 2 project directly.

---

# 2. PRIMARY GOAL OF MODULE 3

The farm must become visibly alive.

After Module 3, the user should be able to enter the farm and see:

- several adult cows
- cows walking naturally
- cows grazing/eating
- cows moving their heads
- cows swinging their tails
- articulated cow legs
- calves
- calves running around the grazing field
- one or two farm workers
- walking worker animation
- arm/leg movement
- rotating ceiling fans
- different animals performing different behaviors simultaneously

The scene must not look like all animals are synchronized clones.

---

# 3. IMPORTANT COURSE CONCEPT

Module 3 must strongly demonstrate:

# Hierarchical Modeling and Transformation

Complex objects must be assembled from smaller primitives.

Examples:

```text id="z2w8cx"
Cow
├── body
├── neck
├── head
│   ├── snout
│   ├── ears
│   ├── eyes
│   └── horns
├── front left leg
├── front right leg
├── back left leg
├── back right leg
└── tail
```

and:

```text id="1xom64"
Human
├── torso
├── head
├── left arm
├── right arm
├── left leg
└── right leg
```

Every child component must inherit its parent's transformation.

Do not position articulated body parts independently in world coordinates.

---

# 4. DO NOT USE DOWNLOADED 3D MODELS

Do not use:

- OBJ cow model
- FBX model
- Blender cow
- Assimp
- Mixamo
- animated external characters

The purpose of this module is to demonstrate OpenGL transformations and hierarchical modeling.

Build the animals and humans using the primitives available in the course project.

Use combinations of:

- cube/cuboid
- sphere
- cone
- simple cylinder if already available

Do not introduce a complicated mesh-loading system.

---

# 5. MODULE 3 IMPLEMENTATION ORDER

Implement in this order:

## Phase 3.1
Animation architecture and states

## Phase 3.2
One complete static adult cow

## Phase 3.3
Hierarchical cow joints

## Phase 3.4
Cow walking animation

## Phase 3.5
Cow grazing/eating animation

## Phase 3.6
Cow tail/head idle animation

## Phase 3.7
Multiple cows with different behavior states

## Phase 3.8
Calf model

## Phase 3.9
Calf running/path animation

## Phase 3.10
Human model

## Phase 3.11
Human walking/working animation

## Phase 3.12
Fan animation

## Phase 3.13
Animation controls

## Phase 3.14
Final scene balancing and testing

Do not attempt every animation at once.

---

# 6. ANIMATION ARCHITECTURE

Keep animation updating separate from rendering.

The project should conceptually have:

```cpp id="5mcaxn"
void updateAnimations(float deltaTime)
{
    updateGate(deltaTime);

    updateCows(deltaTime);

    updateCalves(deltaTime);

    updateHumans(deltaTime);

    updateFans(deltaTime);
}
```

Rendering functions should mainly read the current animation state.

Do not make movement depend directly on how often an object is drawn.

---

# 7. DELTA TIME REQUIREMENT

All animation must remain frame-rate independent.

Correct:

```cpp id="79ri0d"
cow.position += direction * speed * deltaTime;
```

Incorrect:

```cpp id="f1m0ha"
cow.position.x += 0.01f;
```

Use `deltaTime` for:

- cow translation
- calf movement
- human movement
- gate
- fan rotation
- state timers where appropriate

Oscillating joint animations may use time-based sine functions.

---

# 8. ANIMATION STATES

Do not use dozens of unrelated global booleans for each body part.

Create meaningful animation states.

For example:

```cpp id="1627c4"
enum class CowState
{
    Standing,
    Walking,
    Grazing,
    Feeding
};
```

Possible animal structure:

```cpp id="c0vp8x"
struct Cow
{
    glm::vec3 position;

    float yaw;
    float scale;

    CowState state;

    float speed;

    float animationTime;

    float pathDistance;

    bool active;
};
```

You may improve the structure if needed.

The important point is that each cow can behave independently.

---

# 9. ADULT COW MODEL

First build ONE good adult cow.

Do not immediately duplicate an unfinished model.

The cow should visibly contain:

- torso/body
- neck
- head
- snout
- ears
- two horns
- eyes
- four legs
- hooves
- tail
- optional udder

The proportions should resemble a cow even when textures are ignored.

---

# 10. COW BODY

Use a large scaled primitive.

Recommended options:

### Option A
Scaled sphere/ellipsoid

### Option B
Rounded-looking combination of sphere/cuboid

### Option C
Scaled cuboid if sphere integration is difficult

Prefer a sphere/ellipsoid if the supplied sphere primitive works correctly with textures.

Example approximate body scale:

```text id="25iqmn"
length ≈ 2.6
height ≈ 1.3
width ≈ 1.1
```

Exact values may be adjusted.

---

# 11. COW ORIENTATION

Choose one consistent local forward direction.

Recommended:

```text id="i04qm8"
Cow local forward = +X
```

or:

```text id="mvff6p"
Cow local forward = -Z
```

Choose one and use it consistently.

Do not construct some body parts assuming +X while movement code assumes -Z.

Document the convention in code.

---

# 12. COW PARENT MATRIX

All body parts must inherit the complete cow transformation.

Conceptually:

```cpp id="6ko7c4"
glm::mat4 cowModel(1.0f);

cowModel =
    glm::translate(
        cowModel,
        cow.position
    );

cowModel =
    glm::rotate(
        cowModel,
        glm::radians(cow.yaw),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

cowModel =
    glm::scale(
        cowModel,
        glm::vec3(cow.scale)
    );
```

All cow parts are then drawn relative to this matrix.

Do not add the world position individually to every leg/head component.

---

# 13. COW BODY HIERARCHY

Use an approximate hierarchy such as:

```text id="cn1ssj"
CowRoot
│
├── Torso
│
├── NeckPivot
│   └── Neck
│       └── HeadPivot
│           ├── Head
│           ├── Snout
│           ├── EyeL
│           ├── EyeR
│           ├── EarL
│           ├── EarR
│           ├── HornL
│           └── HornR
│
├── FrontLeftLegPivot
│   └── FrontLeftLeg
│
├── FrontRightLegPivot
│   └── FrontRightLeg
│
├── RearLeftLegPivot
│   └── RearLeftLeg
│
├── RearRightLegPivot
│   └── RearRightLeg
│
└── TailPivot
    └── Tail
```

This hierarchy should be reflected in the model matrices.

---

# 14. COW HEAD PIVOT

The head must rotate from the neck region.

Do NOT rotate the head around its geometric center during grazing.

Correct conceptual order:

```text id="7qdy3n"
Cow root
↓
translate to neck/head joint
↓
rotate head downward
↓
translate local head geometry
↓
draw
```

This allows realistic grazing.

---

# 15. COW LEGS

Use four articulated leg objects.

A simple one-piece leg is acceptable for Module 3, but each leg must rotate from the upper joint.

Better version:

```text id="i304ec"
Upper Leg
   ↓
Lower Leg
   ↓
Hoof
```

However, if a two-segment leg creates unnecessary complexity, use:

```text id="slivk9"
Upper-leg pivot
   ↓
long leg cuboid
   ↓
hoof
```

The hierarchical pivot is more important than anatomical detail.

---

# 16. LEG PIVOT

Do not rotate a leg around its center.

For example:

```cpp id="dozmqu"
glm::mat4 legParent = cowRoot;

legParent =
    glm::translate(
        legParent,
        hipPosition
    );

legParent =
    glm::rotate(
        legParent,
        glm::radians(legAngle),
        legRotationAxis
    );
```

Then draw leg geometry below the pivot.

---

# 17. WALKING GAIT

Use a cyclic sine-based gait.

For example:

```cpp id="wmig7r"
float legSwing =
    sin(cow.animationTime * walkFrequency)
    * maxLegAngle;
```

Recommended:

```text id="ozv18b"
walkFrequency ≈ 4–6
maxLegAngle ≈ 20°–30°
```

Then alternate diagonal/opposite legs.

Example:

```text id="ibk90j"
frontLeft  = +legSwing
frontRight = -legSwing
rearLeft   = -legSwing
rearRight  = +legSwing
```

The exact gait can be adjusted visually.

The goal is to avoid all four legs swinging in the same direction.

---

# 18. BODY BOB WHILE WALKING

Optionally add a very small vertical body motion:

```cpp id="zft02e"
float bodyBob =
    abs(sin(cow.animationTime * walkFrequency))
    * 0.03f;
```

Keep it subtle.

Do not make the cow bounce like a cartoon.

---

# 19. COW WALKING TRANSLATION

Walking cows must actually move through the environment.

Do not animate legs while leaving the cow permanently fixed.

Recommended simple approach:

Use predefined line segments or paths.

For example:

```text id="4iafci"
Point A ------------ Point B
```

Cow moves A → B.

When it reaches B:

- reverse direction
or
- rotate and follow another point

---

# 20. SIMPLE COW PATROL PATH

A robust implementation is:

```cpp id="mxdpr0"
glm::vec3 pointA;
glm::vec3 pointB;

float t;
bool forward;
```

or directly move toward a target.

Example:

```cpp id="y0oyc4"
glm::vec3 direction =
    glm::normalize(target - cow.position);

cow.position +=
    direction *
    cow.speed *
    deltaTime;
```

When close to target:

```cpp id="phhnr3"
target = otherPoint;
```

Also update cow yaw so it faces movement direction.

---

# 21. COW YAW FROM DIRECTION

For horizontal movement, determine yaw from direction.

Depending on local forward convention, use something conceptually like:

```cpp id="ea4qjt"
cow.yaw =
    glm::degrees(
        atan2(direction.z, direction.x)
    );
```

Adjust offsets according to the actual cow model orientation.

The animal must visually face the direction in which it moves.

---

# 22. COW GRAZING

At least one adult cow should graze from the ground.

Grazing animation should include:

- head lowering
- slight periodic head movement
- stationary or almost stationary body

Example:

```cpp id="e94ef3"
float grazingAngle =
    55.0f +
    sin(cow.animationTime * 2.0f) * 5.0f;
```

Rotate the head downward from the neck joint.

Depending on orientation, the angle sign may need adjustment.

---

# 23. FEEDING COW

Place another cow beside the Module 2 feeding trough.

Its behavior can be similar to grazing but with its head directed into the trough.

Keep its body stationary.

Use a slightly different head angle from grass grazing.

This makes different behaviors visually distinguishable.

---

# 24. STANDING COW

One adult cow should remain mostly stationary.

It should still have small idle animations:

- tail swing
- slight head movement
- optional ear movement

Example:

```cpp id="fosz1l"
float idleHead =
    sin(time * 0.8f) * 4.0f;
```

This prevents the animal from looking frozen.

---

# 25. TAIL ANIMATION

Tail must rotate around its root.

Create:

```text id="cqyhjn"
TailPivot
   ↓
tail segment
```

Use:

```cpp id="tp1h5y"
float tailAngle =
    sin(cow.animationTime * 3.0f)
    * 20.0f;
```

Do not swing it unrealistically through the body.

---

# 26. TAIL TIP

Optionally add:

- small sphere
or
- slightly larger primitive

at the tail end.

This helps the tail read visually.

---

# 27. COW EARS

Use:

- small flattened cubes
- small triangles
- pyramids

depending on available primitives.

Place them symmetrically.

Ear animation is optional.

Do not overcomplicate.

---

# 28. COW HORNS

Use cone primitives if already available.

Two horns should be attached to the head.

If no cone primitive currently exists:

create a simple reusable cone mesh compatible with:

- model transformation
- texture coordinates if practical
- normals for Module 4

Do not use externally loaded geometry.

---

# 29. COW EYES

Use very small spheres/cubes.

Do not make eyes oversized.

Use dark color or simple texture.

---

# 30. COW TEXTURE

Module 2 introduced texture support.

Use a suitable cow-like texture or color pattern.

Possible strategy:

### Option A
White body with black textured/dark patches.

### Option B
Brown cow texture.

### Option C
Different body colors for different animals.

Do not require a highly realistic UV-unwrapped cow skin.

Since the cow is built from primitives, simple per-component textures/colors are acceptable.

---

# 31. VARIATION BETWEEN COWS

Do not make every cow visually identical.

Use small variations in:

- overall scale
- body color
- orientation
- horn size
- location
- animation state
- speed

For example:

```text id="c9l6jd"
Cow 1:
black/white
walking

Cow 2:
brown
grazing

Cow 3:
black/white
feeding

Cow 4:
brown/white
standing under shed
```

---

# 32. RECOMMENDED ADULT COW COUNT

Use approximately:

```text id="gupk3l"
3–4 adult cows
```

Recommended behaviors:

```text id="lvpegb"
Cow 1 = walking

Cow 2 = grazing

Cow 3 = feeding at trough

Cow 4 = standing/idle under shed
```

This gives strong visual variety without excessive rendering complexity.

---

# 33. COW FUNCTION DESIGN

Create a reusable function.

Conceptually:

```cpp id="ejhujn"
void drawCow(
    Shader& shader,
    const Cow& cow
);
```

Inside it:

- construct root matrix
- calculate animation joint angles
- draw all body components

Do not copy/paste an entire cow model four times.

---

# 34. SEPARATE UPDATE FROM DRAW

Prefer:

```cpp id="uozzwg"
void updateCow(
    Cow& cow,
    float deltaTime
);
```

and:

```cpp id="9785r8"
void drawCow(
    Shader& shader,
    const Cow& cow
);
```

Do not change the cow's world position unpredictably inside `drawCow()`.

---

# 35. CALF MODEL

Reuse the cow architecture.

Do not create a completely unrelated model.

The calf should visually resemble a smaller, younger cow.

Possible differences:

```text id="up534p"
overall scale = 0.50–0.65 adult

slightly larger head relative to body

shorter legs

smaller horns or no horns
```

The calf can reuse the same basic hierarchy.

---

# 36. CALF STRUCTURE

Possible:

```cpp id="z2cvjq"
struct Calf
{
    glm::vec3 position;

    float yaw;

    float scale;

    float speed;

    float animationTime;

    float pathAngle;

    bool running;
};
```

Or reuse a general animal class/structure if architecture remains simple.

Do not over-engineer inheritance systems unnecessarily.

---

# 37. CALF RUNNING PATH

Use a smooth, predefined path inside the grazing field.

A simple elliptical path is ideal.

Example:

```cpp id="l8mb40"
calf.pathAngle +=
    calf.speed *
    deltaTime;
```

Then:

```cpp id="96mjlj"
calf.position.x =
    centerX +
    radiusX * cos(calf.pathAngle);

calf.position.z =
    centerZ +
    radiusZ * sin(calf.pathAngle);
```

This produces continuous movement.

---

# 38. CALF FACING DIRECTION

Do not let the calf slide sideways around the ellipse.

Calculate the tangent direction.

For:

```text id="8exhri"
x = cx + rx cos(t)
z = cz + rz sin(t)
```

the tangent is proportional to:

```text id="mbge7h"
dx/dt = -rx sin(t)

dz/dt =  rz cos(t)
```

Use this tangent to calculate yaw.

The calf should face along the running direction.

---

# 39. CALF RUNNING LEGS

Running must use faster leg movement than adult cow walking.

Example:

```cpp id="j2pmxr"
float runSwing =
    sin(calf.animationTime * 10.0f)
    * 35.0f;
```

Recommended:

```text id="0kd6oe"
frequency = 8–12

angle = 25°–40°
```

Tune visually.

---

# 40. CALF BODY MOTION

Add slightly stronger but still reasonable body bob:

```cpp id="dv6adp"
float runBob =
    abs(sin(calf.animationTime * 10.0f))
    * 0.06f;
```

Keep the calf feet close to the ground.

Do not make it appear to fly.

---

# 41. NUMBER OF CALVES

Use:

```text id="vm97sk"
1–2 calves
```

Recommended:

```text id="hrvs3s"
Calf 1:
larger ellipse

Calf 2:
smaller ellipse / opposite direction
```

Do not make their paths intersect excessively.

---

# 42. CALF PATH LOCATION

Use the open grazing field prepared in Module 2.

Make sure paths do not pass:

- through barn
- through shed
- through troughs
- through trees
- through water tank
- through fences

No physics collision engine is required.

Just design safe paths.

---

# 43. HUMAN MODEL

Add:

```text id="mfpb4i"
1–2 farm workers
```

Use a stylized low-poly human constructed from primitives.

Do not use external character models.

---

# 44. HUMAN BODY PARTS

Construct:

```text id="qbnr1h"
Head
Torso
Left Arm
Right Arm
Left Leg
Right Leg
Shoes
Optional Hat
```

Recommended primitives:

| Part | Primitive |
|---|---|
| Head | sphere |
| Torso | cuboid |
| Upper arms | cuboid |
| Legs | cuboid |
| Shoes | cuboid |
| Hat | flattened cylinder/cone/cubes |

The model only needs to clearly read as a human.

---

# 45. HUMAN HIERARCHY

Use:

```text id="3c6ole"
HumanRoot
│
├── Torso
├── Head
├── LeftArmPivot
│   └── LeftArm
├── RightArmPivot
│   └── RightArm
├── LeftLegPivot
│   └── LeftLeg
└── RightLegPivot
    └── RightLeg
```

Each limb rotates from the correct shoulder/hip pivot.

---

# 46. HUMAN PROPORTIONS

Use approximately believable relative proportions.

Example:

```text id="ja72gu"
total height ≈ 1.7–2.0 world units

head ≈ 0.3

torso ≈ 0.7–0.8

legs ≈ 0.8
```

Adjust relative to your cow scale.

A human should not be taller than the cow shed roof.

---

# 47. HUMAN WALKING ANIMATION

Use opposite arm/leg motion.

Example:

```cpp id="hyjhgc"
float walk =
    sin(worker.animationTime * 5.0f)
    * 25.0f;
```

Then:

```text id="epirbm"
left leg  = +walk
right leg = -walk

left arm  = -walk
right arm = +walk
```

This gives recognizable walking movement.

---

# 48. WORKER 1 — WALKING

Worker 1 should walk along a short path.

Good routes:

```text id="isabg1"
barn → feeding area
```

or:

```text id="9hiqxr"
shed → hay area
```

Use a simple two-way path.

When reaching one end:

- turn around
- continue walking back

Update worker yaw accordingly.

---

# 49. WORKER 2 — FARM ACTIVITY

Worker 2 can remain near:

- feeding trough
- cow shed
- hay
- barn

Possible simple activity:

- slight arm movement
- carrying bucket
- standing beside feeding cow

Do not create overly complex interactions.

The purpose is to show a second distinct animated human behavior.

---

# 50. OPTIONAL BUCKET

Create a simple bucket from:

- cuboid/cylinder-like body
- simple handle if practical

Attach it to worker hand or place it nearby.

This is optional.

Do not let it delay the core animation.

---

# 51. HUMAN TEXTURES/COLORS

Use simple textures/colors:

```text id="sugp90"
shirt:
blue/red/green

pants:
dark

skin:
skin-tone color

boots:
brown/black

hat:
straw/brown
```

Realistic human textures are not required.

---

# 52. FAN ANIMATION

Module 2 already contains static fan geometry.

Now animate it.

Use:

```cpp id="xi3zmn"
bool fansOn = true;
float fanAngle = 0.0f;
```

Update:

```cpp id="l495za"
if (fansOn)
{
    fanAngle +=
        fanSpeed *
        deltaTime;
}
```

Use something approximately:

```text id="08ni40"
fanSpeed = 250–400 degrees per second
```

---

# 53. FAN ANGLE WRAPPING

Keep the angle bounded:

```cpp id="vfqis1"
if (fanAngle >= 360.0f)
    fanAngle -= 360.0f;
```

or:

```cpp id="0u5dry"
fanAngle =
    fmod(fanAngle, 360.0f);
```

---

# 54. FAN HIERARCHICAL TRANSFORM

Do not rotate each blade independently around arbitrary world coordinates.

Use:

```text id="bcogdh"
Fan world position
      ↓
Fan center parent
      ↓
Rotate by fanAngle
      ↓
Draw blades relative to center
```

The mounting rod should usually remain stationary.

Only:

- hub
- blades

need to rotate.

---

# 55. FAN CONTROL

Use:

```text id="bmg42z"
F
```

to toggle fan animation.

Correct behavior:

```text id="r3zntz"
Press F
→ fans stop

Press F again
→ fans rotate
```

Use single-press callback behavior.

---

# 56. GLOBAL ANIMATION CONTROLS

Use clear controls.

Recommended:

```text id="7wmaz1"
C = toggle adult cow movement/animation

R = toggle calf running

H = toggle grazing/head animation

F = toggle fans

M = toggle worker movement
```

However, behavior can remain partly automatic.

A strong system is:

- animations start automatically
- keys allow the examiner to pause/resume specific categories

---

# 57. PRESERVE EXISTING CONTROLS

Keep Module 1–2 controls:

```text id="jqc4h1"
W/A/S/D = camera movement
Q/E     = vertical movement
Mouse   = look
Scroll  = zoom
G       = gate
ESC     = exit
```

Do not break them.

---

# 58. RECOMMENDED COMPLETE MODULE 3 CONTROLS

Use:

```text id="nhccah"
W/A/S/D  : Move Camera
Q/E      : Down / Up
Mouse    : Look Around
Scroll   : Zoom

G        : Open/Close Farm Gate

F        : Fan ON/OFF
C        : Adult Cow Animation ON/OFF
R        : Calf Running ON/OFF
H        : Cow Grazing/Head Motion ON/OFF
M        : Worker Movement ON/OFF

ESC      : Exit
```

Do not assign Module 4 light controls yet.

Reserve:

```text id="j7gdtf"
1
2
3
4
5
6
7
```

for lighting.

Reserve:

```text id="ywsdiu"
B
V
```

for Module 5 views.

---

# 59. INPUT TOGGLE IMPLEMENTATION

Do not implement toggle commands as:

```cpp id="htdaeu"
if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    fansOn = !fansOn;
```

This will switch repeatedly while held.

Use:

```cpp id="4ytx14"
if (
    key == GLFW_KEY_F &&
    action == GLFW_PRESS
)
{
    fansOn = !fansOn;
}
```

Apply the same principle for:

- G
- F
- C
- R
- H
- M

---

# 60. ANIMAL TEXTURE/PART COLORS

Do not force one texture across geometrically incompatible parts if it looks poor.

It is acceptable to combine:

- textured torso
- colored legs
- dark hooves
- light horns
- dark eyes

The final animal should read clearly.

---

# 61. AVOID INTERSECTION WITH GROUND

Ensure:

```text id="2u9y25"
cow feet ≈ ground Y
calf feet ≈ ground Y
human feet ≈ ground Y
```

Do not place model origin blindly at `Y = 0` if body geometry extends below the origin.

Create a meaningful root height or adjust local body positions.

---

# 62. ANIMAL ROOT ORIGIN

Choose the animal root logically.

Recommended:

```text id="g8hh0f"
Cow root approximately at ground center
```

or:

```text id="fyrcxo"
Cow root at body center
```

Either is acceptable, but be consistent.

A ground-based root often makes placement easier.

---

# 63. HUMAN ROOT ORIGIN

Human root should preferably be around:

```text id="e5lz4k"
feet/ground position
```

so:

```cpp id="eqi3pw"
worker.position.y = 0.0f;
```

places them correctly.

---

# 64. KEEP FARM OBJECTS FROM MODULE 2

Do not remove:

- textured ground
- road
- fence
- gate
- shed
- barn
- troughs
- hay
- trees
- tank
- props

Module 3 is an additive upgrade.

---

# 65. OBJECT PLACEMENT PLAN

Use purposeful initial positions.

Example:

```text id="qdbfpd"
Cow 1 — walking:
grazing field

Cow 2 — grazing:
central grass

Cow 3 — feeding:
beside trough

Cow 4 — standing:
under cow shed

Calf 1:
running around central grazing area

Calf 2:
smaller nearby running path

Worker 1:
walking barn ↔ shed

Worker 2:
feeding area

Fans:
under shed roof
```

Exact coordinates depend on Module 2 layout.

Inspect existing object coordinates before choosing paths.

---

# 66. AVOID RANDOM POSITIONING EVERY FRAME

Do not call random position generation during rendering.

Objects should follow deterministic positions/paths.

If small random initialization is used once, seed and initialize before the render loop.

For presentation, predictable animation is better.

---

# 67. ANIMATION TIMER

Each animated character can maintain:

```cpp id="l3veqg"
animationTime += deltaTime;
```

or use:

```cpp id="vzmus2"
glfwGetTime()
```

For independently pausable characters, local accumulated animation time is better.

Example:

```cpp id="zwrsw3"
if (cowAnimationEnabled)
{
    cow.animationTime += deltaTime;
}
```

---

# 68. DIFFERENT ANIMATION PHASES

Do not synchronize every cow.

Initialize different phase offsets:

```cpp id="8xmvrj"
cow1.animationTime = 0.0f;
cow2.animationTime = 1.2f;
cow3.animationTime = 2.4f;
```

or include:

```cpp id="4zv781"
phaseOffset
```

This gives more natural movement.

---

# 69. DIFFERENT SPEEDS

Use small differences.

For example:

```text id="bcmtph"
Cow 1 speed = 0.8
Cow 2 = stationary
Cow 3 = stationary
Cow 4 speed = 0.5

Calf 1 speed = faster
Calf 2 speed = slightly different
```

Avoid exact synchronization.

---

# 70. STATE TRANSITIONS — OPTIONAL ADVANCED FEATURE

If stable, one walking cow may automatically change behavior:

```text id="scbses"
Walk
↓
Stop
↓
Graze
↓
Walk
```

For example:

```text id="9jmcxc"
Walking 8 sec
Grazing 5 sec
Walking again
```

This is optional.

Do not implement it until all basic animations work correctly.

---

# 71. SIMPLE STATE MACHINE EXAMPLE

Conceptually:

```cpp id="k7b83v"
switch (cow.state)
{
case CowState::Walking:
    updateWalkingCow(...);
    break;

case CowState::Grazing:
    updateGrazingCow(...);
    break;

case CowState::Feeding:
    updateFeedingCow(...);
    break;

case CowState::Standing:
    updateStandingCow(...);
    break;
}
```

Do not place all behaviors inside one huge conditional expression.

---

# 72. COW WALKING BOUNDS

Cows must remain inside the farm.

Use predefined paths within fence boundaries.

No physics engine is required.

Avoid allowing cows to walk through:

- gate
- walls
- barn
- trees
- trough

unless intentional.

---

# 73. CALF RUNNING BOUNDS

Elliptical running paths should remain fully inside:

```text id="5axuu9"
open grazing field
```

Inspect actual Module 2 coordinates.

Do not use hard-coded path radii that intersect structures.

---

# 74. WORKER WALKING BOUNDS

Choose a straight or gently segmented route with no obstacles.

Simple routes are preferable to pathfinding.

No A* or navigation mesh is required.

---

# 75. OPTIONAL WAYPOINT SYSTEM

For walkers, you may implement:

```cpp id="etj28z"
std::vector<glm::vec3> waypoints;
int currentTarget;
```

Move toward:

```cpp id="n1d15f"
waypoints[currentTarget]
```

When reached:

```cpp id="odj2vg"
currentTarget =
    (currentTarget + 1) %
    waypoints.size();
```

This can be reused for both cows and humans.

However, a simple two-point path is sufficient.

---

# 76. NORMALIZED DIRECTION SAFETY

Do not normalize a near-zero vector.

Before:

```cpp id="fghifx"
glm::normalize(target - position)
```

check distance.

Example:

```cpp id="vdzzd8"
glm::vec3 delta = target - position;

float distance = glm::length(delta);

if (distance > 0.001f)
{
    glm::vec3 direction =
        delta / distance;
}
```

This avoids invalid values.

---

# 77. SPHERE PRIMITIVE

If using the supplied sphere resource, inspect its:

- VAO
- normals
- UVs
- draw method

before integrating it.

Do not recreate sphere vertices every frame.

Initialize sphere geometry once.

Use for:

- cow body/head
- eyes
- human head
- optional fan hub

---

# 78. CONE PRIMITIVE

If a cone is required for horns:

Create it once.

Recommended vertex information:

```text id="e3uw16"
position
normal
texture coordinate
```

Use indexed geometry if practical.

Do not regenerate the mesh for every cow.

---

# 79. TEXTURE SHADER COMPATIBILITY

Animals should continue to use the Module 2 texture shader or a compatible object shader.

Module 4 will later add advanced Phong lighting.

Do not prematurely duplicate the entire final lighting implementation.

However, normals should remain available for future lighting.

---

# 80. DO NOT ADD MODULE 4 FEATURES

Do NOT implement final:

- directional sunlight
- point-light calculations
- spotlights
- light attenuation
- emissive lighting
- day/night mode
- ambient/diffuse/specular toggles
- full Phong lighting controls

yet.

Lamp geometry already exists from Module 2.

Actual lights belong to Module 4.

---

# 81. DO NOT ADD MODULE 5 FEATURES

Do NOT implement:

- Bézier milk can
- B-spline object
- ruled-surface roof
- bird's-eye view
- four viewports
- final Phong/Gouraud comparison

yet.

Module 3 should concentrate on hierarchical modeling and animation.

---

# 82. RENDERING ARCHITECTURE

`renderScene()` should now conceptually contain:

```cpp id="0b728a"
void renderScene()
{
    drawGround();
    drawRoad();

    drawFarmBoundary();
    drawEntrance();

    drawCowShed();
    drawBarn();

    drawFeedingArea();
    drawHayArea();
    drawWaterFacility();

    drawTrees();
    drawFarmProps();

    drawAnimals();

    drawHumans();

    drawFans();
}
```

Exact signatures may use shaders and matrices.

---

# 83. ANIMAL RENDERING FUNCTION

Create:

```cpp id="w60nfl"
void drawAnimals(...)
{
    for (const Cow& cow : cows)
        drawCow(...);

    for (const Calf& calf : calves)
        drawCalf(...);
}
```

or equivalent.

Do not manually call five nearly identical animal functions.

---

# 84. HUMAN RENDERING FUNCTION

Likewise:

```cpp id="fxfyms"
void drawHumans(...)
{
    for (const Worker& worker : workers)
        drawWorker(...);
}
```

if using a collection.

One or two explicit calls are also acceptable if simpler.

---

# 85. ANIMATION UPDATE FUNCTION

Recommended:

```cpp id="sxsp4u"
void updateAnimations(float dt)
{
    updateGate(dt);

    if (cowAnimationEnabled)
        updateCows(dt);

    if (calfAnimationEnabled)
        updateCalves(dt);

    if (workerAnimationEnabled)
        updateWorkers(dt);

    updateFans(dt);
}
```

Grazing state may have a separate toggle if desired.

---

# 86. MODULE 3 CODE ORGANIZATION

As the project grows, use a reasonable structure such as:

```text id="lo1ptm"
AgroFarm/
│
├── main.cpp
├── shader.h
├── camera.h
│
├── primitives/
│   ├── cube.h
│   ├── sphere.h
│   └── cone.h
│
├── scene/
│   ├── farmEnvironment.h
│   ├── farmBuildings.h
│   └── farmProps.h
│
├── models/
│   ├── cow.h
│   ├── human.h
│   └── fan.h
│
├── animation/
│   └── animationState.h
│
├── shaders/
│   ├── texture.vs
│   ├── texture.fs
│   ├── basicColor.vs
│   └── basicColor.fs
│
└── textures/
```

Do not force this exact structure if existing Module 2 organization differs.

Preserve working architecture.

---

# 87. COW.H RESPONSIBILITIES

If creating `cow.h`, it can contain:

- `CowState`
- `Cow` structure
- cow animation variables
- `drawCow()`
- helper functions for cow body parts

Possible helpers:

```cpp id="u5j39t"
drawCowBody(...)
drawCowHead(...)
drawCowLeg(...)
drawCowTail(...)
```

Avoid putting unrelated farm rendering inside this file.

---

# 88. HUMAN.H RESPONSIBILITIES

If creating `human.h`, it can contain:

```cpp id="qmm0rv"
struct Worker

drawWorker(...)
drawWorkerHead(...)
drawWorkerArm(...)
drawWorkerLeg(...)
```

Again, keep the scope focused.

---

# 89. FAN.H RESPONSIBILITIES

If fan geometry is currently inside farm environment code, it may remain there.

If separated:

```cpp id="8f4c86"
drawFan(...)
```

must accept:

```text id="cj0r3n"
position
rotation angle
```

Do not maintain separate hard-coded fan functions for fan 1 and fan 2.

---

# 90. HIERARCHICAL DRAWING HELPER

Reuse the parent-matrix drawing helper prepared earlier.

For example:

```cpp id="kt5jv8"
drawTexturedCubeWithParent(
    shader,
    texture,
    parentMatrix,
    localPosition,
    localRotation,
    localScale
);
```

This helper should now be used extensively.

---

# 91. MATRIX COMPOSITION

For each articulated component:

```text id="4fvowx"
World parent
× Local joint translation
× Local joint rotation
× Geometry offset
× Geometry scale
```

Do not apply geometry scale before translating to the joint if that causes incorrect pivot movement.

---

# 92. COW LEG MATRIX EXAMPLE

Conceptually:

```cpp id="nn2f3j"
glm::mat4 legJoint = cowRoot;

legJoint =
    glm::translate(
        legJoint,
        hipOffset
    );

legJoint =
    glm::rotate(
        legJoint,
        glm::radians(legAngle),
        glm::vec3(0,0,1)
    );

glm::mat4 legModel = legJoint;

legModel =
    glm::translate(
        legModel,
        glm::vec3(0, -legLength / 2, 0)
    );

legModel =
    glm::scale(
        legModel,
        glm::vec3(
            legWidth,
            legLength,
            legDepth
        )
    );
```

Then draw.

This ensures rotation occurs around the hip.

---

# 93. HEAD MATRIX EXAMPLE

Conceptually:

```cpp id="b04w13"
glm::mat4 headJoint = cowRoot;

headJoint =
    glm::translate(
        headJoint,
        neckJointPosition
    );

headJoint =
    glm::rotate(
        headJoint,
        glm::radians(headAngle),
        headRotationAxis
    );
```

Then draw the actual head relative to `headJoint`.

---

# 94. TAIL MATRIX EXAMPLE

Conceptually:

```cpp id="puwaea"
glm::mat4 tailJoint = cowRoot;

tailJoint =
    glm::translate(
        tailJoint,
        tailBasePosition
    );

tailJoint =
    glm::rotate(
        tailJoint,
        glm::radians(tailAngle),
        tailAxis
    );
```

Then offset the tail segment away from the base.

---

# 95. HUMAN WALK MATRIX

Human root:

```cpp id="w2lpuu"
glm::mat4 humanRoot =
    translate(position)
    * rotateY(yaw)
    * scale(...);
```

Then shoulders and hips inherit `humanRoot`.

Do not position limbs in world coordinates.

---

# 96. KEEP JOINT ANGLES REASONABLE

Recommended maximums:

```text id="oy4ssh"
Cow walking leg:
20°–30°

Calf running leg:
30°–40°

Cow grazing head:
approximately 45°–65° down

Tail:
approximately ±20°

Human walk:
20°–30°
```

Tune according to local axis orientation.

Avoid rotations greater than 90° unless intentionally needed.

---

# 97. ANIMATION SPEEDS

Suggested starting values:

```text id="zo9boc"
Adult cow translation:
0.4–1.0 units/sec

Calf:
1.5–3.0 units/sec

Human:
0.8–1.4 units/sec

Fan:
250–400 degrees/sec
```

Adjust to scene scale.

---

# 98. ANIMATION PAUSING

When an animation category is paused:

- position should stop changing
- joint animation should stop or hold its current/neutral state

Avoid a character continuing to slide while its legs are frozen.

---

# 99. FAN STOP BEHAVIOR

When fan is turned off:

It is acceptable to stop immediately.

Optional polish:

- decelerate gradually

Do not implement gradual inertia unless everything else is stable.

---

# 100. COW GRAZING CONTROL

If using:

```text id="38wikn"
H
```

to toggle head/grazing animation, do not force every cow into grazing.

Only cows assigned:

```text id="o9v4e8"
Grazing
or
Feeding
```

should respond.

Walking cows should keep normal head pose.

---

# 101. ANIMAL INTERSECTION

Inspect body parts carefully.

Avoid:

- legs through torso
- horns through head
- head floating away
- tail disconnected
- hooves above legs

Because hierarchical modeling can amplify small local-coordinate errors.

---

# 102. CAMERA-SCALE TEST

View cow and human models from:

- front
- side
- rear
- above slightly

The model should remain recognizable from multiple angles.

Do not optimize only for one camera view.

---

# 103. GATE COMPATIBILITY TEST

After adding many animations, verify:

```text id="bnzc2j"
G
```

still works correctly.

Do not accidentally replace Module 1 animation update with Module 3 logic that omits the gate.

---

# 104. TEXTURE COMPATIBILITY TEST

Verify Module 2 textures still display correctly.

Animals should not break texture-unit management.

If several textures are used, ensure the correct texture is bound before each draw call.

---

# 105. ANIMATION PERFORMANCE

Do not regenerate animal mesh data every frame.

Reuse:

- one cube mesh
- one sphere mesh
- one cone mesh

Different animals are created primarily by:

- matrices
- textures
- state variables

not duplicated buffers.

---

# 106. NO MEMORY LEAK PATTERNS

Do not allocate new geometry using `new` every frame.

Do not reload textures every frame.

Initialize reusable graphics resources once.

---

# 107. CONSOLE CONTROL OUTPUT

Update startup text:

```text id="g8pxij"
=================================================
      INTERACTIVE 3D AGRO FARM - MODULE 3
=================================================

CAMERA
W/A/S/D : Move
Q/E     : Down / Up
Mouse   : Look Around
Scroll  : Zoom

FARM
G       : Open / Close Gate

ANIMATION
C       : Adult Cow Animation ON/OFF
R       : Calf Running ON/OFF
H       : Grazing / Feeding Animation ON/OFF
M       : Worker Animation ON/OFF
F       : Fan ON/OFF

ESC     : Exit

=================================================
MODULE 3:
Animals + Humans + Hierarchical Animation
=================================================
```

---

# 108. EXPECTED FINAL MODULE 3 SCENE

The scene should now feel approximately like:

```text id="pkla23"
                     BACK OF FARM

 -----------------------------------------------------
|                                                     |
|   COW SHED                         FEED BARN         |
|   __________                       _________         |
|  |    FAN   |                     |         |        |
|  | Cow 4    |                     | Worker 1|        |
|  |__________|                     |_________|        |
|                                                     |
|          Cow 3                                     |
|       [feeding trough]                              |
|                                                     |
|               Cow 1 → walking                      |
|                                                     |
|       Cow 2                                        |
|      grazing              Calf →→→                 |
|                           ↑      ↓                  |
|                           ←←←←←←                   |
|                                                     |
|                         Worker 2                    |
|                                                     |
|---------------------- ROAD -------------------------|
|                                                     |
|           PILLAR   GATE   PILLAR                    |
 -----------------------------------------------------
```

Exact placement may differ.

The important point is visible simultaneous activity.

---

# 109. RECOMMENDED BEHAVIOR ASSIGNMENT

Use this as the initial scenario.

## Cow 1

State:

```text id="l6kv58"
Walking
```

Behavior:

- walks slowly across grazing area
- alternating leg movement
- small body bob
- tail movement
- changes direction at path end

---

## Cow 2

State:

```text id="rv9i96"
Grazing
```

Behavior:

- stationary
- head lowered to grass
- slight up/down head movement
- slow tail movement

---

## Cow 3

State:

```text id="v6b5mg"
Feeding
```

Position:

```text id="glinm4"
beside feeding trough
```

Behavior:

- head positioned into trough
- small feeding movement
- mostly stationary

---

## Cow 4

State:

```text id="2ypgxr"
Standing
```

Position:

```text id="c8n8t7"
inside cow shed
```

Behavior:

- neutral posture
- slow head idle
- tail swing

---

## Calf 1

Behavior:

- runs around elliptical grazing path
- rapid leg animation
- faces path tangent

---

## Calf 2

Optional.

Behavior:

- smaller path
- different speed
- different phase

---

## Worker 1

Behavior:

- walks between barn and feeding area
- arms and legs swing

---

## Worker 2

Behavior:

- remains near trough/hay
- simple arm movement or short walking route

---

## Fans

Behavior:

- two fans rotate continuously while enabled
- `F` toggles them

---

# 110. PRESENTATION VALUE

Module 3 should visibly demonstrate these graphics concepts:

### Translation

Animals and humans move through the world.

### Rotation

- legs
- arms
- head
- tail
- fan blades

### Scaling

Same base model produces different sizes, especially calves.

### Hierarchical modeling

Child body parts follow parent transformations.

### Animation

Time-varying transforms create natural movement.

### Reuse

The same primitive meshes create many complex objects.

### Local vs world coordinates

Joint animation occurs correctly around local pivots.

These should be explainable during viva.

---

# 111. MODULE 3 ACCEPTANCE CHECKLIST

Do not consider Module 3 finished until:

- Module 1 features still work.
- Module 2 textures/environment still work.
- Farm gate still opens/closes.
- At least 3 adult cows are visible.
- Preferably 4 cows are visible.
- Cow model has recognizable body/head/legs/tail.
- Cow components use hierarchical transformations.
- Legs rotate around upper joints.
- Head rotates around neck joint.
- Tail rotates around tail root.
- At least one cow physically walks.
- Walking cow faces its direction of movement.
- Walking legs alternate.
- At least one cow grazes.
- Grazing head visibly lowers.
- At least one cow feeds beside the trough.
- At least one cow has idle behavior.
- At least one calf exists.
- Calf physically runs.
- Calf legs animate faster than adult cow legs.
- Calf faces its movement path.
- At least one human exists.
- Preferably two humans exist.
- Walking human has opposite arm/leg movement.
- Humans use hierarchical transformations.
- Two fans rotate.
- Fan blades rotate around their central hub.
- `F` toggles fans.
- Animation uses `deltaTime`.
- Animal paths remain inside the farm.
- Animals do not obviously travel through buildings.
- No geometry buffers are generated every frame.
- No external 3D models are used.
- No Module 4 lighting features are prematurely implemented.
- No Module 5 curved/multi-view features are prematurely implemented.

---

# 112. TESTING SEQUENCE

Perform this exact testing sequence.

## Test 1 — Existing project

Launch application.

Verify:

- environment loads
- textures load
- camera works
- gate works

---

## Test 2 — Static cow

Temporarily pause animations.

Inspect one cow from several angles.

Verify:

- proportions
- connected body parts
- texture
- no floating components

---

## Test 3 — Leg pivots

Enable walking.

Confirm:

- legs rotate at upper joints
- body does not distort
- feet do not orbit around cow center

---

## Test 4 — Walking path

Watch Cow 1 complete the entire route.

Confirm:

- cow remains inside field
- turns correctly
- faces movement direction

---

## Test 5 — Grazing

Inspect Cow 2.

Confirm:

- head lowers from neck
- motion is subtle
- body remains stable

---

## Test 6 — Feeding

Inspect Cow 3.

Ensure head intersects trough naturally enough to imply feeding but does not disappear completely through it.

---

## Test 7 — Calf

Watch one full running loop.

Confirm:

- no sideways sliding
- yaw follows tangent
- legs animate rapidly
- no structure collision

---

## Test 8 — Humans

Inspect Worker 1 from side/front.

Verify:

- legs alternate
- arms alternate opposite legs
- worker faces walking direction

---

## Test 9 — Fans

Press:

```text id="3mq4l9"
F
```

Verify fans stop.

Press again.

Verify fans rotate.

---

## Test 10 — Combined scene

Enable all animations simultaneously.

Check for:

- stable frame rate
- correct textures
- correct gate
- no obvious matrix corruption

---

# 113. DO NOT SACRIFICE STABILITY FOR REALISM

The objective is not photorealism.

Prioritize:

1. correct OpenGL implementation
2. hierarchical modeling
3. clear animation
4. recognizable models
5. stable performance
6. clean code

over:

- detailed anatomy
- complex physics
- motion capture
- imported meshes

---

# 114. PREPARATION FOR MODULE 4

Module 4 will introduce realistic lighting.

Therefore, by the end of Module 3:

All visible geometry that will receive lighting should preferably already provide:

```text id="7rutml"
vertex position
normal
texture coordinate
```

Do not redesign all animal meshes in Module 4 just to obtain normals.

If the current sphere/cone already supports normals, preserve them.

---

# 115. PREPARATION FOR MODULE 4 LIGHT LOCATIONS

Do not implement actual advanced lights yet, but preserve existing Module 2 fixtures:

```text id="vpx9yx"
shed bulbs
barn lamp
entrance lamp
```

Module 4 will turn these into:

- point lights
- spotlight
- emissive objects

---

# 116. PREPARATION FOR MODULE 5

Do not block locations intended for:

```text id="fkx74f"
Bezier milk can
curved water object
spline object
ruled cow-shed roof
```

Also keep scene rendering centralized.

Module 5 will call the same:

```cpp id="hcr7ym"
renderScene(...)
```

multiple times for different viewports.

Therefore, do NOT hard-code viewport/camera changes inside individual farm-object functions.

---

# 117. IMPORTANT RENDER FUNCTION RULE

Object functions such as:

```cpp id="2czggh"
drawCow(...)
drawHuman(...)
drawFan(...)
```

must not call:

```cpp id="nvj6wf"
glClear(...)
glViewport(...)
glfwSwapBuffers(...)
```

They should only render their own objects.

The central application controls framebuffer/view operations.

This is important for Module 5.

---

# 118. WHAT I WANT FROM YOU

Now implement **MODULE 3 only**, continuing from the completed Module 2 project.

Before coding:

1. inspect the current Module 2 file structure;
2. inspect the existing primitive implementations;
3. inspect the supplied sphere implementation;
4. determine whether a cone already exists;
5. inspect the parent-matrix helper created for gate/fan geometry;
6. inspect actual farm coordinates so animal paths avoid structures.

Then implement Module 3 systematically.

Do not replace the current working project with a minimal demo.

Do not give only isolated snippets.

Provide the complete compile-ready implementation needed to upgrade Module 2 to Module 3.

For every new or modified source file, clearly give its filename and complete relevant content.

Do not leave required code as:

```text id="ts2dgm"
// TODO
// same as previous
// implement animation here
```

---

# 119. FINAL RESPONSE REQUIREMENTS

At the end of the implementation, provide:

## A. Files changed

List every new/modified:

- `.cpp`
- `.h`
- shader if modified

---

## B. Animation architecture

Briefly explain:

- cow state structure
- calf state
- worker state
- fan state
- `updateAnimations()`

---

## C. Hierarchy diagrams

Show the hierarchy for:

- cow
- human
- fan

---

## D. Controls

List all keyboard/mouse controls.

---

## E. Animal behavior list

State exactly:

- what Cow 1 does
- what Cow 2 does
- what Cow 3 does
- what Cow 4 does
- what each calf does
- what each worker does

---

## F. Test procedure

Give the ordered testing sequence.

---

## G. Expected visual result

Explain what should happen when the program runs.

---

## H. Module 4 readiness

Confirm that:

- animal geometry has usable normals
- texture support still works
- light fixture geometry exists
- `renderScene()` remains centralized
- animated objects can later receive Phong lighting

but DO NOT implement Module 4 lighting yet.

---

# FINAL MODULE 3 PRIORITY

The completed scene should visibly communicate:

> This is no longer merely a modeled farm. It is an animated farm environment where animals, people, equipment and articulated body parts behave independently through hierarchical OpenGL transformations.

The most important technical objectives are:

**hierarchical modeling + correct joint pivots + reusable primitive geometry + independent animation states + delta-time-based world movement + stable integration with Modules 1 and 2.**