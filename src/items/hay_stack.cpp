// Item: hay stack.
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

void FarmScene::drawHayArea(const Shader& shader, bool pickupHayAvailable) const
{
    drawHayBale(shader, glm::vec3(-10.2f, 0.52f, 2.0f), -6.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 2.2f), 4.0f);
    drawHayBale(shader, glm::vec3(-5.9f, 0.52f, 2.0f), -3.0f);
    drawHayBale(shader, glm::vec3(-9.1f, 1.48f, 2.1f), 3.0f);
    drawHayBale(shader, glm::vec3(-6.9f, 1.48f, 2.1f), -5.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 4.0f), 8.0f);
    drawHayBale(shader, glm::vec3(-6.0f, 0.52f, 4.1f), -7.0f);
    if (pickupHayAvailable)
        cubes_.drawTextured(shader, glm::vec3(-4.1f, 0.19f, 3.4f),
                            glm::vec3(0.94f, 0.37f, 0.72f),
                            textures_.hay, HayTint);
}

void FarmScene::drawHayBale(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(1.9f, 0.90f, 1.15f), textures_.hay, HayTint, glm::vec2(2.0f, 1.0f));
    cubes_.drawColored(shader, parent, glm::vec3(-0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
    cubes_.drawColored(shader, parent, glm::vec3(0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
}
