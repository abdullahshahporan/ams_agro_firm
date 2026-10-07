// Item: surroundings.
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

void FarmScene::drawSurroundings(const Shader& shader) const
{
    // Alternating crop fields and planted rows give the fenced plot a real
    // surrounding landscape without changing the farm's local coordinates.
    applyMaterial(shader, Materials::soil());
    for (const float side : {-1.0f, 1.0f})
    {
        for (int field = 0; field < 3; ++field)
        {
            const float z = -55.0f + field * 47.0f;
            const float x = side * 45.0f;
            cubes_.drawTextured(shader, glm::vec3(x, -0.092f, z),
                                glm::vec3(43.0f, 0.035f, 39.0f),
                                textures_.soil,
                                field == 1 ? glm::vec3(0.80f, 0.72f, 0.58f)
                                           : glm::vec3(0.66f, 0.60f, 0.43f),
                                glm::vec2(16.0f, 14.0f));
            applyMaterial(shader, Materials::grass());
            for (int row = 0; row < 9; ++row)
                cubes_.drawTextured(shader,
                                    glm::vec3(x - 18.0f + row * 4.5f,
                                              -0.066f, z),
                                    glm::vec3(0.72f, 0.025f, 37.0f),
                                    textures_.grass,
                                    field == 1 ? glm::vec3(0.48f, 0.75f, 0.30f)
                                               : glm::vec3(0.38f, 0.66f, 0.24f),
                                    glm::vec2(1.0f, 13.0f));
            applyMaterial(shader, Materials::soil());
        }
    }

    applyMaterial(shader, Materials::wood());
    for (int index = 0; index < 11; ++index)
    {
        const float z = -69.0f + index * 12.0f;
        const float xOffset = static_cast<float>(index % 3) * 2.4f;
        drawTree(shader, glm::vec3(-24.5f - xOffset, -0.10f, z), 0.8f);
        drawTree(shader, glm::vec3(24.5f + xOffset, -0.10f, z + 4.0f), 0.9f);
    }

    // Sparse, irregular groves give the sculpted horizon scale in every
    // direction. Their bases follow the distant terrain, not a floating plane.
    for (int index = 0; index < 42; ++index)
    {
        const float angle = index * 2.39996323f; // golden-angle distribution
        const float radius = 195.0f + (index % 5) * 19.0f;
        const float x = radius * cos(angle), z = radius * sin(angle);
        const float groundY = TerrainRenderer::height(x, z);
        drawTree(shader, glm::vec3(x, groundY, z), 1.15f + (index % 4) * 0.17f);
    }
}
