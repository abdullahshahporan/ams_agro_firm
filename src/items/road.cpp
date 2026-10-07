// Item: road.
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

void FarmScene::drawRoad(const Shader& shader) const
{
    cubes_.drawTextured(shader,
                        glm::vec3(0.0f, -0.055f, 72.0f),
                        glm::vec3(8.0f, 0.09f, 112.0f),
                        textures_.soil, glm::vec3(0.82f, 0.74f, 0.61f),
                        glm::vec2(3.0f, 42.0f));
    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, 0.025f, 9.0f),
        glm::vec3(4.8f, 0.08f, 22.0f),
        textures_.soil,
        glm::vec3(0.88f, 0.80f, 0.68f),
        glm::vec2(2.0f, 11.0f));
    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, 0.026f, 19.0f),
        glm::vec3(7.0f, 0.085f, 2.0f),
        textures_.soil,
        glm::vec3(0.84f, 0.76f, 0.64f),
        glm::vec2(3.0f, 1.0f));
}
