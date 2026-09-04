#include "animation_system.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>

AnimationSystem::AnimationSystem()
    : cows_{{
          {glm::vec3(-12.0f, 0.0f, -8.0f), -90.0f, 0.84f, CowState::Feeding,
           0.0f, 0.0f, 1.6f, 1.6f,
           glm::vec3(-12.0f, 0.0f, -8.0f), glm::vec3(-12.0f, 0.0f, -8.0f), false,
           glm::vec3(0.96f, 0.95f, 0.90f), glm::vec3(0.025f, 0.022f, 0.020f), 0.48f,
           true, glm::vec3(-12.0f, 1.05f, -6.10f), CattleRole::MilchCow},
          {glm::vec3(-8.0f, 0.0f, -8.0f), -90.0f, 0.96f, CowState::Standing,
           0.0f, 0.0f, 2.4f, 2.4f,
           glm::vec3(-8.0f, 0.0f, -8.0f), glm::vec3(-8.0f, 0.0f, -8.0f), false,
           glm::vec3(0.62f, 0.255f, 0.095f), glm::vec3(0.20f, 0.075f, 0.035f), 0.58f,
           true, glm::vec3(-8.0f, 1.05f, -6.10f), CattleRole::Ox}
      }},
      calves_{{
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
      }},
      workers_{{
          {glm::vec3(8.0f, 0.0f, 10.25f), 90.0f, WorkerState::HomeIdle,
           2.00f, 0.0f, 0.0f,
           glm::vec3(8.0f, 0.0f, 10.25f), glm::vec3(-10.0f, 0.0f, -4.15f), true,
           glm::vec3(0.12f, 0.42f, 0.72f)}
      }},
      birds_{{
          {BirdSpecies::Chicken, glm::vec3(0.0f), glm::vec3(9.20f,0.0f,6.70f),
           glm::vec3(4.0f,0.0f,7.0f), glm::vec2(2.4f,1.5f), 0.2f, 1.0f, 1.20f, 0.48f, 0.0f, 0.0f, true, false, glm::vec3(0.72f,0.24f,0.10f), Calf::RouteState::Roaming},
          {BirdSpecies::Chicken, glm::vec3(0.0f), glm::vec3(9.65f,0.0f,6.75f),
           glm::vec3(3.5f,0.0f,8.6f), glm::vec2(2.0f,1.0f), 2.8f, -1.0f, 1.05f, 0.44f, 0.0f, 0.5f, true, false, glm::vec3(0.92f,0.78f,0.42f), Calf::RouteState::Roaming},
          {BirdSpecies::Duck, glm::vec3(0.0f), glm::vec3(10.15f,0.0f,6.70f),
           glm::vec3(6.0f,0.0f,5.5f), glm::vec2(1.6f,1.1f), 1.4f, 1.0f, 0.82f, 0.52f, 0.0f, 1.0f, true, false, glm::vec3(0.88f,0.86f,0.68f), Calf::RouteState::Roaming},
          {BirdSpecies::Duck, glm::vec3(0.0f), glm::vec3(10.60f,0.0f,6.75f),
           glm::vec3(6.0f,0.0f,8.2f), glm::vec2(1.5f,0.9f), 4.2f, -1.0f, 0.76f, 0.49f, 0.0f, 1.5f, true, false, glm::vec3(0.36f,0.42f,0.30f), Calf::RouteState::Roaming},
          {BirdSpecies::Chicken, glm::vec3(10.35f,0.0f,7.35f), glm::vec3(10.35f,0.0f,7.35f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.50f, 180.0f, 0.0f, false, false, glm::vec3(0.62f,0.20f,0.08f), Calf::RouteState::Sheltered},
          {BirdSpecies::Chicken, glm::vec3(9.95f,0.0f,7.05f), glm::vec3(9.95f,0.0f,7.05f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.22f, 40.0f, 0.0f, false, true, glm::vec3(0.96f,0.82f,0.20f), Calf::RouteState::Sheltered},
          {BirdSpecies::Chicken, glm::vec3(10.28f,0.0f,6.98f), glm::vec3(10.28f,0.0f,6.98f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.20f, -20.0f, 0.0f, false, true, glm::vec3(0.98f,0.86f,0.24f), Calf::RouteState::Sheltered}
      }}
{
    for (Bird& bird : birds_)
        if (bird.mobile)
            updateBirdPath(bird, 0.0f);
}

void AnimationSystem::update(float deltaTime)
{
    if (!nightMode_ && adultCowsOn_)
    {
        for (Cow& cow : cows_)
        {
            cow.animationTime += deltaTime;
            if (cow.state == CowState::Walking)
                updateCowPatrol(cow, deltaTime);
        }
    }

    if (!nightMode_ && headMotionOn_)
    {
        for (Cow& cow : cows_)
            cow.headAnimationTime += deltaTime;
    }

    if (!nightMode_)
    {
        for (Calf& calf : calves_)
        {
            // R is a true calf-movement pause, including commanded navigation
            // and feeding motion rather than only the free-roaming loop.
            if (!calvesOn_)
                continue;
            if (calf.routeState == Calf::RouteState::Roaming)
            {
                calf.animationTime += deltaTime;
                updateCalfPath(calf, deltaTime);
            }
            else if (calf.routeState == Calf::RouteState::GoingToShelter ||
                     calf.routeState == Calf::RouteState::GoingToField ||
                     calf.routeState == Calf::RouteState::GoingToFeed)
            {
                calf.animationTime += deltaTime;
                updateCalfNavigation(calf, deltaTime, calfDestination_);
            }
            else if (calf.routeState == Calf::RouteState::Feeding)
                calf.animationTime += deltaTime;
        }
        for (Bird& bird : birds_)
        {
            if (!bird.mobile)
                continue;
            if (bird.routeState == Calf::RouteState::Roaming)
            {
                bird.animationTime += deltaTime;
                updateBirdPath(bird, deltaTime);
            }
            else if (bird.routeState == Calf::RouteState::GoingToShelter ||
                     bird.routeState == Calf::RouteState::GoingToField)
            {
                bird.animationTime += deltaTime;
                updateBirdNavigation(bird, deltaTime, shelterRequested_);
            }
        }
    }

    for (Worker& worker : workers_)
    {
        if (worker.state == WorkerState::WalkingToCows ||
            worker.state == WorkerState::WalkingHome)
        {
            worker.animationTime += deltaTime;
            updateWorker(worker, deltaTime);
        }
        else if (worker.state == WorkerState::Feeding)
            worker.animationTime += deltaTime;
    }

    if (fansOn_)
        fanAngle_ = std::fmod(fanAngle_ + 320.0f * deltaTime, 360.0f);
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

void AnimationSystem::toggleCalfShed()
{
    if (nightMode_)
        return;

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

    calfDestination_ = CalfDestination::FeedingArea;
    for (Calf& calf : calves_)
        calf.routeState = Calf::RouteState::GoingToFeed;
}

bool AnimationSystem::calfShedDoorNeeded() const
{
    for (const Calf& calf : calves_)
    {
        // Keep the door open only while a calf is inside or crossing the
        // doorway envelope; close it once both animals have cleared the wall.
        if (calf.routeState == Calf::RouteState::GoingToField &&
            calf.position.x < -5.05f)
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

void AnimationSystem::commandWorker()
{
    if (nightMode_)
        return;

    Worker& worker = workers_[0];
    switch (worker.state)
    {
    case WorkerState::HomeIdle:
        worker.state = WorkerState::WalkingToCows;
        break;
    case WorkerState::ReadyAtCows:
        worker.state = WorkerState::Feeding;
        break;
    case WorkerState::Feeding:
        worker.state = WorkerState::WalkingHome;
        break;
    case WorkerState::WalkingToCows:
    case WorkerState::WalkingHome:
        break;
    }
}

void AnimationSystem::sendWorkerHome()
{
    Worker& worker = workers_[0];
    if (worker.state != WorkerState::HomeIdle)
        worker.state = WorkerState::WalkingHome;
}

const char* AnimationSystem::workerStatus() const
{
    switch (workers_[0].state)
    {
    case WorkerState::HomeIdle: return "resting at home";
    case WorkerState::WalkingToCows: return "walking to the cow shed";
    case WorkerState::ReadyAtCows: return "ready beside the fodder trough";
    case WorkerState::Feeding: return "giving food to the cattle";
    case WorkerState::WalkingHome: return "walking home";
    }
    return "unknown";
}

void AnimationSystem::updateCowPatrol(Cow& cow, float deltaTime)
{
    const glm::vec3 target = cow.targetingB ? cow.pathB : cow.pathA;
    const glm::vec3 difference = target - cow.position;
    const float distanceSquared = glm::length2(difference);
    if (distanceSquared < 0.0025f)
    {
        cow.position = target;
        cow.targetingB = !cow.targetingB;
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const glm::vec3 direction = difference / distance;
    cow.position += direction * std::min(cow.speed * deltaTime, distance);
    cow.yaw = yawForDirection(direction);
}

void AnimationSystem::updateCalfPath(Calf& calf, float deltaTime)
{
    calf.pathAngle = std::fmod(
        calf.pathAngle + calf.direction * calf.speed * 0.42f * deltaTime,
        glm::two_pi<float>());
    if (calf.pathAngle < 0.0f)
        calf.pathAngle += glm::two_pi<float>();

    const float cosine = std::cos(calf.pathAngle);
    const float sine = std::sin(calf.pathAngle);
    calf.position = calf.pathCenter + glm::vec3(calf.pathRadius.x * cosine, 0.0f, calf.pathRadius.y * sine);

    glm::vec3 tangent(
        -calf.pathRadius.x * sine * calf.direction,
        0.0f,
         calf.pathRadius.y * cosine * calf.direction);
    if (glm::length2(tangent) > 0.000001f)
        calf.yaw = yawForDirection(glm::normalize(tangent));
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
        target = std::abs(calf.position.z - insideDoor.z) > 0.045f
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
        target = calf.pathCenter + glm::vec3(calf.pathRadius.x * std::cos(calf.pathAngle),
                                             0.0f,
                                             calf.pathRadius.y * std::sin(calf.pathAngle));
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

void AnimationSystem::updateWorker(Worker& worker, float deltaTime)
{
    const bool goingToCows = worker.state == WorkerState::WalkingToCows;
    const glm::vec3 target = goingToCows ? worker.pathB : worker.pathA;
    const glm::vec3 difference = target - worker.position;
    const float distanceSquared = glm::length2(difference);
    if (distanceSquared < 0.0025f)
    {
        worker.position = target;
        worker.state = goingToCows ? WorkerState::ReadyAtCows : WorkerState::HomeIdle;
        if (goingToCows)
            worker.yaw = 90.0f;
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const glm::vec3 direction = difference / distance;
    worker.position += direction * std::min(worker.speed * deltaTime, distance);
    worker.yaw = yawForDirection(direction);
}

void AnimationSystem::updateBirdPath(Bird& bird, float deltaTime)
{
    bird.pathAngle = std::fmod(
        bird.pathAngle + bird.direction * bird.speed * deltaTime,
        glm::two_pi<float>());
    if (bird.pathAngle < 0.0f)
        bird.pathAngle += glm::two_pi<float>();
    const float c = std::cos(bird.pathAngle);
    const float s = std::sin(bird.pathAngle);
    bird.position = bird.pathCenter + glm::vec3(bird.pathRadius.x * c, 0.0f,
                                                bird.pathRadius.y * s);
    const glm::vec3 tangent(-bird.pathRadius.x * s * bird.direction, 0.0f,
                             bird.pathRadius.y * c * bird.direction);
    if (glm::length2(tangent) > 0.000001f)
        bird.yaw = yawForDirection(glm::normalize(tangent));
}

void AnimationSystem::updateBirdNavigation(Bird& bird, float deltaTime,
                                           bool shelterRequested)
{
    glm::vec3 target = bird.shelterPosition;
    if (!shelterRequested)
        target = bird.pathCenter + glm::vec3(bird.pathRadius.x * std::cos(bird.pathAngle),
                                             0.0f,
                                             bird.pathRadius.y * std::sin(bird.pathAngle));
    if (moveToward(bird.position, bird.yaw, target,
                   std::max(0.72f, bird.speed), deltaTime))
        bird.routeState = shelterRequested
            ? Calf::RouteState::Sheltered : Calf::RouteState::Roaming;
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
    const float distance = std::sqrt(distanceSquared);
    const glm::vec3 direction = difference / distance;
    position += direction * std::min(speed * deltaTime, distance);
    yaw = yawForDirection(direction);
    return distance <= speed * deltaTime;
}

float AnimationSystem::yawForDirection(const glm::vec3& direction)
{
    // All animated models use local +X as forward.
    return glm::degrees(std::atan2(-direction.z, direction.x));
}
