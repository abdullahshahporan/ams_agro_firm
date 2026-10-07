// Item: barn.
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

void FarmScene::drawBarn(const Shader& shader) const
{
    const glm::vec3 origin(11.0f, 0.0f, -11.0f);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f), glm::vec3(7.0f, 0.20f, 6.5f), textures_.concrete, ConcreteTint, glm::vec2(4.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(-3.15f, 2.1f, 0.0f), glm::vec3(0.32f, 4.0f, 6.0f), textures_.brick, BrickTint, glm::vec2(4.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(3.15f, 2.1f, 0.0f), glm::vec3(0.32f, 4.0f, 6.0f), textures_.brick, BrickTint, glm::vec2(4.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 2.1f, -3.0f), glm::vec3(6.0f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(5.0f, 3.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(-2.25f, 2.1f, 3.0f), glm::vec3(1.8f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(2.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(2.25f, 2.1f, 3.0f), glm::vec3(1.8f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(2.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.50f, 3.0f), glm::vec3(2.7f, 1.2f, 0.32f), textures_.brick, BrickTint, glm::vec2(3.0f, 1.0f));

    for (const float x : {-1.28f, 1.28f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.50f, 3.25f), glm::vec3(0.16f, 3.0f, 0.14f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 2.95f, 3.25f), glm::vec3(2.72f, 0.16f, 0.14f), textures_.wood, WarmWood);

    cubes_.drawTextured(shader, origin + glm::vec3(-1.65f, 4.45f, 0.0f), glm::vec3(3.75f, 0.28f, 6.8f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), 27.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(1.65f, 4.45f, 0.0f), glm::vec3(3.75f, 0.28f, 6.8f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), -27.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Simple shuttered windows visually distinguish the enclosed barn.
    for (const float x : {-2.15f, 2.15f})
    {
        cubes_.drawColored(shader, origin + glm::vec3(x, 2.65f, 3.19f), glm::vec3(0.80f, 0.65f, 0.07f), glm::vec3(0.20f, 0.34f, 0.38f));
        cubes_.drawTextured(shader, origin + glm::vec3(x, 2.65f, 3.24f), glm::vec3(0.08f, 0.82f, 0.09f), textures_.wood, WarmWood);
    }
}
