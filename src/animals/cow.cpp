// Item: cow model and behaviour.
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

void EntityRenderer::drawCow(const Shader& shader, const Cow& cow) const
{
    float gait = 0.0f;
    float bob = 0.0f;
    if (cow.state == CowState::Walking)
    {
        gait = sin((cow.animationTime + cow.phase) * 5.0f) * 27.0f;
        bob = abs(sin((cow.animationTime + cow.phase) * 5.0f)) * 0.045f;
    }

    float headDrop = 0.0f;
    if (cow.state == CowState::Grazing)
        headDrop = 59.0f + sin(cow.headAnimationTime * 1.8f) * 5.0f;
    else if (cow.state == CowState::Feeding)
        // Keep the longer natural face above the trough's back rim.
        headDrop = 14.0f + sin(cow.headAnimationTime * 1.4f) * 1.5f;

    const float headTurn = (cow.state == CowState::Standing)
        ? sin(cow.headAnimationTime * 0.8f) * 12.0f
        : sin(cow.headAnimationTime * 1.1f) * 3.0f;
    const float tail = sin((cow.animationTime + cow.phase) * 3.0f) * 20.0f;

    drawBovine(shader, makeRoot(cow.position, cow.yaw, cow.scale),
                cow.bodyColor, cow.patchColor, gait, headDrop, headTurn,
                tail, bob, cow.hornScale, false,
                cow.role == CattleRole::MilchCow,
                cow.role == CattleRole::Ox);
}

void EntityRenderer::drawTether(const Shader& shader, const Cow& cow) const
{
    const glm::mat4 cowRoot = makeRoot(cow.position, cow.yaw, cow.scale);
    const glm::vec3 collarPoint = glm::vec3(
        cowRoot * glm::vec4(1.35f, 1.79f, 0.30f, 1.0f));
    const glm::vec3 difference = cow.tetherAnchor - collarPoint;
    const float length = glm::length(difference);
    if (length < 0.001f)
        return;

    const glm::vec3 direction = difference / length;
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const float cosine = glm::clamp(glm::dot(up, direction), -1.0f, 1.0f);
    glm::mat4 rope(1.0f);
    rope = glm::translate(rope, (collarPoint + cow.tetherAnchor) * 0.5f);
    glm::vec3 axis = glm::cross(up, direction);
    if (glm::length(axis) > 0.0001f)
        rope = glm::rotate(rope, acos(cosine), glm::normalize(axis));
    else if (cosine < 0.0f)
        rope = glm::rotate(rope, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    applyMaterial(shader, Materials::wood());
    primitives_.drawSphere(shader, glm::mat4(1.0f), collarPoint,
                           glm::vec3(0.11f), glm::vec3(0.38f, 0.12f, 0.06f));
    cubes_.drawColored(shader, rope, glm::vec3(0.0f),
                       glm::vec3(0.055f, length, 0.055f),
                       glm::vec3(0.48f, 0.30f, 0.12f));
    cubes_.drawColored(shader, cow.tetherAnchor, glm::vec3(0.13f),
                       glm::vec3(0.34f, 0.22f, 0.10f));
    applyMaterial(shader, Materials::cow());
}

// Initial positions, dimensions, colours and states for this item.
void AnimationSystem::initializeCows()
{
    cows_ = {{
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
      }};
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

    const float distance = sqrt(distanceSquared);
    const glm::vec3 direction = difference / distance;
    cow.position += direction * min(cow.speed * deltaTime, distance);
    cow.yaw = yawForDirection(direction);
}

void AnimationSystem::updateCows(float deltaTime)
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

}
