// Item: boundary fence.
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

void FarmScene::drawFarmBoundary(const Shader& shader, float lampEmission) const
{
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, EntranceZ), glm::vec3(-4.75f, 0.0f, EntranceZ), lampEmission);
    drawFenceRun(shader, glm::vec3(4.75f, 0.0f, EntranceZ), glm::vec3(18.0f, 0.0f, EntranceZ), lampEmission);
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, -18.0f), glm::vec3(-18.0f, 0.0f, EntranceZ), lampEmission);
    drawFenceRun(shader, glm::vec3(18.0f, 0.0f, -18.0f), glm::vec3(18.0f, 0.0f, EntranceZ), lampEmission);
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, -18.0f), glm::vec3(18.0f, 0.0f, -18.0f), lampEmission);
}

void FarmScene::drawFenceRun(const Shader& shader, const glm::vec3& start,
                             const glm::vec3& end, float lampEmission) const
{
    const glm::vec3 difference = end - start;
    const float length = glm::length(difference);
    const glm::vec3 direction = difference / length;
    const int sectionCount = max(1, static_cast<int>(ceil(length / 2.7f)));
    const float sectionLength = length / static_cast<float>(sectionCount);
    const bool runsAlongX = abs(difference.x) > abs(difference.z);

    for (int i = 0; i <= sectionCount; ++i)
    {
        const glm::vec3 point = start + direction * (sectionLength * static_cast<float>(i));
        cubes_.drawTextured(
            shader,
            glm::vec3(point.x, 0.86f, point.z),
            glm::vec3(0.23f, 1.72f, 0.23f),
            textures_.wood,
            WarmWood,
            glm::vec2(0.6f, 2.2f));

        cubes_.drawTextured(
            shader,
            glm::vec3(point.x, 1.78f, point.z),
            glm::vec3(0.31f, 0.16f, 0.31f),
            textures_.wood,
            glm::vec3(0.72f, 0.60f, 0.45f));

        Material globeMaterial = Materials::metal();
        globeMaterial.emission = glm::vec3(1.0f, 0.80f, 0.43f) * (lampEmission * 0.72f);
        applyMaterial(shader, globeMaterial);
        primitives_.drawSphere(shader, glm::mat4(1.0f),
                               glm::vec3(point.x, 2.08f, point.z),
                               glm::vec3(0.34f), glm::vec3(1.0f, 0.84f, 0.55f));
        applyMaterial(shader, Materials::wood());
    }

    for (int i = 0; i < sectionCount; ++i)
    {
        const glm::vec3 center = start + direction * (sectionLength * (static_cast<float>(i) + 0.5f));
        for (const float height : {0.55f, 1.15f})
        {
            const glm::vec3 scale = runsAlongX
                ? glm::vec3(sectionLength + 0.04f, 0.15f, 0.16f)
                : glm::vec3(0.16f, 0.15f, sectionLength + 0.04f);
            cubes_.drawTextured(
                shader,
                glm::vec3(center.x, height, center.z),
                scale,
                textures_.wood,
                WarmWood,
                glm::vec2(sectionLength, 0.5f));
        }
    }
}
