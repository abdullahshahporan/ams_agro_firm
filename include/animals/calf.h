#ifndef AMS_AGRO_FARM_CALF_H
#define AMS_AGRO_FARM_CALF_H

#include <glm/glm.hpp>

struct Calf
{
    glm::vec3 position;
    float yaw;
    float scale;
    float speed;
    float animationTime;
    float pathAngle;
    glm::vec3 pathCenter;
    glm::vec2 pathRadius;
    float direction;
    glm::vec3 bodyColor;
    glm::vec3 patchColor;
    glm::vec3 shelterPosition;
    glm::vec3 feedingPosition;
    enum class RouteState
    {
        Roaming,
        GoingToShelter,
        Sheltered,
        GoingToField,
        GoingToFeed,
        Feeding
    } routeState;
};

enum class CalfDestination
{
    Home,
    Field,
    FeedingArea
};

#endif
