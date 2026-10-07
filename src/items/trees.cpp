// Item: trees.
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

void FarmScene::drawTrees(const Shader& shader) const
{
    const array<pair<glm::vec3, float>, 8> trees = {{
        {glm::vec3(-15.8f, 0.0f, -14.5f), 1.05f},
        {glm::vec3(-16.4f, 0.0f,  7.2f), 0.90f},
        {glm::vec3(-16.6f, 0.0f, 13.0f), 0.82f},
        {glm::vec3( 15.8f, 0.0f,-15.4f), 1.00f},
        {glm::vec3( 16.0f, 0.0f,  6.0f), 0.90f},
        {glm::vec3( 13.6f, 0.0f, 12.5f), 0.78f},
        {glm::vec3( -2.0f, 0.0f,-16.0f), 0.92f},
        {glm::vec3(  5.5f, 0.0f,-16.1f), 0.82f}
    }};
    for (const auto& tree : trees)
        drawTree(shader, tree.first, tree.second);
}

void FarmScene::drawTree(const Shader& shader, const glm::vec3& position, float scale) const
{
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 1.9f * scale, 0.0f), glm::vec3(0.55f, 3.8f, 0.55f) * scale, textures_.bark, glm::vec3(0.88f, 0.78f, 0.66f), glm::vec2(1.0f, 3.0f));
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 4.2f * scale, 0.0f), glm::vec3(3.2f, 2.2f, 3.0f) * scale, textures_.leaves, LeafTint, glm::vec2(1.0f));
    cubes_.drawTextured(shader, position + glm::vec3(-1.15f * scale, 4.0f * scale, 0.35f * scale), glm::vec3(2.1f, 1.8f, 2.1f) * scale, textures_.leaves, glm::vec3(0.62f, 0.86f, 0.56f), glm::vec2(1.0f), 18.0f);
    cubes_.drawTextured(shader, position + glm::vec3(1.05f * scale, 4.35f * scale, -0.30f * scale), glm::vec3(2.0f, 1.7f, 2.2f) * scale, textures_.leaves, glm::vec3(0.72f, 0.94f, 0.64f), glm::vec2(1.0f), -15.0f);
}
