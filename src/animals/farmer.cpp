// Item: farmer model and behaviour.
#include "entity_renderer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include "shared/entity_transform.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include "shared/farm_interaction.h"

using namespace std;
using namespace EntityTransform;

void EntityRenderer::drawWorker(const Shader& shader, const Worker& worker) const
{
    const bool walking = worker.state == WorkerState::WalkingToHay ||
                         worker.state == WorkerState::WalkingToCows ||
                         worker.state == WorkerState::WalkingHome ||
                         (worker.state == WorkerState::Manual && worker.manualWalking);
    const bool feeding = worker.state == WorkerState::Feeding;
    const bool lifting = worker.state == WorkerState::PickingUpHay;
    const bool scattering = worker.scatteringFeed;
    const bool digging = worker.digging;
    const float cycle = sin((worker.animationTime + worker.phase) * 6.0f);
    const float legSwing = cycle * 31.0f * worker.gaitBlend;
    const float workSwing = walking ? -legSwing
                                    : (digging ? 48.0f+cycle*58.0f
                                       : (scattering ? 38.0f+cycle*52.0f
                                       : ((feeding || lifting) ? 30.0f + cycle * 33.0f : 0.0f)));
    glm::mat4 root = makeRoot(worker.position, worker.yaw, 1.0f);
    if (worker.gaitBlend>0)
        root = glm::translate(root, glm::vec3(0.0f, abs(cycle) * 0.035f * worker.gaitBlend, 0.0f));

    const glm::vec3 trousers(0.12f, 0.16f, 0.22f);
    const glm::vec3 skin(0.58f, 0.34f, 0.20f);
    // Rounded volumes replace the old box torso and pelvis.  The narrower
    // waist, shoulder volume and joint spheres make the silhouette readable
    // from both the follow camera and the high establishing shots.
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 1.28f, 0.0f),
                           glm::vec3(0.66f, 0.78f, 0.43f), worker.shirtColor);
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 0.91f, 0.0f),
                           glm::vec3(0.55f, 0.30f, 0.36f), trousers);
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 1.59f, 0.0f),
                           glm::vec3(0.22f, 0.20f, 0.21f), skin);
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 1.82f, 0.0f),
                           glm::vec3(0.46f, 0.56f, 0.44f), skin);
    // The farmer faces local +X. Nose, ears, eyes and brows turn with root.
    primitives_.drawSphere(shader, root, glm::vec3(0.225f, 1.82f, 0.0f),
                           glm::vec3(0.13f, 0.14f, 0.13f), skin * 0.96f);
    for(float side:{-1.0f,1.0f})
    {
        primitives_.drawSphere(shader, root, glm::vec3(0.02f,1.84f,side*0.225f),
                               glm::vec3(0.10f,0.15f,0.07f),skin*0.92f);
        primitives_.drawSphere(shader, root, glm::vec3(0.208f,1.93f,side*0.105f),
                               glm::vec3(0.045f,0.052f,0.040f),glm::vec3(0.035f,0.026f,0.020f));
        cubes_.drawColored(shader,root,glm::vec3(0.216f,1.995f,side*0.105f),
                           glm::vec3(0.020f,0.025f,0.105f),glm::vec3(0.10f,0.055f,0.025f));
    }
    primitives_.drawSphere(shader,root,glm::vec3(0.233f,1.735f,0.0f),
                           glm::vec3(0.026f,0.018f,0.15f),glm::vec3(0.20f,0.09f,0.055f));
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 2.075f, 0.0f),
                           glm::vec3(0.98f, 0.10f, 0.98f),
                           glm::vec3(0.84f, 0.52f, 0.10f));
    primitives_.drawCone(shader, root, glm::vec3(0.0f, 2.27f, 0.0f),
                         glm::vec3(0.62f, 0.38f, 0.62f),
                         glm::vec3(0.92f, 0.66f, 0.16f));

    for (float side : {-1.0f, 1.0f})
    {
        glm::mat4 hip = glm::translate(root, glm::vec3(0.0f, 0.88f, side * 0.17f));
        hip = glm::rotate(hip, glm::radians(side * legSwing), glm::vec3(0.0f, 0.0f, 1.0f));
        drawHumanLimb(shader, hip, 0.45f, 0.45f, -side * legSwing * 0.25f,
                      trousers, trousers * 0.78f, 0.18f);

        const float armAngle = worker.carryingHay && !feeding
            ? side * 135.0f
            : (worker.holdingHose && side>0.0f ? 55.0f
               : ((side > 0.0f) ? workSwing
                                 : -workSwing * (walking ? 1.0f : 0.35f)));
        glm::mat4 shoulder = glm::translate(root, glm::vec3(0.0f, 1.52f, side * 0.38f));
        shoulder = glm::rotate(shoulder, glm::radians(armAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        drawHumanLimb(shader, shoulder, 0.36f, 0.34f,
                      feeding ? -38.0f : 0.0f, worker.shirtColor, skin, 0.14f);
    }

    if (worker.carryingHay || lifting)
    {
        // The bundle physically rises onto the head and later moves from the
        // worker to the cow's trough. It is drawn independently in world space.
        glm::vec3 bundlePosition = worker.position + glm::vec3(0.0f, 2.55f, 0.0f);
        if (lifting)
            bundlePosition = glm::mix(worker.hayPickup, bundlePosition,
                glm::smoothstep(0.0f,1.0f,min(worker.taskTimer / 0.85f,1.0f)));
        if (feeding)
            bundlePosition = glm::mix(bundlePosition,
                                      glm::vec3(-12.0f, 0.72f, -5.45f),
                                      glm::smoothstep(0.0f,1.0f,min(worker.taskTimer / 1.35f, 1.0f)));
        applyMaterial(shader, Materials::hay());
        cubes_.drawColored(shader, bundlePosition,
                           glm::vec3(0.94f, 0.37f, 0.72f),
                           glm::vec3(0.79f, 0.64f, 0.28f));
        for (float x : {-0.24f, 0.24f})
            cubes_.drawColored(shader, bundlePosition + glm::vec3(x, 0.0f, 0.0f),
                               glm::vec3(0.045f, 0.39f, 0.75f),
                               glm::vec3(0.31f, 0.20f, 0.09f));
    }

    if(worker.holdingFeedBag)
    {
        applyMaterial(shader,Materials::hay());
        cubes_.drawColored(shader,root,glm::vec3(.42f,1.18f,.42f),
                           glm::vec3(.58f,.76f,.42f),glm::vec3(.92f,.80f,.52f),-12.0f,glm::vec3(0,0,1));
        cubes_.drawColored(shader,root,glm::vec3(.43f,1.18f,.64f),
                           glm::vec3(.42f,.08f,.035f),glm::vec3(.38f,.22f,.08f));
    }
    if(worker.holdingHoe)
    {
        applyMaterial(shader,Materials::wood());
        glm::mat4 hoe=glm::translate(root,glm::vec3(.38f,1.42f,-.55f));
        const float angle=worker.digging?(-28.0f+cycle*42.0f):-10.0f;
        hoe=glm::rotate(hoe,glm::radians(angle),glm::vec3(0,0,1));
        cubes_.drawColored(shader,hoe,glm::vec3(0,-.62f,0),glm::vec3(.09f,2.05f,.09f),glm::vec3(.48f,.28f,.10f));
        cubes_.drawColored(shader,hoe,glm::vec3(0,-1.62f,0),glm::vec3(.66f,.14f,.28f),glm::vec3(.28f,.31f,.33f),-12.0f,glm::vec3(0,0,1));
    }
}

void EntityRenderer::drawHumanLimb(
    const Shader& shader, const glm::mat4& joint,
    float upperLength, float lowerLength, float bend,
    const glm::vec3& upperColor, const glm::vec3& lowerColor,
    float thickness) const
{
    primitives_.drawSphere(shader,joint,glm::vec3(0.0f,-upperLength*.48f,0.0f),
                           glm::vec3(thickness*1.15f,upperLength*1.10f,thickness*1.15f),upperColor);
    glm::mat4 elbow = glm::translate(joint, glm::vec3(0.0f, -upperLength, 0.0f));
    primitives_.drawSphere(shader,elbow,glm::vec3(0.0f),glm::vec3(thickness*1.20f),
                           glm::mix(upperColor,lowerColor,.35f));
    elbow = glm::rotate(elbow, glm::radians(bend), glm::vec3(0.0f, 0.0f, 1.0f));
    primitives_.drawSphere(shader,elbow,glm::vec3(0.0f,-lowerLength*.48f,0.0f),
                           glm::vec3(thickness*.94f,lowerLength*1.08f,thickness*.94f),lowerColor);
    primitives_.drawSphere(shader,elbow,glm::vec3(0.03f,-lowerLength,0.0f),
                           glm::vec3(thickness*1.16f,thickness*.78f,thickness*1.05f),lowerColor*.82f);
}

namespace
{
glm::vec3 workerHoseHand(const Worker& worker, const glm::vec3& position)
{
    const float angle=glm::radians(worker.yaw);
    const glm::vec3 forward(cos(angle),0.0f,-sin(angle));
    const glm::vec3 right(-forward.z,0.0f,forward.x);
    return position+forward*0.30f+right*0.34f+glm::vec3(0.0f,1.05f,0.0f);
}

int homeRouteIndex(const glm::vec3& position)
{
    if(position.z>13.7f&&position.x>6.2f&&position.x<9.8f)return -2;
    if(position.z>10.0f&&position.x>=9.8f)return -1;
    if(position.z>10.0f&&position.x<=6.2f)return -3;
    if(position.z>10.7f&&position.x>6.9f&&position.x<9.1f)return -4;
    if (position.x > 6.0f && position.z > 9.5f) return 5;
    if (position.x > 1.0f && position.z > 7.0f) return 4;
    if (position.x < -3.5f && position.z > 2.5f && position.z < 6.6f) return -5;
    if (position.z > 5.0f) return 3;
    if (position.z > 0.0f) return 2;
    if (position.z > -3.8f) return 1;
    return 0;
}
}


// Initial positions, dimensions, colours and states for this item.
void AnimationSystem::initializeWorkers()
{
    workers_ = {{
          {glm::vec3(8.0f, 0.22f, 12.70f), 90.0f, WorkerState::HomeIdle,
           2.00f, 0.0f, 0.0f,
           glm::vec3(8.0f, 0.22f, 12.70f), glm::vec3(-12.0f, 0.0f, -3.8f), true,
           glm::vec3(0.12f, 0.42f, 0.72f), false, 0, 0.0f, 0}
      }};
}

void AnimationSystem::commandWorker()
{
    if (nightMode_ || manualWorkerControl_)
        return;

    Worker& worker = workers_[0];
    switch (worker.state)
    {
    case WorkerState::HomeIdle:
        if (hayRemaining() == 0) return;
        for(int i=0;i<FarmInteraction::HayCount;++i) if(hayAvailable_[i]) {worker.hayIndex=i;break;}
        worker.carryingHay = false;
        worker.routeIndex = 0;
        worker.state = WorkerState::WalkingToHay;
        break;
    case WorkerState::ReadyAtCows:
        worker.state = WorkerState::Feeding;
        worker.taskTimer = 0.0f;
        break;
    case WorkerState::WalkingToHay:
    case WorkerState::PickingUpHay:
    case WorkerState::Feeding:
    case WorkerState::WalkingToCows:
    case WorkerState::WalkingHome:
    case WorkerState::Manual:
        break;
    }
}

void AnimationSystem::sendWorkerHome()
{
    manualWorkerControl_ = false;
    Worker& worker = workers_[0];
    worker.scatteringFeed=false;
    worker.scatteringTimer=0.0f;
    if(worker.holdingHose)
    {
        worker.holdingHose=false;
        hosePlacedAtTrough_=false;
    }
    waterValveOpen_=false;
    if(worker.state == WorkerState::PickingUpHay) returnUndeliveredHay(worker);
    if (worker.state != WorkerState::HomeIdle)
    {
        if (glm::distance(worker.position, worker.pathA) < 0.05f)
        {
            worker.position = worker.pathA;
            worker.state = WorkerState::HomeIdle;
            returnUndeliveredHay(worker);
            worker.carryingHay = false;
        }
        else
        {
            worker.state = WorkerState::WalkingHome;
            worker.routeIndex = homeRouteIndex(worker.position);
        }
    }
}

void AnimationSystem::toggleManualWorker()
{
    if(workers_[0].state == WorkerState::PickingUpHay || workers_[0].state == WorkerState::Feeding) return;
    if(manualCalfControl_) {resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;}
    manualWorkerControl_ = !manualWorkerControl_;
    Worker& worker = workers_[0];
    worker.taskTimer = 0.0f;
    if (manualWorkerControl_)
        worker.state = WorkerState::Manual;
    else
        sendWorkerHome();
}

bool AnimationSystem::interactWithWorker()
{
    if (!manualWorkerControl_ || nightMode_)
        return false;
    Worker& worker = workers_[0];
    if (worker.state != WorkerState::Manual)
        return false;
    const glm::vec2 here(worker.position.x, worker.position.z);
    if(worker.holdingHose)
    {
        worker.holdingHose=false;
        if(glm::distance(here,glm::vec2(FarmInteraction::WaterTroughInteraction.x,
                                        FarmInteraction::WaterTroughInteraction.z))<1.85f)
        {
            hosePlacedAtTrough_=true;
            hoseEndPosition_=FarmInteraction::WaterTroughNozzle;
        }
        else if(glm::distance(here,glm::vec2(FarmInteraction::ValvePosition.x,
                                              FarmInteraction::ValvePosition.z))<1.75f)
        {
            hosePlacedAtTrough_=false;
            hoseEndPosition_=FarmInteraction::HoseRest;
        }
        else
        {
            hosePlacedAtTrough_=false;
            hoseEndPosition_=workerHoseHand(worker,worker.position);
        }
        return true;
    }
    if(!worker.carryingHay &&
       glm::distance(here,glm::vec2(hoseEndPosition_.x,hoseEndPosition_.z))<1.45f)
    {
        worker.holdingHose=true;
        hosePlacedAtTrough_=false;
        hoseEndPosition_=workerHoseHand(worker,worker.position);
        return true;
    }
    if (!worker.carryingHay)
    {
        int nearest=-1;float distance=1.5f;
        for(int i=0;i<FarmInteraction::HayCount;++i)if(hayAvailable_[i])
        {
            const glm::vec3 source=FarmInteraction::hayPosition(i);
            const float candidate=glm::distance(here,glm::vec2(source.x,source.z));
            if(candidate<distance){nearest=i;distance=candidate;}
        }
        if(nearest>=0)return beginHayPickup(worker,nearest);
    }
    if (worker.carryingHay &&
             glm::distance(here, glm::vec2(-12.0f, -3.8f)) < 1.6f)
    {
        worker.state = WorkerState::Feeding;
        worker.taskTimer = 0.0f;
        return true;
    }
    if(!worker.carryingHay &&
       glm::distance(here,glm::vec2(FarmInteraction::PoultryFeedPosition.x,
                                    FarmInteraction::PoultryFeedPosition.z))<1.75f)
        return scatterPoultryFeed();
    return false;
}

bool AnimationSystem::scatterPoultryFeed()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||nightMode_||worker.state!=WorkerState::Manual||
       worker.carryingHay||worker.holdingHose)
        return false;
    const glm::vec2 here(worker.position.x,worker.position.z);
    const glm::vec2 feedingArea(FarmInteraction::PoultryFeedPosition.x,
                                FarmInteraction::PoultryFeedPosition.z);
    if(glm::distance(here,feedingArea)>=1.75f)return false;
    worker.scatteringFeed=true;
    worker.scatteringTimer=2.2f;
    worker.holdingFeedBag=false;
    poultryFeedLevel_=1.0f;
    return true;
}

bool AnimationSystem::pickupFeedBag()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||nightMode_||worker.state!=WorkerState::Manual||
       !feedBagAvailable_||worker.carryingHay||worker.holdingHose||worker.holdingHoe)return false;
    if(glm::distance(glm::vec2(worker.position.x,worker.position.z),
       glm::vec2(FarmInteraction::FeedStorePosition.x,FarmInteraction::FeedStorePosition.z))>1.55f)return false;
    feedBagAvailable_=false;
    worker.holdingFeedBag=true;
    return true;
}

bool AnimationSystem::pickupHoe()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||nightMode_||worker.state!=WorkerState::Manual||
       !hoeAvailable_||worker.carryingHay||worker.holdingHose||worker.holdingFeedBag)return false;
    if(glm::distance(glm::vec2(worker.position.x,worker.position.z),
       glm::vec2(FarmInteraction::HoeStorePosition.x,FarmInteraction::HoeStorePosition.z))>1.55f)return false;
    hoeAvailable_=false;
    worker.holdingHoe=true;
    return true;
}

bool AnimationSystem::beginHoeWork()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||nightMode_||worker.state!=WorkerState::Manual||!worker.holdingHoe)return false;
    if(glm::distance(glm::vec2(worker.position.x,worker.position.z),
       glm::vec2(FarmInteraction::HoeWorkPosition.x,FarmInteraction::HoeWorkPosition.z))>1.5f)return false;
    worker.digging=true;
    worker.diggingTimer=4.5f;
    return true;
}

bool AnimationSystem::returnHoe()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||worker.state!=WorkerState::Manual||!worker.holdingHoe)return false;
    if(glm::distance(glm::vec2(worker.position.x,worker.position.z),
       glm::vec2(FarmInteraction::HoeStorePosition.x,FarmInteraction::HoeStorePosition.z))>1.55f)return false;
    worker.holdingHoe=false;
    worker.digging=false;
    worker.diggingTimer=0;
    hoeAvailable_=true;
    return true;
}

bool AnimationSystem::toggleWaterValve()
{
    Worker& worker=workers_[0];
    if(!manualWorkerControl_||nightMode_||worker.state!=WorkerState::Manual||worker.carryingHay)
        return false;
    const glm::vec2 here(worker.position.x,worker.position.z);
    const glm::vec2 valve(FarmInteraction::ValvePosition.x,FarmInteraction::ValvePosition.z);
    if(glm::distance(here,valve)>=1.65f)
        return false;
    waterValveOpen_=!waterValveOpen_;
    return true;
}

void AnimationSystem::turnManualWorker(float angleDegrees)
{
    if (manualWorkerControl_ && workers_[0].state==WorkerState::Manual)
        workers_[0].yaw = fmod(workers_[0].yaw+angleDegrees,360.0f);
}

void AnimationSystem::moveManualWorker(const glm::vec3& position, float deltaTime)
{
    if (!manualWorkerControl_ || workers_[0].state != WorkerState::Manual)
        return;
    if(!isfinite(position.x)||!isfinite(position.y)||!isfinite(position.z)||!isfinite(deltaTime))return;
    if(workers_[0].holdingHose &&
       glm::distance(workerHoseHand(workers_[0],position),FarmInteraction::HoseAnchor)>
           FarmInteraction::HoseMaximumLength)
        return;
    workers_[0].manualWalking = glm::distance(position, workers_[0].position) > 0.001f;
    workers_[0].position = position;
    workers_[0].position.y = FarmInteraction::workerFloor(position.x,position.z);
    workers_[0].animationTime += deltaTime;
}

const char* AnimationSystem::workerStatus() const
{
    switch (workers_[0].state)
    {
    case WorkerState::HomeIdle: return hayRemaining() ? "resting inside the worker house" : "no hay left - press F9 to restock";
    case WorkerState::WalkingToHay: return "leaving the house for the hay stack";
    case WorkerState::PickingUpHay: return "lifting hay onto his head";
    case WorkerState::WalkingToCows: return "carrying hay to the cattle";
    case WorkerState::ReadyAtCows: return "ready beside the cattle trough";
    case WorkerState::Feeding: return "placing hay in front of the cows";
    case WorkerState::WalkingHome: return "walking home";
    case WorkerState::Manual:
        if(workers_[0].carryingHay) return "under your control, carrying hay";
        if(workers_[0].holdingHose) return "under your control, pulling the water hose";
        if(workers_[0].holdingFeedBag) return "carrying poultry feed from the instrument room";
        if(workers_[0].digging) return "working the soil with the hoe";
        if(workers_[0].holdingHoe) return "carrying the hoe";
        if(workers_[0].scatteringFeed) return "scattering grain for the moving poultry";
        return "under your control";
    }
    return "unknown";
}

void AnimationSystem::updateWorker(Worker& worker, float deltaTime)
{
    worker.waitingDoor=false;
    if ((worker.position.z > 9.6f && worker.position.x > 7.3f && worker.position.x < 8.7f)
        && (worker.state==WorkerState::WalkingToHay || worker.state==WorkerState::WalkingHome)
        && abs(workerDoorAngle_) < 85.0f) {worker.waitingDoor=true;return;}
    // The route skirts the poultry shed, the hay stacks and the cattle shed
    // walls. Its door and pickup positions match the visible scene objects.
    const array<glm::vec3, 6> toHay = {{
        {8.0f, 0.0f, 9.8f}, {4.6f, 0.0f, 9.8f},
        {0.0f, 0.0f, 7.2f}, {-2.95f, 0.0f, 6.8f},
        {-2.95f, 0.0f, 6.1f}, {-2.95f, 0.0f, 3.4f}
    }};
    const array<glm::vec3, 3> toCows = {{
        {-2.95f, 0.0f, 0.0f}, {-3.9f, 0.0f, -3.8f},
        {-12.0f, 0.0f, -3.8f}
    }};
    const array<glm::vec3, 8> toHome = {{
        {-3.9f, 0.0f, -3.8f}, {-2.95f, 0.0f, 0.0f},
        {-2.95f, 0.0f, 6.8f}, {0.0f, 0.0f, 7.2f},
        {4.6f, 0.0f, 9.8f}, {8.0f, 0.0f, 9.8f},
        {8.0f, 0.0f, 10.5f}, worker.pathA
    }};

    glm::vec3 target;
    int count = 0;
    if (worker.state == WorkerState::WalkingToHay)
    {
        count = static_cast<int>(toHay.size());
        target = toHay[worker.routeIndex];
        if(worker.routeIndex==static_cast<int>(toHay.size())-1) {
            target=FarmInteraction::hayApproachPosition(worker.hayIndex);
        }
    }
    else if (worker.state == WorkerState::WalkingToCows)
    {
        count = static_cast<int>(toCows.size());
        target = toCows[worker.routeIndex];
    }
    else if(worker.routeIndex<0)
    {
        count=static_cast<int>(toHome.size());
        if(worker.routeIndex==-2)target=glm::vec3(10.5f,0,14.2f);
        else if(worker.routeIndex==-1)target=glm::vec3(10.5f,0,9.8f);
        else if(worker.routeIndex==-3)target=glm::vec3(5.7f,0,9.8f);
        else if(worker.routeIndex==-5)target=glm::vec3(-5.2f,0,6.8f);
        else target=worker.pathA;
    }
    else
    {
        count = static_cast<int>(toHome.size());
        target = toHome[worker.routeIndex];
    }

    glm::vec3 difference = target - worker.position;
    difference.y=0;
    const float distance = glm::length(difference);
    if (distance > 0.001f)
    {
        const glm::vec3 direction = difference / distance;
        worker.position += direction * min(worker.speed * deltaTime, distance);
        worker.position.y=FarmInteraction::workerFloor(worker.position.x,worker.position.z);
        worker.yaw = FarmInteraction::turnToward(worker.yaw,yawForDirection(direction),deltaTime);
    }
    if (glm::length(glm::vec2(worker.position.x-target.x,worker.position.z-target.z)) > 0.015f)
        return;

    worker.position = target;
    worker.position.y=FarmInteraction::workerFloor(target.x,target.z);
    if(worker.routeIndex<0)
    {
        if(worker.routeIndex==-2)worker.routeIndex=-1;
        else if(worker.routeIndex==-5)worker.routeIndex=2;
        else worker.routeIndex=5;
        return;
    }
    ++worker.routeIndex;
    if (worker.routeIndex < count)
        return;
    worker.routeIndex = 0;
    if (worker.state == WorkerState::WalkingToHay)
    {
        if(!beginHayPickup(worker,worker.hayIndex)) {worker.state=WorkerState::WalkingHome;worker.routeIndex=homeRouteIndex(worker.position);}
    }
    else if (worker.state == WorkerState::WalkingToCows)
    {
        worker.state = WorkerState::Feeding;
        worker.taskTimer = 0.0f;
        worker.yaw = 90.0f;
    }
    else
    {
        worker.state = WorkerState::HomeIdle;
        returnUndeliveredHay(worker);
        worker.carryingHay = false;
        worker.yaw = 90.0f;
    }
}

void AnimationSystem::updateWorkers(float deltaTime)
{
    // Inward swing leaves the outside westbound route clear. Resting farther
    // inside the room also keeps the farmer outside the closing leaf's arc.
    const float target=workers_[0].state==WorkerState::HomeIdle?0.0f:-90.0f;
    workerDoorAngle_ += glm::clamp(target-workerDoorAngle_,-180.0f*deltaTime,180.0f*deltaTime);
    const float valveTarget=waterValveOpen_?110.0f:0.0f;
    waterValveAngle_+=glm::clamp(valveTarget-waterValveAngle_,-260.0f*deltaTime,260.0f*deltaTime);
    for (Worker& worker : workers_)
    {
        if (worker.state == WorkerState::WalkingToHay ||
            worker.state == WorkerState::WalkingToCows ||
            worker.state == WorkerState::WalkingHome)
        {
            worker.animationTime += deltaTime;
            updateWorker(worker, deltaTime);
        }
        else if (worker.state == WorkerState::PickingUpHay ||
                 worker.state == WorkerState::Feeding)
        {
            worker.animationTime += deltaTime;
            worker.taskTimer += deltaTime;
            if (worker.state == WorkerState::PickingUpHay && worker.taskTimer >= 0.85f)
            {
                worker.carryingHay = true;
                worker.state = manualWorkerControl_ ? WorkerState::Manual
                                                    : WorkerState::WalkingToCows;
                worker.routeIndex = 0;
                worker.taskTimer = 0.0f;
            }
            else if (worker.state == WorkerState::Feeding && worker.taskTimer >= 1.35f)
            {
                worker.carryingHay = false;
                ++worker.deliveredLoads;
                worker.hayIndex=-1; // Consumed stock remains unavailable after feeding.
                worker.taskTimer = 0.0f;
                worker.routeIndex = 0;
                worker.state = manualWorkerControl_ ? WorkerState::Manual
                                                    : WorkerState::WalkingHome;
            }
        }
        const bool moving=((worker.state==WorkerState::WalkingToHay||worker.state==WorkerState::WalkingToCows||worker.state==WorkerState::WalkingHome)
            && !worker.waitingDoor)||(worker.state==WorkerState::Manual&&worker.manualWalking);
        worker.gaitBlend += ((moving?1.0f:0.0f)-worker.gaitBlend)*(1.0f-exp(-10.0f*deltaTime));
        if(worker.holdingHose)
            hoseEndPosition_=workerHoseHand(worker,worker.position);
        if(worker.scatteringTimer>0.0f)
        {
            worker.scatteringTimer=max(0.0f,worker.scatteringTimer-deltaTime);
            worker.scatteringFeed=worker.scatteringTimer>0.0f;
            worker.animationTime+=deltaTime;
        }
        if(worker.diggingTimer>0.0f)
        {
            worker.diggingTimer=max(0.0f,worker.diggingTimer-deltaTime);
            worker.digging=worker.diggingTimer>0.0f;
            worker.animationTime+=deltaTime;
        }
    }
    if(waterValveOpen_ && hosePlacedAtTrough_)
        waterTroughFill_=min(1.0f,waterTroughFill_+0.12f*deltaTime);
}

const char* AnimationSystem::waterStatus() const
{
    if(waterTroughFill_>=0.999f) return "water trough full";
    if(waterValveOpen_&&hosePlacedAtTrough_) return "filling the water trough";
    if(workers_[0].holdingHose) return waterValveOpen_?"hose in hand - water flowing":"hose in hand - valve closed";
    if(hosePlacedAtTrough_) return waterValveOpen_?"hose placed at trough":"hose at trough - valve closed";
    return waterValveOpen_?"valve open - pick up the hose":"valve closed";
}

unsigned int AnimationSystem::hayAvailableMask() const
{
    unsigned int mask=0;for(int i=0;i<FarmInteraction::HayCount;++i)if(hayAvailable_[i])mask|=1u<<i;return mask;
}
int AnimationSystem::hayRemaining() const
{int count=0;for(bool available:hayAvailable_)if(available)++count;return count;}
bool AnimationSystem::beginHayPickup(Worker& worker,int index)
{
    if(index<0||index>=FarmInteraction::HayCount||!hayAvailable_[index])return false;
    hayAvailable_[index]=false;worker.hayIndex=index;worker.hayPickup=FarmInteraction::hayPosition(index);
    worker.carryingHay=false;worker.state=WorkerState::PickingUpHay;worker.taskTimer=0;return true;
}
void AnimationSystem::returnUndeliveredHay(Worker& worker)
{
    if(worker.hayIndex>=0&&worker.hayIndex<FarmInteraction::HayCount)hayAvailable_[worker.hayIndex]=true;
    worker.hayIndex=-1;worker.carryingHay=false;
}
bool AnimationSystem::restockHay()
{
    if(workers_[0].state!=WorkerState::HomeIdle)return false;
    hayAvailable_.fill(true);return true;
}
