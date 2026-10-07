// Coordinator: updates item modules and handles shared day/night/shelter state.
#include "animation_system.h"
#include <algorithm>
#include <cmath>
#include <glm/gtx/norm.hpp>

using namespace std;

AnimationSystem::AnimationSystem()
{
    initializeCows();
    initializeCalves();
    initializeWorkers();
    initializeBirds();
    for (Bird& bird : birds_)
        if (bird.mobile)
            updateBirdPath(bird, 0.0f);
}

void AnimationSystem::update(float deltaTime)
{
    updateCows(deltaTime);
    if (!nightMode_)
    {
        updateCalves(deltaTime);
        updateBirds(deltaTime);
    }
    updateWorkers(deltaTime);
    updateFans(deltaTime);
}

void AnimationSystem::setNightMode(bool nightMode)
{
    if (nightMode_ == nightMode)
        return;
    nightMode_ = nightMode;
    if (nightMode_)
    {
        calfDestination_ = CalfDestination::Home;
        for (Calf& calf : calves_)
        {
            calf.position = calf.shelterPosition;
            calf.yaw = 90.0f;
            calf.routeState = Calf::RouteState::Sheltered;
        }
        for (Bird& bird : birds_)
        {
            bird.position = bird.shelterPosition;
            bird.yaw = 180.0f;
            bird.routeState = Calf::RouteState::Sheltered;
        }
        sendWorkerHome();
    }
    else
    {
        // Calves remain safely beside the ox until J or L explicitly releases them.
        calfDestination_ = CalfDestination::Home;
        for (Calf& calf : calves_)
            calf.routeState = Calf::RouteState::Sheltered;
        for (Bird& bird : birds_)
            if (bird.mobile)
                bird.routeState = shelterRequested_
                    ? Calf::RouteState::Sheltered : Calf::RouteState::GoingToField;
    }
}

void AnimationSystem::toggleAnimalShelter()
{
    if (nightMode_)
        return;
    shelterRequested_ = !shelterRequested_;
    calfDestination_ = shelterRequested_ ? CalfDestination::Home
                                         : CalfDestination::Field;
    for (Calf& calf : calves_)
        calf.routeState = shelterRequested_
            ? Calf::RouteState::GoingToShelter : Calf::RouteState::GoingToField;
    for (Bird& bird : birds_)
        if (bird.mobile)
                bird.routeState = shelterRequested_
                    ? Calf::RouteState::GoingToShelter : Calf::RouteState::GoingToField;
}

bool AnimationSystem::moveToward(glm::vec3& position, float& yaw,
                                 const glm::vec3& target, float speed,
                                 float deltaTime)
{
    const glm::vec3 difference = target - position;
    const float distanceSquared = glm::length2(difference);
    if (distanceSquared < 0.0025f)
    {
        position = target;
        return true;
    }
    const float distance = sqrt(distanceSquared);
    const glm::vec3 direction = difference / distance;
    position += direction * min(speed * deltaTime, distance);
    yaw = yawForDirection(direction);
    return distance <= speed * deltaTime;
}

float AnimationSystem::yawForDirection(const glm::vec3& direction)
{
    // All animated models use local +X as forward.
    return glm::degrees(atan2(-direction.z, direction.x));
}
