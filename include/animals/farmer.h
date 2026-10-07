#ifndef AMS_AGRO_FARM_FARMER_H
#define AMS_AGRO_FARM_FARMER_H

#include <glm/glm.hpp>

enum class WorkerState
{
    HomeIdle,
    WalkingToHay,
    PickingUpHay,
    WalkingToCows,
    ReadyAtCows,
    Feeding,
    WalkingHome,
    Manual
};

struct Worker
{
    glm::vec3 position;
    float yaw;
    WorkerState state;
    float speed;
    float animationTime;
    float phase;
    glm::vec3 pathA;
    glm::vec3 pathB;
    bool targetingB;
    glm::vec3 shirtColor;
    bool carryingHay;
    int deliveredLoads;
    float taskTimer;
    int routeIndex;
    bool manualWalking{false};
};

#endif
