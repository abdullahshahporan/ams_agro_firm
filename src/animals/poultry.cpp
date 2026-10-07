// Item: poultry model and behaviour.
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

void EntityRenderer::drawBird(const Shader& shader, const Bird& bird) const
{
    glm::mat4 root = makeRoot(bird.position, bird.yaw, bird.scale);
    const bool moving = bird.mobile &&
                        bird.routeState != Calf::RouteState::Sheltered;
    const float cycle = moving ? sin(bird.animationTime * 9.0f) : 0.0f;
    const bool seatedHen = !bird.mobile && !bird.juvenile;
    const float bodyY = seatedHen ? 0.30f : 0.46f;
    const glm::vec3 orange(0.92f, 0.48f, 0.08f);

    if (bird.species == BirdSpecies::Duck)
    {
        primitives_.drawSphere(shader, root, glm::vec3(0.0f, bodyY, 0.0f),
                               glm::vec3(1.15f, 0.62f, 0.72f), bird.color);
        primitives_.drawSphere(shader, root, glm::vec3(0.48f, 0.82f, 0.0f),
                               glm::vec3(0.58f), bird.color * 0.82f);
        cubes_.drawColored(shader, root, glm::vec3(0.82f, 0.76f, 0.0f),
                           glm::vec3(0.42f, 0.12f, 0.34f), orange);
    }
    else
    {
        primitives_.drawSphere(shader, root, glm::vec3(0.0f, bodyY, 0.0f),
                               glm::vec3(0.88f, 0.78f, 0.76f), bird.color);
        primitives_.drawSphere(shader, root, glm::vec3(0.38f, 0.82f, 0.0f),
                               glm::vec3(0.48f), bird.color * 0.88f);
        primitives_.drawCone(shader, root, glm::vec3(0.67f, 0.80f, 0.0f),
                             glm::vec3(0.16f, 0.34f, 0.16f), orange,
                             -90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        if (!bird.juvenile)
            cubes_.drawColored(shader, root, glm::vec3(0.35f, 1.08f, 0.0f),
                               glm::vec3(0.20f, 0.18f, 0.12f), glm::vec3(0.78f, 0.05f, 0.03f));
    }

    for (float side : {-1.0f, 1.0f})
        primitives_.drawSphere(shader, root, glm::vec3(-0.05f, bodyY + 0.04f, side * 0.34f),
                               glm::vec3(0.55f, 0.38f, 0.12f), bird.color * 0.76f,
                               side * (10.0f + cycle * 8.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    if (!seatedHen)
    {
        for (float side : {-1.0f, 1.0f})
        {
            cubes_.drawColored(shader, root,
                               glm::vec3(side * cycle * 0.05f, 0.15f, side * 0.18f),
                               glm::vec3(0.06f, 0.30f, 0.06f), orange,
                               side * cycle * 18.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }
}

// Initial positions, dimensions, colours and states for this item.
void AnimationSystem::initializeBirds()
{
    birds_ = {{
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
      }};
}

void AnimationSystem::updateBirdPath(Bird& bird, float deltaTime)
{
    bird.pathAngle = fmod(
        bird.pathAngle + bird.direction * bird.speed * deltaTime,
        glm::two_pi<float>());
    if (bird.pathAngle < 0.0f)
        bird.pathAngle += glm::two_pi<float>();
    const float c = cos(bird.pathAngle);
    const float s = sin(bird.pathAngle);
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
        target = bird.pathCenter + glm::vec3(bird.pathRadius.x * cos(bird.pathAngle),
                                             0.0f,
                                             bird.pathRadius.y * sin(bird.pathAngle));
    if (moveToward(bird.position, bird.yaw, target,
                   max(0.72f, bird.speed), deltaTime))
        bird.routeState = shelterRequested
            ? Calf::RouteState::Sheltered : Calf::RouteState::Roaming;
}

void AnimationSystem::updateBirds(float deltaTime)
{
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
