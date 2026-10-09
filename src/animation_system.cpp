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
    if(!isfinite(deltaTime)||deltaTime<=0)return;
    if(deltaTime>.025f)
    {
        float remaining=min(deltaTime,.25f);
        while(remaining>0.000001f){float step=min(remaining,1.0f/120.0f);update(step);remaining-=step;}
        return;
    }
    updateCows(deltaTime);
    if (!nightMode_)
    {
        updateCalves(deltaTime);
        updateBirds(deltaTime);
    }
    updateWorkers(deltaTime);
    updateFans(deltaTime);
    const bool poultryHome=all_of(birds_.begin(),birds_.end(),[](const Bird& bird)
    {
        return !bird.mobile||bird.routeState==Calf::RouteState::Sheltered;
    });
    const float poultryDoorTarget=(!shelterRequested_||!poultryHome)?92.0f:0.0f;
    poultryDoorAngle_+=glm::clamp(poultryDoorTarget-poultryDoorAngle_,
                                  -150.0f*deltaTime,150.0f*deltaTime);
    // Grain is visibly consumed before the afternoon return-to-shelter shot.
    poultryFeedLevel_=max(0.0f,poultryFeedLevel_-0.030f*deltaTime);
}

void AnimationSystem::setNightMode(bool nightMode)
{
    if (nightMode_ == nightMode)
        return;
    nightMode_ = nightMode;
    if (nightMode_)
    {
        manualCalfControl_=false;
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
    if(manualCalfControl_){resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;}
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

void AnimationSystem::sendCalvesHome()
{
    if(nightMode_)return;
    if(manualCalfControl_){resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;}
    calfDestination_=CalfDestination::Home;
    for(Calf& calf:calves_)
        calf.routeState=Calf::RouteState::GoingToShelter;
}

void AnimationSystem::sendPoultryHome()
{
    if(nightMode_)return;
    shelterRequested_=true;
    for(Bird& bird:birds_)
        if(bird.mobile)
        {
            bird.routeState=Calf::RouteState::GoingToShelter;
            bird.navigationStage=0;
        }
}

void AnimationSystem::preparePoultrySheltered()
{
    shelterRequested_=true;
    for(Bird& bird:birds_)
    {
        if(!bird.mobile)continue;
        bird.position=bird.shelterPosition;
        bird.yaw=180.0f;
        bird.routeState=Calf::RouteState::Sheltered;
        bird.navigationStage=0;
    }
}

void AnimationSystem::sendPoultryOut()
{
    if(nightMode_)return;
    shelterRequested_=false;
    for(Bird& bird:birds_)
        if(bird.mobile)
        {
            bird.routeState=Calf::RouteState::GoingToField;
            bird.navigationStage=0;
        }
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
    yaw = FarmInteraction::turnToward(yaw,yawForDirection(direction),deltaTime);
    return distance <= speed * deltaTime;
}

float AnimationSystem::yawForDirection(const glm::vec3& direction)
{
    // All animated models use local +X as forward.
    return glm::degrees(atan2(-direction.z, direction.x));
}
