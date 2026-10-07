#ifndef AMS_AGRO_FARM_POULTRY_H
#define AMS_AGRO_FARM_POULTRY_H

#include <glm/glm.hpp>
#include "calf.h"

enum class BirdSpecies
{
    Chicken,
    Duck
};

struct Bird
{
    BirdSpecies species;
    glm::vec3 position;
    glm::vec3 shelterPosition;
    glm::vec3 pathCenter;
    glm::vec2 pathRadius;
    float pathAngle;
    float direction;
    float speed;
    float scale;
    float yaw;
    float animationTime;
    bool mobile;
    bool juvenile;
    glm::vec3 color;
    Calf::RouteState routeState;
};

#endif
