// Item: inspection drone model and emissive moving lamp.
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

void FarmScene::drawInspectionDrone(const Shader& shader,
                                    const LightingSystem& lighting) const
{
    // Inspection drone: its suspended lamp is the actual sixth point-light
    // position, so both the fixture and illuminated area travel together.
    const glm::vec3 drone = lighting.movingLightPosition();
    cubes_.drawColored(shader, drone + glm::vec3(0.0f, 0.28f, 0.0f),
                       glm::vec3(0.42f, 0.22f, 0.34f), glm::vec3(0.14f, 0.18f, 0.22f));
    for (float x : {-0.52f, 0.52f})
        for (float z : {-0.52f, 0.52f})
        {
            cubes_.drawColored(shader, drone + glm::vec3(x * 0.5f, 0.28f, z * 0.5f),
                               glm::vec3(0.76f, 0.07f, 0.09f), DarkMetal,
                               x * z > 0.0f ? -45.0f : 45.0f);
            cubes_.drawColored(shader, drone + glm::vec3(x, 0.41f, z),
                               glm::vec3(0.45f, 0.025f, 0.10f), glm::vec3(0.12f),
                               glm::degrees(lighting.movingLightPhase()) * 18.0f);
        }
    Material lamp = Materials::metal();
    lamp.emission = lighting.movingLightEnabled()
        ? glm::vec3(0.30f, 1.2f, 2.0f) : glm::vec3(0.0f);
    applyMaterial(shader, lamp);
    primitives_.drawSphere(shader, glm::mat4(1.0f), drone,
                           glm::vec3(0.28f), glm::vec3(0.30f, 0.75f, 1.0f));
    applyMaterial(shader, Materials::metal());
}
