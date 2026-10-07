// Item: crate.
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

void FarmScene::drawCrate(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(1.2f, 1.0f, 1.1f), textures_.wood, WarmWood, glm::vec2(1.5f));
    for (const float x : {-0.52f, 0.52f})
        cubes_.drawTextured(shader, parent, glm::vec3(x, 0.0f, 0.56f), glm::vec3(0.10f, 1.08f, 0.08f), textures_.wood, glm::vec3(0.62f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f, 0.0f, 0.57f), glm::vec3(1.12f, 0.10f, 0.08f), textures_.wood, glm::vec3(0.62f));
}
