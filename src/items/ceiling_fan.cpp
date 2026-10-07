// Item: ceiling fan.
#include "farm_scene.h"
#include "animation_system.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;
using namespace FarmStyle;

void FarmScene::drawShedFans(const Shader& shader, float fanAngleDegrees) const
{
    drawFan(shader, glm::vec3(-12.1f, 3.15f, -7.0f), fanAngleDegrees);
    drawFan(shader, glm::vec3(-7.9f, 3.15f, -7.0f), -fanAngleDegrees);
}

void FarmScene::drawFan(const Shader& shader, const glm::vec3& position, float parentAngle) const
{
    glm::mat4 mount(1.0f);
    mount = glm::translate(mount, position);
    cubes_.drawTextured(shader, mount, glm::vec3(0.0f, 0.48f, 0.0f),
                        glm::vec3(0.12f, 0.95f, 0.12f), textures_.metal, DarkMetal);

    // Only the rotor inherits this transform; the ceiling mount stays fixed.
    glm::mat4 rotor = glm::rotate(mount, glm::radians(parentAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f), glm::vec3(0.42f, 0.22f, 0.42f), textures_.metal, glm::vec3(0.58f));
    cubes_.drawTextured(shader, rotor, glm::vec3(1.05f, 0.0f, 0.0f), glm::vec3(1.85f, 0.10f, 0.38f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(-1.05f, 0.0f, 0.0f), glm::vec3(1.85f, 0.10f, 0.38f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f, 0.0f, 1.05f), glm::vec3(0.38f, 0.10f, 1.85f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f, 0.0f, -1.05f), glm::vec3(0.38f, 0.10f, 1.85f), textures_.metal, DarkMetal);
}

void AnimationSystem::updateFans(float deltaTime)
{
    if (fansOn_)
        fanAngle_ = fmod(fanAngle_ + 320.0f * deltaTime, 360.0f);
}
