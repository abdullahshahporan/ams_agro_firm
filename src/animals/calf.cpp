// Item: calf model and behaviour.
#include "entity_renderer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include "shared/entity_transform.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include "collision_system.h"

using namespace std;
using namespace EntityTransform;

void EntityRenderer::drawCalf(const Shader& shader, const Calf& calf) const
{
    const bool feeding = calf.routeState == Calf::RouteState::Feeding;
    const bool stationary = feeding || calf.routeState == Calf::RouteState::Sheltered
        || (calf.routeState==Calf::RouteState::Manual && !calf.manualWalking);
    const float gait = stationary ? 0.0f
                                  : sin(calf.animationTime * 10.0f) * 36.0f;
    const float bob = stationary ? 0.0f
                                 : abs(sin(calf.animationTime * 10.0f)) * 0.075f;
    const float tail = sin(calf.animationTime * 6.0f) * 24.0f;
    const float headDrop = feeding
        ? 27.0f + sin(calf.animationTime * 1.8f) * 3.0f : 0.0f;
    drawBovine(shader, makeRoot(calf.position, calf.yaw, calf.scale),
                calf.bodyColor, calf.patchColor, gait, headDrop,
                sin(calf.animationTime * 2.0f) * 4.0f,
                tail, bob, 0.0f, true, false, false);
}

void EntityRenderer::drawCalfTether(const Shader& shader, const Calf& calf) const
{
    const glm::mat4 calfRoot = makeRoot(calf.position, calf.yaw, calf.scale);
    const glm::vec3 collar = glm::vec3(
        calfRoot * glm::vec4(1.20f, 1.52f, 0.26f, 1.0f));
    const glm::vec3 anchor(-6.34f, 0.72f, calf.shelterPosition.z);
    const glm::vec3 difference = anchor - collar;
    const float length = glm::length(difference);
    if (length < 0.001f)
        return;

    const glm::vec3 direction = difference / length;
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const float cosine = glm::clamp(glm::dot(up, direction), -1.0f, 1.0f);
    glm::mat4 rope(1.0f);
    rope = glm::translate(rope, (collar + anchor) * 0.5f);
    const glm::vec3 axis = glm::cross(up, direction);
    if (glm::length(axis) > 0.0001f)
        rope = glm::rotate(rope, acos(cosine), glm::normalize(axis));
    else if (cosine < 0.0f)
        rope = glm::rotate(rope, glm::radians(180.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f));

    applyMaterial(shader, Materials::wood());
    primitives_.drawSphere(shader, glm::mat4(1.0f), collar,
                           glm::vec3(0.08f), glm::vec3(0.38f, 0.12f, 0.06f));
    cubes_.drawColored(shader, rope, glm::vec3(0.0f),
                       glm::vec3(0.045f, length, 0.045f),
                       glm::vec3(0.48f, 0.30f, 0.12f));
    cubes_.drawColored(shader, anchor, glm::vec3(0.12f),
                       glm::vec3(0.34f, 0.20f, 0.08f));
}

// Initial positions, dimensions, colours and states for this item.
void AnimationSystem::initializeCalves()
{
    calves_ = {{
          {glm::vec3(-6.75f, 0.0f, -10.30f), 90.0f, 0.60f, 1.85f, 0.0f, 0.25f,
           glm::vec3(3.0f, 0.0f, -7.0f), glm::vec2(3.0f, 1.8f), 1.0f,
           glm::vec3(0.78f, 0.48f, 0.24f), glm::vec3(0.96f, 0.84f, 0.64f),
           glm::vec3(-6.75f, 0.0f, -10.30f), glm::vec3(-6.75f, 0.0f, -10.05f),
           Calf::RouteState::Sheltered},
          {glm::vec3(-6.75f, 0.0f, -8.45f), 90.0f, 0.54f, 2.15f, 0.0f, 3.40f,
           glm::vec3(3.2f, 0.0f, 0.0f), glm::vec2(1.8f, 1.15f), -1.0f,
           glm::vec3(0.86f, 0.84f, 0.76f), glm::vec3(0.12f, 0.10f, 0.08f),
           glm::vec3(-6.75f, 0.0f, -8.45f), glm::vec3(-6.75f, 0.0f, -8.45f),
           Calf::RouteState::Sheltered}
      }};
}

void AnimationSystem::toggleCalfShed()
{
    if (nightMode_)
        return;
    if(manualCalfControl_) {resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;}

    calfDestination_ = (calfDestination_ == CalfDestination::Home)
        ? CalfDestination::Field : CalfDestination::Home;
    for (Calf& calf : calves_)
        calf.routeState = calfDestination_ == CalfDestination::Home
            ? Calf::RouteState::GoingToShelter : Calf::RouteState::GoingToField;
}

void AnimationSystem::sendCalvesToFeed()
{
    if (nightMode_)
        return;
    if(manualCalfControl_) {resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;}

    calfDestination_ = CalfDestination::FeedingArea;
    for (Calf& calf : calves_)
        calf.routeState = Calf::RouteState::GoingToFeed;
}

bool AnimationSystem::calfShedDoorNeeded() const
{
    for (const Calf& calf : calves_)
    {
        if(calf.routeState==Calf::RouteState::Manual && calf.position.x < -3.6f && abs(calf.position.z+9.62f)<2.5f) return true;
        // Keep the door open only while a calf is inside or crossing the
        // doorway envelope; close it once both animals have cleared the wall.
        if (calf.routeState == Calf::RouteState::GoingToField &&
            calf.position.x < -3.6f)
            return true;
        if ((calf.routeState == Calf::RouteState::GoingToShelter ||
             calf.routeState == Calf::RouteState::GoingToFeed) &&
            calf.position.x > -6.75f)
            return true;
    }
    return false;
}

const char* AnimationSystem::calfStatus() const
{
    if (nightMode_)
        return "inside the cattle shed beside the ox (night routine)";
    switch (calfDestination_)
    {
    case CalfDestination::Home: return "returning/staying inside the cattle shed";
    case CalfDestination::Field: return "leaving the ox-side door for the field";
    case CalfDestination::FeedingArea: return "entering the ox-side door to feed";
    }
    return "unknown";
}

void AnimationSystem::updateCalfPath(Calf& calf, float deltaTime)
{
    calf.pathAngle = fmod(
        calf.pathAngle + calf.direction * calf.speed * 0.42f * deltaTime,
        glm::two_pi<float>());
    if (calf.pathAngle < 0.0f)
        calf.pathAngle += glm::two_pi<float>();

    const float cosine = cos(calf.pathAngle);
    const float sine = sin(calf.pathAngle);
    calf.position = calf.pathCenter + glm::vec3(calf.pathRadius.x * cosine, 0.0f, calf.pathRadius.y * sine);

    glm::vec3 tangent(
        -calf.pathRadius.x * sine * calf.direction,
        0.0f,
         calf.pathRadius.y * cosine * calf.direction);
    if (glm::length2(tangent) > 0.000001f)
        calf.yaw = FarmInteraction::turnToward(calf.yaw,yawForDirection(glm::normalize(tangent)),deltaTime);
}

void AnimationSystem::updateCalfNavigation(Calf& calf, float deltaTime,
                                            CalfDestination destination)
{
    const bool insideShed = calf.position.x < -6.35f &&
                            calf.position.z > -11.25f && calf.position.z < -7.75f;
    const glm::vec3 outsideDoor(-5.20f, 0.0f, -9.62f);
    const glm::vec3 insideDoor(-6.70f, 0.0f, -9.62f);
    glm::vec3 target;
    bool finalTarget = false;

    if (destination == CalfDestination::Home)
    {
        target = insideShed ? calf.shelterPosition : insideDoor;
        finalTarget = insideShed;
    }
    else if (destination == CalfDestination::Field && insideShed)
    {
        // First align with the opening from either internal stall, then cross
        // the wall. This prevents the nearer calf from clipping a brick edge.
        target = abs(calf.position.z - insideDoor.z) > 0.045f
            ? insideDoor : outsideDoor;
    }
    else if (destination == CalfDestination::FeedingArea)
    {
        // Feeding is inside the cattle shed. From the field, approach the
        // opening in the ox-side wall instead of climbing through a trough.
        if (!insideShed)
            target = insideDoor;
        else
        {
            target = calf.feedingPosition;
            finalTarget = true;
        }
    }
    else
    {
        target = calf.pathCenter + glm::vec3(calf.pathRadius.x * cos(calf.pathAngle),
                                             0.0f,
                                             calf.pathRadius.y * sin(calf.pathAngle));
        finalTarget = true;
    }

    if (moveToward(calf.position, calf.yaw, target, calf.speed, deltaTime))
    {
        if (!finalTarget)
            return;
        if (destination == CalfDestination::Home)
        {
            calf.routeState = Calf::RouteState::Sheltered;
            calf.yaw = 90.0f;
        }
        else if (destination == CalfDestination::Field)
            calf.routeState = Calf::RouteState::Roaming;
        else if (destination == CalfDestination::FeedingArea)
        {
            calf.routeState = Calf::RouteState::Feeding;
            calf.yaw = 90.0f;
        }
    }
}

void AnimationSystem::updateCalves(float deltaTime)
{
    for (size_t index=0;index<calves_.size();++index)
    {
        Calf& calf=calves_[index];
        // R is a true calf-movement pause, including commanded navigation
        // and feeding motion rather than only the free-roaming loop.
        if (!calvesOn_)
            continue;
        if (calf.routeState == Calf::RouteState::Roaming)
        {
            const glm::vec3 previous=calf.position;
            const float previousAngle=calf.pathAngle;
            calf.animationTime += deltaTime;
            updateCalfPath(calf, deltaTime);
            if(glm::distance(glm::vec2(calf.position.x,calf.position.z),
                             glm::vec2(workers_[0].position.x,workers_[0].position.z))<.95f)
            {
                calf.position=previous;
                calf.pathAngle=previousAngle;
            }
        }
        else if (calf.routeState == Calf::RouteState::GoingToShelter ||
                 calf.routeState == Calf::RouteState::GoingToField ||
                 calf.routeState == Calf::RouteState::GoingToFeed)
        {
            // The side doorway only has room for one calf. Stagger the second
            // animal so the two bodies never overlap in the opening.
            if(index==1)
            {
                if(calf.routeState==Calf::RouteState::GoingToField &&
                   calves_[0].position.x<-4.35f)continue;
                if(calf.routeState==Calf::RouteState::GoingToShelter &&
                   calves_[0].routeState!=Calf::RouteState::Sheltered)continue;
            }
            calf.animationTime += deltaTime;
            const glm::vec3 previous=calf.position;
            updateCalfNavigation(calf, deltaTime, calfDestination_);
            if(glm::distance(glm::vec2(calf.position.x,calf.position.z),
                             glm::vec2(workers_[0].position.x,workers_[0].position.z))<.95f)
                calf.position=previous;
        }
        else if (calf.routeState == Calf::RouteState::Feeding)
            calf.animationTime += deltaTime;
    }
}

void AnimationSystem::resumeCalf(Calf& calf)
{
    calf.manualWalking=false;
    calf.routeState=calfDestination_==CalfDestination::Home?Calf::RouteState::GoingToShelter:
        (calfDestination_==CalfDestination::Field?Calf::RouteState::GoingToField:Calf::RouteState::GoingToFeed);
}
void AnimationSystem::toggleManualCalf()
{
    if(nightMode_)return;
    if(manualCalfControl_){resumeCalf(calves_[selectedCalf_]);manualCalfControl_=false;return;}
    if(manualWorkerControl_)sendWorkerHome();
    manualCalfControl_=true;calves_[selectedCalf_].routeState=Calf::RouteState::Manual;
    calves_[selectedCalf_].manualWalking=false;calvesOn_=true;
}
void AnimationSystem::selectNextCalf()
{
    if(manualCalfControl_)resumeCalf(calves_[selectedCalf_]);
    selectedCalf_=(selectedCalf_+1)%calves_.size();
    if(manualCalfControl_){calves_[selectedCalf_].routeState=Calf::RouteState::Manual;calves_[selectedCalf_].manualWalking=false;}
}
void AnimationSystem::driveManualCalf(float forward,float turn,float dt,float gate,float stall,float calfDoor)
{
    if(!manualCalfControl_||nightMode_||!calvesOn_||!isfinite(dt)||dt<=0)return;
    Calf& calf=calves_[selectedCalf_];calf.manualWalking=false;
    calf.yaw=fmod(calf.yaw+glm::clamp(turn,-1.0f,1.0f)*110.0f*dt,360.0f);
    const float angle=glm::radians(calf.yaw);
    const glm::vec3 motion=glm::vec3(cos(angle),0,-sin(angle))*glm::clamp(forward,-1.0f,1.0f)*calf.speed*min(dt,.1f);
    const int steps=max(1,static_cast<int>(ceil(glm::length(motion)/.04f)));
    const glm::vec3 start=calf.position;
    for(int i=1;i<=steps;++i)
    {
        const glm::vec3 candidate=start+motion*(static_cast<float>(i)/steps);
        if(!CollisionSystem::canOccupy(candidate+glm::vec3(0,1.2f,0),gate,stall,calfDoor,
                                       workerDoorAngle_,hayAvailableMask()))break;
        const Calf& other=calves_[(selectedCalf_+1)%calves_.size()];
        if(glm::length(glm::vec2(candidate.x-other.position.x,candidate.z-other.position.z))<.8f)break;
        calf.position=candidate;calf.manualWalking=glm::length2(calf.position-start)>.000001f;
    }
    if(calf.manualWalking)calf.animationTime+=dt;
}
