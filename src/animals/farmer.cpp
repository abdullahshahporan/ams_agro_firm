// Item: farmer model and behaviour.
#include "entity_renderer.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include "shared/entity_transform.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

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
    const float cycle = sin((worker.animationTime + worker.phase) * 6.0f);
    const float legSwing = walking ? cycle * 31.0f : 0.0f;
    const float workSwing = walking ? -legSwing
                                    : ((feeding || lifting) ? 30.0f + cycle * 33.0f : 0.0f);
    glm::mat4 root = makeRoot(worker.position, worker.yaw, 1.0f);
    if (walking)
        root = glm::translate(root, glm::vec3(0.0f, abs(cycle) * 0.035f, 0.0f));

    const glm::vec3 trousers(0.12f, 0.16f, 0.22f);
    const glm::vec3 skin(0.58f, 0.34f, 0.20f);
    cubes_.drawColored(shader, root, glm::vec3(0.0f, 1.27f, 0.0f),
                       glm::vec3(0.58f, 0.70f, 0.34f), worker.shirtColor);
    cubes_.drawColored(shader, root, glm::vec3(0.0f, 0.88f, 0.0f),
                       glm::vec3(0.54f, 0.18f, 0.32f), trousers);
    primitives_.drawSphere(shader, root, glm::vec3(0.0f, 1.82f, 0.0f),
                           glm::vec3(0.48f, 0.56f, 0.48f), skin);
    primitives_.drawCone(shader, root, glm::vec3(0.0f, 2.16f, 0.0f),
                         glm::vec3(0.78f, 0.34f, 0.78f),
                         glm::vec3(0.92f, 0.66f, 0.16f));
    cubes_.drawColored(shader, root, glm::vec3(0.0f, 2.02f, 0.0f),
                       glm::vec3(0.86f, 0.06f, 0.86f),
                       glm::vec3(0.84f, 0.52f, 0.10f));

    for (float side : {-1.0f, 1.0f})
    {
        glm::mat4 hip = glm::translate(root, glm::vec3(0.0f, 0.88f, side * 0.17f));
        hip = glm::rotate(hip, glm::radians(side * legSwing), glm::vec3(0.0f, 0.0f, 1.0f));
        drawHumanLimb(shader, hip, 0.45f, 0.45f, -side * legSwing * 0.25f,
                      trousers, trousers * 0.78f, 0.18f);

        const float armAngle = worker.carryingHay && !feeding
            ? side * 135.0f
            : ((side > 0.0f) ? workSwing
                             : -workSwing * (walking ? 1.0f : 0.35f));
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
            bundlePosition.y = 0.6f + 1.95f * min(worker.taskTimer / 0.85f, 1.0f);
        if (feeding)
            bundlePosition = glm::mix(bundlePosition,
                                      glm::vec3(-12.0f, 0.72f, -5.45f),
                                      min(worker.taskTimer / 1.35f, 1.0f));
        applyMaterial(shader, Materials::hay());
        cubes_.drawColored(shader, bundlePosition,
                           glm::vec3(0.94f, 0.37f, 0.72f),
                           glm::vec3(0.79f, 0.64f, 0.28f));
        for (float x : {-0.24f, 0.24f})
            cubes_.drawColored(shader, bundlePosition + glm::vec3(x, 0.0f, 0.0f),
                               glm::vec3(0.045f, 0.39f, 0.75f),
                               glm::vec3(0.31f, 0.20f, 0.09f));
    }
}

void EntityRenderer::drawHumanLimb(
    const Shader& shader, const glm::mat4& joint,
    float upperLength, float lowerLength, float bend,
    const glm::vec3& upperColor, const glm::vec3& lowerColor,
    float thickness) const
{
    cubes_.drawColored(shader, joint, glm::vec3(0.0f, -upperLength * 0.5f, 0.0f),
                       glm::vec3(thickness, upperLength, thickness), upperColor);
    glm::mat4 elbow = glm::translate(joint, glm::vec3(0.0f, -upperLength, 0.0f));
    elbow = glm::rotate(elbow, glm::radians(bend), glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawColored(shader, elbow, glm::vec3(0.0f, -lowerLength * 0.5f, 0.0f),
                       glm::vec3(thickness * 0.86f, lowerLength, thickness * 0.86f),
                       lowerColor);
}

namespace
{
int homeRouteIndex(const glm::vec3& position)
{
    if (position.x > 6.0f && position.z > 9.5f) return 5;
    if (position.x > 1.0f && position.z > 7.0f) return 4;
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
          {glm::vec3(8.0f, 0.0f, 11.30f), 90.0f, WorkerState::HomeIdle,
           2.00f, 0.0f, 0.0f,
           glm::vec3(8.0f, 0.0f, 11.30f), glm::vec3(-12.0f, 0.0f, -3.8f), true,
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
    if (worker.state != WorkerState::HomeIdle)
    {
        if (glm::distance(worker.position, worker.pathA) < 0.65f)
        {
            worker.position = worker.pathA;
            worker.state = WorkerState::HomeIdle;
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
    if (!worker.carryingHay &&
        glm::distance(here, glm::vec2(-4.1f, 3.4f)) < 1.5f)
    {
        worker.state = WorkerState::PickingUpHay;
        worker.taskTimer = 0.0f;
        return true;
    }
    else if (worker.carryingHay &&
             glm::distance(here, glm::vec2(-12.0f, -3.8f)) < 1.6f)
    {
        worker.state = WorkerState::Feeding;
        worker.taskTimer = 0.0f;
        return true;
    }
    return false;
}

void AnimationSystem::turnManualWorker(float angleDegrees)
{
    if (manualWorkerControl_)
        workers_[0].yaw += angleDegrees;
}

void AnimationSystem::moveManualWorker(const glm::vec3& position, float deltaTime)
{
    if (!manualWorkerControl_ || workers_[0].state != WorkerState::Manual)
        return;
    workers_[0].manualWalking = glm::distance(position, workers_[0].position) > 0.001f;
    workers_[0].position = position;
    workers_[0].animationTime += deltaTime;
}

const char* AnimationSystem::workerStatus() const
{
    switch (workers_[0].state)
    {
    case WorkerState::HomeIdle: return "resting inside the worker house";
    case WorkerState::WalkingToHay: return "leaving the house for the hay stack";
    case WorkerState::PickingUpHay: return "lifting hay onto his head";
    case WorkerState::WalkingToCows: return "carrying hay to the cattle";
    case WorkerState::ReadyAtCows: return "ready beside the cattle trough";
    case WorkerState::Feeding: return "placing hay in front of the cows";
    case WorkerState::WalkingHome: return "walking home";
    case WorkerState::Manual: return workers_[0].carryingHay
        ? "under your control, carrying hay" : "under your control";
    }
    return "unknown";
}

void AnimationSystem::updateWorker(Worker& worker, float deltaTime)
{
    // The route skirts the poultry shed, the hay stacks and the cattle shed
    // walls. Its door and pickup positions match the visible scene objects.
    const array<glm::vec3, 5> toHay = {{
        {8.0f, 0.0f, 9.8f}, {4.6f, 0.0f, 9.8f},
        {0.0f, 0.0f, 7.2f}, {-3.9f, 0.0f, 6.1f}, {-4.1f, 0.0f, 3.4f}
    }};
    const array<glm::vec3, 3> toCows = {{
        {-3.9f, 0.0f, 0.0f}, {-3.9f, 0.0f, -3.8f},
        {-12.0f, 0.0f, -3.8f}
    }};
    const array<glm::vec3, 8> toHome = {{
        {-3.9f, 0.0f, -3.8f}, {-3.9f, 0.0f, 0.0f},
        {-3.9f, 0.0f, 6.1f}, {0.0f, 0.0f, 7.2f},
        {4.6f, 0.0f, 9.8f}, {8.0f, 0.0f, 9.8f},
        {8.0f, 0.0f, 10.5f}, worker.pathA
    }};

    glm::vec3 target;
    int count = 0;
    if (worker.state == WorkerState::WalkingToHay)
    {
        count = static_cast<int>(toHay.size());
        target = toHay[worker.routeIndex];
    }
    else if (worker.state == WorkerState::WalkingToCows)
    {
        count = static_cast<int>(toCows.size());
        target = toCows[worker.routeIndex];
    }
    else
    {
        count = static_cast<int>(toHome.size());
        target = toHome[worker.routeIndex];
    }

    const glm::vec3 difference = target - worker.position;
    const float distance = glm::length(difference);
    if (distance > 0.001f)
    {
        const glm::vec3 direction = difference / distance;
        worker.position += direction * min(worker.speed * deltaTime, distance);
        worker.yaw = yawForDirection(direction);
    }
    if (glm::distance(worker.position, target) > 0.015f)
        return;

    worker.position = target;
    ++worker.routeIndex;
    if (worker.routeIndex < count)
        return;
    worker.routeIndex = 0;
    if (worker.state == WorkerState::WalkingToHay)
    {
        worker.state = WorkerState::PickingUpHay;
        worker.taskTimer = 0.0f;
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
        worker.carryingHay = false;
        worker.yaw = 90.0f;
    }
}

void AnimationSystem::updateWorkers(float deltaTime)
{
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
                worker.taskTimer = 0.0f;
                worker.routeIndex = 0;
                worker.state = manualWorkerControl_ ? WorkerState::Manual
                                                    : WorkerState::WalkingHome;
            }
        }
    }

}
