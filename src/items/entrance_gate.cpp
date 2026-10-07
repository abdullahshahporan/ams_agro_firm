// Item: entrance gate.
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

void FarmScene::drawEntrance(const Shader& shader, float gateAngleDegrees) const
{
    applyMaterial(shader, Materials::brick());
    drawPillar(shader, -4.30f);
    drawPillar(shader, 4.30f);
    applyMaterial(shader, Materials::wood());
    drawFarmSign(shader);

    applyMaterial(shader, Materials::metal());

    glm::mat4 leftParent(1.0f);
    leftParent = glm::translate(leftParent, glm::vec3(LeftHingeX, 0.0f, EntranceZ - 0.03f));
    leftParent = glm::rotate(leftParent, glm::radians(gateAngleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    drawGateLeaf(shader, leftParent, 1.0f);

    glm::mat4 rightParent(1.0f);
    rightParent = glm::translate(rightParent, glm::vec3(RightHingeX, 0.0f, EntranceZ - 0.03f));
    rightParent = glm::rotate(rightParent, glm::radians(-gateAngleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    drawGateLeaf(shader, rightParent, -1.0f);
}

void FarmScene::drawPillar(const Shader& shader, float xPosition) const
{
    cubes_.drawTextured(shader, glm::vec3(xPosition, 0.16f, EntranceZ), glm::vec3(1.18f, 0.32f, 1.18f), textures_.concrete, ConcreteTint);
    cubes_.drawTextured(shader, glm::vec3(xPosition, 2.25f, EntranceZ), glm::vec3(0.86f, 4.20f, 0.86f), textures_.brick, BrickTint, glm::vec2(2.0f, 6.0f));
    cubes_.drawTextured(shader, glm::vec3(xPosition, 4.42f, EntranceZ), glm::vec3(1.12f, 0.28f, 1.12f), textures_.concrete, ConcreteTint);
    cubes_.drawTextured(shader, glm::vec3(xPosition, 4.68f, EntranceZ), glm::vec3(0.78f, 0.28f, 0.78f), textures_.brick, BrickTint);
}

void FarmScene::drawGateLeaf(const Shader& shader, const glm::mat4& hingeTransform, float direction) const
{
    constexpr float leafWidth = 3.82f;
    constexpr float bottom = 0.35f;
    constexpr float height = 2.55f;

    for (const float xDistance : {0.12f, leafWidth - 0.12f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * xDistance, bottom + height * 0.5f, 0.0f), glm::vec3(0.16f, height, 0.16f), textures_.metal, DarkMetal, glm::vec2(0.5f, 3.0f));

    for (const float y : {bottom + 0.12f, bottom + height * 0.50f, bottom + height - 0.12f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * leafWidth * 0.5f, y, 0.0f), glm::vec3(leafWidth, 0.16f, 0.16f), textures_.metal, DarkMetal, glm::vec2(4.0f, 0.5f));

    for (int bar = 1; bar <= 5; ++bar)
    {
        const float distance = leafWidth * static_cast<float>(bar) / 6.0f;
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * distance, bottom + height * 0.5f, 0.0f), glm::vec3(0.09f, height - 0.18f, 0.09f), textures_.metal, glm::vec3(0.86f, 0.72f, 0.34f), glm::vec2(0.4f, 3.0f));
    }

    for (const float y : {0.72f, 2.42f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(0.0f, y, 0.0f), glm::vec3(0.24f, 0.28f, 0.26f), textures_.metal, glm::vec3(0.38f));
}
