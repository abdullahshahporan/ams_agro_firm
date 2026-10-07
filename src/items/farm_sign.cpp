// Item: farm sign.
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

void FarmScene::drawFarmSign(const Shader& shader) const
{
    cubes_.drawTextured(shader, glm::vec3(0.0f, 4.72f, EntranceZ), glm::vec3(8.0f, 1.20f, 0.32f), textures_.wood, WarmWood, glm::vec2(4.0f, 1.0f));
    cubes_.drawTextured(shader, glm::vec3(0.0f, 4.72f, EntranceZ + 0.18f), glm::vec3(7.70f, 0.90f, 0.08f), textures_.wood, glm::vec3(0.64f, 0.50f, 0.32f), glm::vec2(3.0f, 1.0f));
    drawBlockText(shader, "AMS AGRO FARM", glm::vec3(0.0f, 4.72f, EntranceZ + 0.245f));
}
