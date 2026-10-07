// Item: owner billboard.
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

void FarmScene::drawBillboard(const Shader& shader) const
{
    const glm::vec3 center(-10.0f, 3.45f, 10.7f);
    constexpr float halfWidth = 5.25f;
    constexpr float curveDepth = 0.42f;
    constexpr int panelCount = 9;
    constexpr float panelWidth = (halfWidth * 2.0f) / panelCount;
    applyMaterial(shader, Materials::wood());
    for (float x : {-14.05f, -5.95f})
    {
        cubes_.drawTextured(shader, glm::vec3(x, 2.05f, center.z),
                            glm::vec3(0.30f, 4.10f, 0.30f), textures_.wood,
                            glm::vec3(0.62f, 0.42f, 0.22f), glm::vec2(0.7f, 4.0f));
        cubes_.drawTextured(shader, glm::vec3(x, 0.20f, center.z),
                            glm::vec3(0.70f, 0.24f, 0.70f), textures_.concrete,
                            ConcreteTint);
    }
    // Nine tangent-aligned panels form a gentle convex billboard. The shallow
    // curve keeps both text lines readable from the entrance and side angles.
    for (int panel = 0; panel < panelCount; ++panel)
    {
        const float localX = -halfWidth +
            (static_cast<float>(panel) + 0.5f) * panelWidth;
        const float normalized = localX / halfWidth;
        const float localZ = curveDepth * (1.0f - normalized * normalized);
        const float slope = -2.0f * curveDepth * localX / (halfWidth * halfWidth);
        const float angle = -glm::degrees(atan(slope));
        const glm::vec3 panelCenter = center + glm::vec3(localX, 0.0f, localZ);

        cubes_.drawTextured(shader, panelCenter,
                            glm::vec3(panelWidth + 0.035f, 3.10f, 0.30f),
                            textures_.wood, glm::vec3(0.34f, 0.22f, 0.12f),
                            glm::vec2(1.0f, 2.0f), angle,
                            glm::vec3(0.0f, 1.0f, 0.0f));
        for (float yOffset : {-1.48f, 1.48f})
            cubes_.drawTextured(shader,
                                panelCenter + glm::vec3(0.0f, yOffset, 0.0f),
                                glm::vec3(panelWidth + 0.04f, 0.14f, 0.36f),
                                textures_.wood, glm::vec3(0.64f, 0.43f, 0.20f),
                                glm::vec2(1.0f), angle,
                                glm::vec3(0.0f, 1.0f, 0.0f));
    }
    drawCurvedBlockText(shader, "AMS AGRO FARM",
                        center + glm::vec3(0.0f, 0.62f, 0.0f), 1.20f,
                        halfWidth, curveDepth, 0.65f);
    drawCurvedBlockText(shader, "OWNER: MD. SHAHPORAN",
                        center + glm::vec3(0.0f, -0.58f, 0.0f), 0.72f,
                        halfWidth, curveDepth, 0.65f);
}
