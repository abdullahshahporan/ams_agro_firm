// Item: worker house.
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

void FarmScene::drawWorkerHouse(const Shader& shader, float doorAngle) const
{
    // Kept clear of the right entrance leaf's full 90-degree swing envelope.
    const glm::vec3 origin(8.0f, 0.0f, 12.0f);
    applyMaterial(shader, Materials::concrete());
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f),
                        glm::vec3(3.5f, 0.20f, 3.2f), textures_.concrete,
                        ConcreteTint, glm::vec2(3.0f));
    // A shallow threshold ramp matches workerFloor(), so feet do not sink
    // through the raised slab while crossing the doorway.
    cubes_.drawTextured(shader,glm::vec3(8.0f,.1f-.01f/sqrt(1.25f),10.2f+.005f/sqrt(1.25f)),
                        glm::vec3(1.6f,.02f,sqrt(.20f)),textures_.concrete,ConcreteTint,glm::vec2(2,1),
                        -glm::degrees(atan(.5f)),glm::vec3(1,0,0));
    applyMaterial(shader, Materials::brick());
    cubes_.drawTextured(shader, origin + glm::vec3(-1.60f, 1.60f, 0.0f),
                        glm::vec3(0.22f, 3.0f, 3.0f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(1.60f, 1.60f, 0.0f),
                        glm::vec3(0.22f, 3.0f, 3.0f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 1.60f, 1.48f),
                        glm::vec3(3.0f, 3.0f, 0.22f), textures_.brick, BrickTint);
    for (float x : {-1.15f, 1.15f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.60f, -1.48f),
                            glm::vec3(0.70f, 3.0f, 0.22f), textures_.brick, BrickTint);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.175f, -1.48f),
                        glm::vec3(1.60f, 0.15f, 0.22f), textures_.brick, BrickTint);
    applyMaterial(shader, Materials::wood());
    glm::mat4 door(1.0f);
    door = glm::translate(door, origin + glm::vec3(-0.8f, 0.0f, -1.52f));
        door = glm::rotate(door, glm::radians(doorAngle),
                           glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, door, glm::vec3(0.8f, 1.575f, 0.0f),
                        glm::vec3(1.6f, 2.95f, 0.12f), textures_.wood,
                        glm::vec3(0.38f, 0.24f, 0.12f));
    applyMaterial(shader, Materials::metal());
    cubes_.drawTextured(shader, origin + glm::vec3(-0.78f, 3.45f, 0.0f),
                        glm::vec3(1.95f, 0.18f, 3.55f), textures_.roof, RoofTint,
                        glm::vec2(2.0f), 24.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.78f, 3.45f, 0.0f),
                        glm::vec3(1.95f, 0.18f, 3.55f), textures_.roof, RoofTint,
                        glm::vec2(2.0f), -24.0f, glm::vec3(0.0f, 0.0f, 1.0f));
}
