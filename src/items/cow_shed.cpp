// Item: cow shed.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;
using namespace FarmStyle;

void FarmScene::drawCowShed(const Shader& shader, float calfShedDoorAngleDegrees) const
{
    const glm::vec3 origin(-10.0f, 0.0f, -8.0f);

    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.09f, 0.0f), glm::vec3(8.2f, 0.18f, 7.0f), textures_.concrete, ConcreteTint, glm::vec2(5.0f, 4.0f));
    // The rear remains gate-enclosed instead of being sealed by a brick wall,
    // allowing O to create a genuine visitor route behind both adult cattle.
    cubes_.drawTextured(shader, origin + glm::vec3(-3.78f, 1.0f, -1.7f), glm::vec3(0.24f, 1.8f, 2.8f), textures_.brick, BrickTint, glm::vec2(2.0f, 2.0f));
    // Split the ox-side wall around a real calf doorway instead of allowing
    // calves to pass through the concrete feed boxes at the shed front.
    cubes_.drawTextured(shader, glm::vec3(-6.22f, 1.0f, -10.72f),
                        glm::vec3(0.24f, 1.8f, 0.92f), textures_.brick,
                        BrickTint, glm::vec2(1.0f, 2.0f));
    cubes_.drawTextured(shader, glm::vec3(-6.22f, 1.0f, -8.58f),
                        glm::vec3(0.24f, 1.8f, 0.84f), textures_.brick,
                        BrickTint, glm::vec2(1.0f, 2.0f));
    cubes_.drawTextured(shader, glm::vec3(-6.22f, 2.05f, -9.64f),
                        glm::vec3(0.24f, 0.34f, 3.05f), textures_.brick,
                        BrickTint, glm::vec2(3.0f, 1.0f));
    drawCalfShedSideDoor(shader, calfShedDoorAngleDegrees);

    for (const float x : {-3.65f, 0.0f, 3.65f})
        for (const float z : {-3.0f, 3.0f})
            cubes_.drawTextured(shader, origin + glm::vec3(x, 1.85f, z), glm::vec3(0.28f, 3.7f, 0.28f), textures_.metal, glm::vec3(0.76f), glm::vec2(0.6f, 4.0f));

    for (const float z : {-3.0f, 3.0f})
        cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.55f, z), glm::vec3(7.7f, 0.28f, 0.28f), textures_.wood, WarmWood, glm::vec2(6.0f, 0.5f));
    for (const float x : {-3.5f, 0.0f, 3.5f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 3.55f, 0.0f), glm::vec3(0.25f, 0.25f, 6.2f), textures_.wood, WarmWood, glm::vec2(5.0f, 0.5f));

    // Module 5 draws one sampled ruled-surface roof here. The old overlapping
    // planar roof halves were intentionally removed.
}

void FarmScene::drawCalfShedSideDoor(const Shader& shader, float angleDegrees) const
{
    constexpr float width = 1.24f;
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, glm::vec3(-6.06f, 0.0f, -10.24f));
    parent = glm::rotate(parent, glm::radians(angleDegrees),
                         glm::vec3(0.0f, 1.0f, 0.0f));

    applyMaterial(shader, Materials::wood());
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f, 0.94f, width * 0.5f),
                        glm::vec3(0.14f, 1.76f, width), textures_.wood,
                        glm::vec3(0.60f, 0.39f, 0.18f), glm::vec2(2.0f, 2.0f));
    for (float y : {0.28f, 0.94f, 1.60f})
        cubes_.drawTextured(shader, parent,
                            glm::vec3(-0.09f, y, width * 0.5f),
                            glm::vec3(0.10f, 0.10f, width), textures_.metal,
                            DarkMetal);
    cubes_.drawColored(shader, parent, glm::vec3(-0.13f, 0.92f, width - 0.14f),
                       glm::vec3(0.10f, 0.17f, 0.10f),
                       glm::vec3(0.82f, 0.64f, 0.20f));
    applyMaterial(shader, Materials::brick());
}
