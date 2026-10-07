// Item: feed sack.
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

void FarmScene::drawFeedSack(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(0.78f, 1.0f, 0.50f), textures_.hay, glm::vec3(0.94f, 0.86f, 0.68f));
    cubes_.drawColored(shader, parent, glm::vec3(0.0f, 0.0f, 0.26f), glm::vec3(0.58f, 0.10f, 0.04f), glm::vec3(0.36f, 0.24f, 0.10f));
}
