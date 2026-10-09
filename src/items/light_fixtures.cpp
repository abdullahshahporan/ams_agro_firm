// Item: light fixtures.
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

void FarmScene::drawLightFixtures(const Shader& shader,
                                  const LightingSystem& lighting) const
{
    const auto pointEmissions = lighting.pointFixtureEmissions();
    const glm::vec3 warmBulb(1.0f, 0.78f, 0.40f);
    applyMaterial(shader, Materials::metal());
    const auto& positions = LightingSystem::pointLightPositions();
    for (size_t index = 0; index < positions.size(); ++index)
    {
        const glm::vec3 position = positions[index];
        const bool signLight = index >= 3;
        if (signLight)
        {
            // Short wall bracket projects the bulb in front of the sign so
            // the front-facing text receives real diffuse/specular lighting.
            cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.18f, -0.34f),
                                glm::vec3(0.10f, 0.10f, 0.72f), textures_.metal,
                                DarkMetal);
            cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.04f, 0.0f),
                                glm::vec3(0.62f, 0.16f, 0.62f), textures_.metal,
                                DarkMetal);
        }
        else
        {
            cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.30f, 0.0f),
                                glm::vec3(0.09f, 0.58f, 0.09f),
                                textures_.metal, glm::vec3(0.55f));
            cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.07f, 0.0f),
                                glm::vec3(0.56f, 0.18f, 0.56f),
                                textures_.metal, DarkMetal);
        }

        Material bulbMaterial = Materials::metal();
        bulbMaterial.emission = warmBulb * pointEmissions[index];
        applyMaterial(shader, bulbMaterial);
        if (signLight)
            primitives_.drawSphere(shader, glm::mat4(1.0f),
                                   position + glm::vec3(0.0f, -0.12f, 0.0f),
                                   glm::vec3(0.34f), warmBulb);
        else
            cubes_.drawColored(shader, position + glm::vec3(0.0f, -0.12f, 0.0f),
                               glm::vec3(0.34f, 0.24f, 0.34f), warmBulb);
        applyMaterial(shader, Materials::metal());
    }

    const glm::vec3 spotPosition = LightingSystem::spotlightPosition();
    const glm::mat4 spotTransform = glm::inverse(glm::lookAt(
        spotPosition, spotPosition + lighting.activeSpotlightDirection(),
        glm::vec3(0.0f, 1.0f, 0.0f)));
    cubes_.drawTextured(shader, spotTransform, glm::vec3(0.0f, 0.0f, 0.08f),
                        glm::vec3(0.18f, 0.42f, 0.52f),
                        textures_.metal, DarkMetal);
    Material spotMaterial = Materials::metal();
    spotMaterial.emission = glm::vec3(1.0f, 0.92f, 0.64f)
        * lighting.spotlightFixtureEmission();
    applyMaterial(shader, spotMaterial);
    cubes_.drawColored(shader, spotTransform, glm::vec3(0.0f, 0.0f, -0.20f),
                       glm::vec3(0.42f, 0.26f, 0.08f), glm::vec3(1.0f, 0.92f, 0.64f));
    applyMaterial(shader, Materials::metal());

}
