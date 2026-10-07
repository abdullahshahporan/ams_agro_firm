// Item: water tank.
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

void FarmScene::drawWaterFacility(const Shader& shader) const
{
    const glm::vec3 origin(13.0f, 0.0f, 0.5f);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f), glm::vec3(5.0f, 0.20f, 4.5f), textures_.concrete, ConcreteTint, glm::vec2(3.0f));

    for (const float x : {-1.45f, 1.45f})
        for (const float z : {-1.25f, 1.25f})
            cubes_.drawTextured(shader, origin + glm::vec3(x, 2.0f, z), glm::vec3(0.28f, 3.8f, 0.28f), textures_.metal, DarkMetal, glm::vec2(0.5f, 4.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.85f, 0.0f), glm::vec3(4.0f, 2.5f, 3.4f), textures_.metal, glm::vec3(0.56f, 0.76f, 0.88f), glm::vec2(3.0f, 2.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 5.18f, 0.0f), glm::vec3(4.25f, 0.20f, 3.65f), textures_.roof, RoofTint, glm::vec2(3.0f));
    cubes_.drawColored(shader, origin + glm::vec3(0.0f, 3.85f, 1.73f), glm::vec3(3.7f, 0.32f, 0.08f), glm::vec3(0.16f, 0.42f, 0.65f));

    // Ladder and lower cross-bracing make the block-built tank read as a tower.
    for (const float x : {-0.38f, 0.38f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 2.05f, 1.70f), glm::vec3(0.10f, 4.0f, 0.10f), textures_.metal, glm::vec3(0.68f));
    for (int rung = 0; rung < 7; ++rung)
        cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.55f + rung * 0.52f, 1.72f), glm::vec3(0.86f, 0.09f, 0.09f), textures_.metal, glm::vec3(0.68f));
}
