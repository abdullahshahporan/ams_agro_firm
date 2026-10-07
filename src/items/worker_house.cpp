// Item: worker house.
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

void FarmScene::drawWorkerHouse(const Shader& shader, bool doorOpen) const
{
    // Kept clear of the right entrance leaf's full 90-degree swing envelope.
    const glm::vec3 origin(8.0f, 0.0f, 12.0f);
    applyMaterial(shader, Materials::brick());
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f),
                        glm::vec3(3.5f, 0.20f, 3.2f), textures_.concrete,
                        ConcreteTint, glm::vec2(3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(-1.60f, 1.35f, 0.0f),
                        glm::vec3(0.22f, 2.55f, 3.0f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(1.60f, 1.35f, 0.0f),
                        glm::vec3(0.22f, 2.55f, 3.0f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 1.35f, 1.48f),
                        glm::vec3(3.0f, 2.55f, 0.22f), textures_.brick, BrickTint);
    for (float x : {-1.15f, 1.15f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.35f, -1.48f),
                            glm::vec3(0.70f, 2.55f, 0.22f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 2.20f, -1.48f),
                        glm::vec3(1.60f, 0.85f, 0.22f), textures_.brick, BrickTint);
    applyMaterial(shader, Materials::wood());
    glm::mat4 door(1.0f);
    door = glm::translate(door, origin + glm::vec3(-0.625f, 0.0f, -1.52f));
    if (doorOpen)
        door = glm::rotate(door, glm::radians(88.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, door, glm::vec3(0.625f, 1.0f, 0.0f),
                        glm::vec3(1.25f, 1.95f, 0.12f), textures_.wood,
                        glm::vec3(0.38f, 0.24f, 0.12f));
    applyMaterial(shader, Materials::metal());
    cubes_.drawTextured(shader, origin + glm::vec3(-0.78f, 2.85f, 0.0f),
                        glm::vec3(1.95f, 0.18f, 3.55f), textures_.roof, RoofTint,
                        glm::vec2(2.0f), 24.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.78f, 2.85f, 0.0f),
                        glm::vec3(1.95f, 0.18f, 3.55f), textures_.roof, RoofTint,
                        glm::vec2(2.0f), -24.0f, glm::vec3(0.0f, 0.0f, 1.0f));
}
