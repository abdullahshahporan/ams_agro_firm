// Item: cow stalls.
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

const array<glm::vec3, 4>& FarmScene::futureCowPositions()
{
    static const array<glm::vec3, 4> positions = {
        glm::vec3(-12.0f, 0.0f, -7.0f),
        glm::vec3( -9.0f, 0.0f, -7.5f),
        glm::vec3(  2.0f, 0.0f, -7.0f),
        glm::vec3(  6.0f, 0.0f, -4.5f)
    };
    return positions;
}

void FarmScene::drawTrough(const Shader& shader, const glm::vec3& position, float length) const
{
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.18f, 0.0f), glm::vec3(length, 0.18f, 0.90f), textures_.concrete, ConcreteTint, glm::vec2(3.0f, 1.0f));
    for (const float z : {-0.48f, 0.48f})
        cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.53f, z), glm::vec3(length, 0.70f, 0.14f), textures_.concrete, ConcreteTint, glm::vec2(3.0f, 1.0f));
    for (const float x : {-0.5f * length, 0.5f * length})
        cubes_.drawTextured(shader, position + glm::vec3(x, 0.53f, 0.0f), glm::vec3(0.14f, 0.70f, 1.10f), textures_.concrete, ConcreteTint);
    cubes_.drawColored(shader, position + glm::vec3(0.0f, 0.38f, 0.0f), glm::vec3(length - 0.22f, 0.08f, 0.72f), glm::vec3(0.30f, 0.22f, 0.08f));
}

void FarmScene::drawIndoorCowStalls(const Shader& shader,
                                    float stallGateAngleDegrees,
                                    int deliveredHayLoads) const
{
    // Two stalls align with the tied cows. The front remains accessible for
    // feeding, while O opens the rear gates for inspection behind the cattle.
    applyMaterial(shader, Materials::wood());
    for (const float x : {-14.0f, -10.0f, -6.0f})
    {
        cubes_.drawTextured(shader, glm::vec3(x, 0.82f, -7.65f),
                            glm::vec3(0.16f, 1.55f, 0.16f),
                            textures_.wood, WarmWood);
        cubes_.drawTextured(shader, glm::vec3(x, 0.88f, -6.55f),
                            glm::vec3(0.12f, 0.12f, 2.20f),
                            textures_.wood, WarmWood, glm::vec2(2.0f, 0.5f));
    }
    cubes_.drawTextured(shader, glm::vec3(-10.0f, 1.05f, -6.10f),
                        glm::vec3(7.9f, 0.14f, 0.14f),
                        textures_.wood, WarmWood, glm::vec2(6.0f, 0.5f));

    applyMaterial(shader, Materials::concrete());
    drawTrough(shader, glm::vec3(-12.0f, 0.0f, -5.45f), 2.55f);
    drawTrough(shader, glm::vec3( -8.0f, 0.0f, -5.45f), 2.55f);

    // Fresh cut grass/fodder is deliberately green and visibly raised above
    // the trough base so it is obvious when entering the shed.
    applyMaterial(shader, Materials::grass());
    for (const float xCenter : {-12.0f, -8.0f})
    {
        for (int strip = -2; strip <= 2; ++strip)
        {
            cubes_.drawTextured(
                shader,
                glm::vec3(xCenter + strip * 0.42f, 0.48f + abs(strip) * 0.015f, -5.45f),
                glm::vec3(0.50f, 0.15f, 0.62f), textures_.hay,
                glm::vec3(0.30f, 0.72f, 0.18f), glm::vec2(1.0f),
                static_cast<float>(strip) * 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    if (deliveredHayLoads > 0)
    {
        applyMaterial(shader, Materials::hay());
        const float height = min(deliveredHayLoads, 4) * 0.055f;
        for (int strip = -2; strip <= 2; ++strip)
            cubes_.drawTextured(shader,
                                glm::vec3(-12.0f + strip * 0.40f,
                                          0.60f + height, -5.45f),
                                glm::vec3(0.40f, 0.15f + height, 0.52f),
                                textures_.hay, HayTint);
    }

    // Two small fodder mats sit inside the ox-side calf lane. N routes the
    // calves through the side door to these positions and lowers their heads.
    for (const float zCenter : {-10.98f, -9.43f})
    {
        cubes_.drawTextured(shader, glm::vec3(-6.78f, 0.18f, zCenter),
                            glm::vec3(0.72f, 0.14f, 0.56f), textures_.hay,
                            glm::vec3(0.28f, 0.70f, 0.16f));
        for (int strip = -1; strip <= 1; ++strip)
            cubes_.drawColored(shader,
                               glm::vec3(-6.78f + strip * 0.20f, 0.29f, zCenter),
                               glm::vec3(0.05f, 0.24f, 0.05f),
                               glm::vec3(0.20f, 0.58f, 0.12f),
                               static_cast<float>(strip) * 12.0f,
                               glm::vec3(1.0f, 0.0f, 0.0f));
    }

    applyMaterial(shader, Materials::wood());
    drawStallGate(shader, glm::vec3(-14.0f, 0.0f, -11.05f), stallGateAngleDegrees);
    drawStallGate(shader, glm::vec3(-10.0f, 0.0f, -11.05f), stallGateAngleDegrees);
}

void FarmScene::drawStallGate(const Shader& shader, const glm::vec3& hinge,
                              float angleDegrees) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, hinge);
    parent = glm::rotate(parent, glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    for (float x : {0.10f, 3.90f})
        cubes_.drawTextured(shader, parent, glm::vec3(x, 0.78f, 0.0f),
                            glm::vec3(0.14f, 1.45f, 0.14f), textures_.wood, WarmWood);
    for (float y : {0.24f, 0.78f, 1.32f})
        cubes_.drawTextured(shader, parent, glm::vec3(2.0f, y, 0.0f),
                            glm::vec3(4.0f, 0.13f, 0.13f), textures_.wood,
                            WarmWood, glm::vec2(4.0f, 0.5f));
}
