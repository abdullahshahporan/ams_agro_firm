#ifndef AMS_AGRO_FARM_COW_H
#define AMS_AGRO_FARM_COW_H

#include <glm/glm.hpp>

enum class CowState
{
    Standing,
    Walking,
    Grazing,
    Feeding
};

enum class CattleRole
{
    MilchCow,
    Ox
};

struct Cow
{
    glm::vec3 position;
    float yaw;
    float scale;
    CowState state;
    float speed;
    float animationTime;
    float headAnimationTime;
    float phase;
    glm::vec3 pathA;
    glm::vec3 pathB;
    bool targetingB;
    glm::vec3 bodyColor;
    glm::vec3 patchColor;
    float hornScale;
    bool tied;
    glm::vec3 tetherAnchor;
    CattleRole role;
};

#endif
