#ifndef AMS_AGRO_FARM_STYLE_H
#define AMS_AGRO_FARM_STYLE_H

#include <glm/glm.hpp>

// Shared colours and entrance dimensions; object coordinates live in src/items/.
namespace FarmStyle
{
constexpr float EntranceZ = 15.5f;
constexpr float LeftHingeX = -3.82f;
constexpr float RightHingeX = 3.82f;

const glm::vec3 WarmWood(0.88f, 0.77f, 0.62f);
const glm::vec3 DarkMetal(0.70f, 0.74f, 0.75f);
const glm::vec3 BrickTint(0.88f, 0.72f, 0.62f);
const glm::vec3 RoofTint(0.68f, 0.74f, 0.78f);
const glm::vec3 ConcreteTint(0.88f, 0.88f, 0.86f);
const glm::vec3 HayTint(1.00f, 0.84f, 0.48f);
const glm::vec3 LeafTint(0.68f, 0.92f, 0.62f);
}

#endif
