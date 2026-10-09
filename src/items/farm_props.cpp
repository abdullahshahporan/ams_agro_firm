// Item: farm props.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "shared/farm_interaction.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;
using namespace FarmStyle;

void FarmScene::drawFarmProps(const Shader& shader) const
{
    // Small cultivated work patch used by the farmer's visible hoe routine.
    const glm::vec3 work=FarmInteraction::HoeWorkPosition;
    cubes_.drawTextured(shader,work+glm::vec3(0,.035f,0),glm::vec3(2.4f,.07f,1.75f),
                        textures_.soil,glm::vec3(.72f,.55f,.34f),glm::vec2(2.0f));
    for(float offset:{-.80f,-.40f,0.0f,.40f,.80f})
        cubes_.drawColored(shader,work+glm::vec3(offset,.085f,0),glm::vec3(.07f,.03f,1.55f),
                           glm::vec3(.34f,.19f,.08f));

    drawCrate(shader, glm::vec3(7.1f, 0.55f, -7.3f), -7.0f);
    drawCrate(shader, glm::vec3(8.5f, 0.55f, -7.0f), 5.0f);
    drawCrate(shader, glm::vec3(7.8f, 1.62f, -7.2f), 2.0f);
    // Keep the barn doorway visually organized and its center lane unobstructed.
    drawFeedSack(shader, glm::vec3(12.95f, 0.55f, -7.15f), -8.0f);
    drawFeedSack(shader, glm::vec3(13.90f, 0.55f, -7.10f), 7.0f);
    drawFeedSack(shader, glm::vec3(13.42f, 1.45f, -7.12f), 1.0f);

    // Tool rack near the barn and an empty collection pad reserved for Module 5.
    cubes_.drawTextured(shader, glm::vec3(15.2f, 1.2f, -6.9f), glm::vec3(0.18f, 2.4f, 0.18f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(16.2f, 1.2f, -6.9f), glm::vec3(0.18f, 2.4f, 0.18f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(15.7f, 1.65f, -6.9f), glm::vec3(1.25f, 0.16f, 0.16f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(8.3f, 0.06f, -1.0f), glm::vec3(3.4f, 0.12f, 2.6f), textures_.concrete, ConcreteTint, glm::vec2(2.0f));
}
