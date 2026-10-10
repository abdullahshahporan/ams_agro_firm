// Item: hay stack.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include "shared/farm_interaction.h"

using namespace std;
using namespace FarmStyle;

void FarmScene::drawHayArea(const Shader& shader, unsigned int hayAvailableMask,
                            float workProgress) const
{
    drawHayBale(shader, glm::vec3(-10.2f, 0.52f, 2.0f), -6.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 2.2f), 4.0f);
    drawHayBale(shader, glm::vec3(-5.9f, 0.52f, 2.0f), -3.0f);
    drawHayBale(shader, glm::vec3(-9.1f, 1.48f, 2.1f), 3.0f);
    // This top bundle is physically pulled down by the hoe. It remains in
    // its new ground-level pile after the action, including afternoon shots.
    const float pull=glm::smoothstep(.12f,.85f,glm::clamp(workProgress,0.0f,1.0f));
    const glm::vec3 top(-5.9f,1.48f,2.0f),pile(-5.65f,.52f,5.45f);
    glm::vec3 moving=glm::mix(top,pile,pull);
    moving.y+=sin(pull*glm::pi<float>())*.18f;
    drawHayBale(shader,moving,-5.0f+pull*18.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 4.0f), 8.0f);
    drawHayBale(shader, glm::vec3(-6.0f, 0.52f, 4.1f), -7.0f);
    for(int i=0;i<FarmInteraction::HayCount;++i) if(hayAvailableMask & (1u<<i))
    {
        const glm::vec3 position=FarmInteraction::hayPosition(i);
        cubes_.drawColored(shader, position,
                            glm::vec3(0.94f, 0.37f, 0.72f),
                            glm::vec3(0.79f,0.64f,0.28f));
        for(float x:{-.24f,.24f})cubes_.drawColored(shader,position+glm::vec3(x,0,0),glm::vec3(.045f,.39f,.75f),glm::vec3(.31f,.20f,.09f));
    }
}

void FarmScene::drawHayBale(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(1.9f, 0.90f, 1.15f), textures_.hay, HayTint, glm::vec2(2.0f, 1.0f));
    cubes_.drawColored(shader, parent, glm::vec3(-0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
    cubes_.drawColored(shader, parent, glm::vec3(0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
}
