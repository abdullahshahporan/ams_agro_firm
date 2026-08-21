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
           glm::vec3(0.91f, 0.88f, 0.78f), glm::vec3(0.08f, 0.07f, 0.06f), 0.55f,
           true, glm::vec3(-12.0f, 1.05f, -6.10f), CattleRole::MilchCow},
          {glm::vec3(-8.0f, 0.0f, -8.0f), -90.0f, 0.96f, CowState::Standing,
           0.0f, 0.0f, 2.4f, 2.4f,
           glm::vec3(-8.0f, 0.0f, -8.0f), glm::vec3(-8.0f, 0.0f, -8.0f), false,
           glm::vec3(0.24f, 0.18f, 0.13f), glm::vec3(0.88f, 0.80f, 0.68f), 1.08f,
           true, glm::vec3(-8.0f, 1.05f, -6.10f), CattleRole::Ox}
      }},
      calves_{{
          {glm::vec3(0.0f), 0.0f, 0.60f, 1.85f, 0.0f, 0.25f,
           glm::vec3(3.0f, 0.0f, -7.0f), glm::vec2(3.0f, 1.8f), 1.0f,
           glm::vec3(0.78f, 0.48f, 0.24f), glm::vec3(0.96f, 0.84f, 0.64f),
           glm::vec3(-11.0f, 0.0f, -9.65f)},
          {glm::vec3(0.0f), 0.0f, 0.54f, 2.15f, 0.0f, 3.40f,
           glm::vec3(3.2f, 0.0f, 0.0f), glm::vec2(1.8f, 1.15f), -1.0f,
           glm::vec3(0.86f, 0.84f, 0.76f), glm::vec3(0.12f, 0.10f, 0.08f),
           glm::vec3(-9.0f, 0.0f, -9.65f)}
      }},
      workers_{{
          {glm::vec3(7.0f, 0.0f, -5.5f), 0.0f, WorkerState::Walking,
           1.0f, 0.0f, 0.0f,
           glm::vec3(7.0f, 0.0f, -5.5f), glm::vec3(-4.5f, 0.0f, -1.8f), true,
           glm::vec3(0.12f, 0.42f, 0.72f)},
          {glm::vec3(-10.0f, 0.0f, -4.15f), 90.0f, WorkerState::Working,
           0.0f, 0.0f, 1.2f,
           glm::vec3(-10.0f, 0.0f, -4.15f), glm::vec3(-10.0f, 0.0f, -4.15f), false,
           glm::vec3(0.68f, 0.18f, 0.12f)}
      }},
      birds_{{
          {BirdSpecies::Chicken, glm::vec3(0.0f), glm::vec3(9.20f,0.0f,6.70f),
           glm::vec3(4.0f,0.0f,7.0f), glm::vec2(2.4f,1.5f), 0.2f, 1.0f, 1.20f, 0.48f, 0.0f, 0.0f, true, false, glm::vec3(0.72f,0.24f,0.10f)},
          {BirdSpecies::Chicken, glm::vec3(0.0f), glm::vec3(9.65f,0.0f,6.75f),
           glm::vec3(3.5f,0.0f,10.0f), glm::vec2(2.0f,1.2f), 2.8f, -1.0f, 1.05f, 0.44f, 0.0f, 0.5f, true, false, glm::vec3(0.92f,0.78f,0.42f)},
          {BirdSpecies::Duck, glm::vec3(0.0f), glm::vec3(10.15f,0.0f,6.70f),
           glm::vec3(7.0f,0.0f,5.5f), glm::vec2(2.1f,1.3f), 1.4f, 1.0f, 0.82f, 0.52f, 0.0f, 1.0f, true, false, glm::vec3(0.88f,0.86f,0.68f)},
          {BirdSpecies::Duck, glm::vec3(0.0f), glm::vec3(10.60f,0.0f,6.75f),
           glm::vec3(6.5f,0.0f,9.0f), glm::vec2(1.7f,1.0f), 4.2f, -1.0f, 0.76f, 0.49f, 0.0f, 1.5f, true, false, glm::vec3(0.36f,0.42f,0.30f)},
          {BirdSpecies::Chicken, glm::vec3(10.35f,0.0f,7.35f), glm::vec3(10.35f,0.0f,7.35f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.50f, 180.0f, 0.0f, false, false, glm::vec3(0.62f,0.20f,0.08f)},
          {BirdSpecies::Chicken, glm::vec3(9.95f,0.0f,7.05f), glm::vec3(9.95f,0.0f,7.05f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.22f, 40.0f, 0.0f, false, true, glm::vec3(0.96f,0.82f,0.20f)},
          {BirdSpecies::Chicken, glm::vec3(10.28f,0.0f,6.98f), glm::vec3(10.28f,0.0f,6.98f),
           glm::vec3(0.0f), glm::vec2(0.0f), 0.0f, 1.0f, 0.0f, 0.20f, -20.0f, 0.0f, false, true, glm::vec3(0.98f,0.86f,0.24f)}
      }}
{
    for (Calf& calf : calves_)
        updateCalfPath(calf, 0.0f);
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
        {
            // H controls only the cows assigned to grazing/feeding behavior.
            if (cow.state == CowState::Grazing || cow.state == CowState::Feeding)
                cow.headAnimationTime += deltaTime;
        }
    }

    if (!nightMode_ && calvesOn_)
    {
        for (Calf& calf : calves_)
        {
            calf.animationTime += deltaTime;
            updateCalfPath(calf, deltaTime);
        }
    }

    if (!nightMode_)
    {
        for (Bird& bird : birds_)
        {
            if (!bird.mobile)
                continue;
            bird.animationTime += deltaTime;
            updateBirdPath(bird, deltaTime);
        }
    }

    if (workersOn_)
    {
        for (Worker& worker : workers_)
        {
            worker.animationTime += deltaTime;
            if (worker.state == WorkerState::Walking)
                updateWorker(worker, deltaTime);
        }
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
        for (Calf& calf : calves_)
        {
            calf.position = calf.shelterPosition;
            calf.yaw = 90.0f;
        }
        for (Bird& bird : birds_)
        {
            bird.position = bird.shelterPosition;
            bird.yaw = 180.0f;
        }
    }
    else
    {
        for (Calf& calf : calves_)
            updateCalfPath(calf, 0.0f);
        for (Bird& bird : birds_)
            if (bird.mobile)
                updateBirdPath(bird, 0.0f);
    }
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

void AnimationSystem::updateWorker(Worker& worker, float deltaTime)
{
    const glm::vec3 target = worker.targetingB ? worker.pathB : worker.pathA;
    const glm::vec3 difference = target - worker.position;
    const float distanceSquared = glm::length2(difference);
    if (distanceSquared < 0.0025f)
    {
        worker.position = target;
        worker.targetingB = !worker.targetingB;
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

float AnimationSystem::yawForDirection(const glm::vec3& direction)
{
    // All animated models use local +X as forward.
    return glm::degrees(std::atan2(-direction.z, direction.x));
}
