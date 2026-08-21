#include "entity_renderer.h"
#include "material.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <algorithm>

namespace
{
glm::mat4 makeRoot(const glm::vec3& position, float yaw, float scale)
{
    glm::mat4 root(1.0f);
    root = glm::translate(root, position);
    root = glm::rotate(root, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::scale(root, glm::vec3(scale));
}
}

void EntityRenderer::render(const Shader& shader, const AnimationSystem& animations) const
{
    for (const Cow& cow : animations.cows())
    {
        applyMaterial(shader, Materials::cow());
        drawCow(shader, cow);
        if (cow.tied)
            drawTether(shader, cow);
    }
    for (const Calf& calf : animations.calves())
    {
        applyMaterial(shader, Materials::cow());
        drawCalf(shader, calf);
        if (animations.nightMode() &&
            calf.routeState == Calf::RouteState::Sheltered)
            drawCalfTether(shader, calf);
    }
    for (const Worker& worker : animations.workers())
    {
        if (worker.state == WorkerState::HomeIdle)
            continue;
        applyMaterial(shader, Materials::cloth());
        drawWorker(shader, worker);
    }
    for (const Bird& bird : animations.birds())
    {
        applyMaterial(shader, Materials::cow());
        drawBird(shader, bird);
    }
}

void EntityRenderer::drawCow(const Shader& shader, const Cow& cow) const
{
    float gait = 0.0f;
    float bob = 0.0f;
    if (cow.state == CowState::Walking)
    {
        gait = std::sin((cow.animationTime + cow.phase) * 5.0f) * 27.0f;
        bob = std::abs(std::sin((cow.animationTime + cow.phase) * 5.0f)) * 0.045f;
    }

    float headDrop = 0.0f;
    if (cow.state == CowState::Grazing)
        headDrop = 59.0f + std::sin(cow.headAnimationTime * 1.8f) * 5.0f;
    else if (cow.state == CowState::Feeding)
        headDrop = 42.0f + std::sin(cow.headAnimationTime * 1.4f) * 3.0f;

    const float headTurn = (cow.state == CowState::Standing)
        ? std::sin(cow.headAnimationTime * 0.8f) * 12.0f
        : std::sin(cow.headAnimationTime * 1.1f) * 3.0f;
    const float tail = std::sin((cow.animationTime + cow.phase) * 3.0f) * 20.0f;

    drawBovine(shader, makeRoot(cow.position, cow.yaw, cow.scale),
                cow.bodyColor, cow.patchColor, gait, headDrop, headTurn,
                tail, bob, cow.hornScale, false,
                cow.role == CattleRole::MilchCow,
                cow.role == CattleRole::Ox);
}

void EntityRenderer::drawCalf(const Shader& shader, const Calf& calf) const
{
    const bool feeding = calf.routeState == Calf::RouteState::Feeding;
    const bool stationary = feeding || calf.routeState == Calf::RouteState::Sheltered;
    const float gait = stationary ? 0.0f
                                  : std::sin(calf.animationTime * 10.0f) * 36.0f;
    const float bob = stationary ? 0.0f
                                 : std::abs(std::sin(calf.animationTime * 10.0f)) * 0.075f;
    const float tail = std::sin(calf.animationTime * 6.0f) * 24.0f;
    const float headDrop = feeding
        ? 50.0f + std::sin(calf.animationTime * 1.8f) * 4.0f : 0.0f;
    drawBovine(shader, makeRoot(calf.position, calf.yaw, calf.scale),
                calf.bodyColor, calf.patchColor, gait, headDrop,
                std::sin(calf.animationTime * 2.0f) * 4.0f,
                tail, bob, 0.0f, true, false, false);
}

void EntityRenderer::drawCalfTether(const Shader& shader, const Calf& calf) const
{
    const glm::mat4 calfRoot = makeRoot(calf.position, calf.yaw, calf.scale);
    const glm::vec3 collar = glm::vec3(
        calfRoot * glm::vec4(1.0f, 1.18f, 0.0f, 1.0f));
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
        rope = glm::rotate(rope, std::acos(cosine), glm::normalize(axis));
    else if (cosine < 0.0f)
        rope = glm::rotate(rope, glm::radians(180.0f),
                           glm::vec3(1.0f, 0.0f, 0.0f));

    applyMaterial(shader, Materials::wood());
    cubes_.drawColored(shader, collar, glm::vec3(0.22f, 0.09f, 0.25f),
                       glm::vec3(0.38f, 0.12f, 0.06f), calf.yaw,
                       glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawColored(shader, rope, glm::vec3(0.0f),
                       glm::vec3(0.045f, length, 0.045f),
                       glm::vec3(0.48f, 0.30f, 0.12f));
    cubes_.drawColored(shader, anchor, glm::vec3(0.12f),
                       glm::vec3(0.34f, 0.20f, 0.08f));
}

void EntityRenderer::drawBird(const Shader& shader, const Bird& bird) const
{
    glm::mat4 root = makeRoot(bird.position, bird.yaw, bird.scale);
    const bool moving = bird.mobile &&
                        bird.routeState != Calf::RouteState::Sheltered;
    const float cycle = moving ? std::sin(bird.animationTime * 9.0f) : 0.0f;
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

void EntityRenderer::drawTether(const Shader& shader, const Cow& cow) const
{
    const glm::mat4 cowRoot = makeRoot(cow.position, cow.yaw, cow.scale);
    const glm::vec3 collarPoint = glm::vec3(
        cowRoot * glm::vec4(1.18f, 1.46f, 0.0f, 1.0f));
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
        rope = glm::rotate(rope, std::acos(cosine), glm::normalize(axis));
    else if (cosine < 0.0f)
        rope = glm::rotate(rope, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    applyMaterial(shader, Materials::wood());
    cubes_.drawColored(shader, collarPoint, glm::vec3(0.30f, 0.12f, 0.34f),
                       glm::vec3(0.38f, 0.12f, 0.06f), cow.yaw,
                       glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawColored(shader, rope, glm::vec3(0.0f),
                       glm::vec3(0.055f, length, 0.055f),
                       glm::vec3(0.48f, 0.30f, 0.12f));
    cubes_.drawColored(shader, cow.tetherAnchor, glm::vec3(0.13f),
                       glm::vec3(0.34f, 0.22f, 0.10f));
    applyMaterial(shader, Materials::cow());
}

void EntityRenderer::drawBovine(
    const Shader& shader, const glm::mat4& root,
    const glm::vec3& bodyColor, const glm::vec3& patchColor,
    float gaitAngle, float headDrop, float headTurn,
    float tailAngle, float bodyBob, float hornScale, bool juvenile,
    bool showUdder, bool ox) const
{
    const bool milchCow = showUdder && !juvenile;
    const bool adultOx = ox && !juvenile;
    const glm::vec3 darkLeg = adultOx ? bodyColor * 0.74f : bodyColor * 0.88f;
    const float bodyY = juvenile ? 1.20f : (adultOx ? 1.48f : 1.40f);
    const glm::vec3 bodySize = juvenile
        ? glm::vec3(2.75f, 1.42f, 1.20f)
        : (adultOx ? glm::vec3(3.48f, 1.72f, 1.48f)
                   : glm::vec3(3.38f, 1.62f, 1.38f));
    glm::mat4 bobRoot = glm::translate(root, glm::vec3(0.0f, bodyBob, 0.0f));

    primitives_.drawSphere(shader, bobRoot, glm::vec3(0.0f, bodyY, 0.0f),
                           bodySize, bodyColor);

    if (adultOx)
    {
        // Brahman-type ox: muscular shoulder, high hump and loose dewlap.
        primitives_.drawSphere(shader, bobRoot, glm::vec3(0.76f, bodyY + 0.62f, 0.0f),
                               glm::vec3(1.18f, 0.86f, 1.30f), bodyColor * 1.03f);
        primitives_.drawSphere(shader, bobRoot, glm::vec3(1.18f, bodyY + 0.03f, 0.0f),
                               glm::vec3(1.14f, 1.42f, 1.36f), bodyColor * 0.96f);
        primitives_.drawSphere(shader, bobRoot, glm::vec3(1.02f, bodyY - 0.70f, 0.0f),
                               glm::vec3(1.34f, 0.82f, 0.22f), bodyColor * 0.82f,
                               -12.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        primitives_.drawSphere(shader, bobRoot, glm::vec3(1.38f, bodyY - 0.86f, 0.0f),
                               glm::vec3(0.72f, 0.58f, 0.18f), bodyColor * 0.76f,
                               -18.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else if (milchCow)
    {
        // Irregular Holstein markings are layered on both flanks instead of a
        // single stripe, keeping the white dairy-cow body clearly readable.
        for (float side : {-1.0f, 1.0f})
        {
            const float surface = side * 0.688f;
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(-1.02f, bodyY + 0.16f, surface),
                                   glm::vec3(0.82f, 0.74f, 0.075f), patchColor,
                                   side * 19.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(-0.77f, bodyY + 0.43f, surface * 1.002f),
                                   glm::vec3(0.46f, 0.42f, 0.078f), patchColor,
                                   side * -16.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(-0.12f, bodyY - 0.12f, surface),
                                   glm::vec3(1.10f, 0.88f, 0.078f), patchColor,
                                   side * -13.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(0.17f, bodyY - 0.39f, surface * 1.003f),
                                   glm::vec3(0.58f, 0.40f, 0.080f), patchColor,
                                   side * 21.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(0.82f, bodyY + 0.24f, surface),
                                   glm::vec3(0.76f, 0.68f, 0.076f), patchColor,
                                   side * 24.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(1.06f, bodyY + 0.03f, surface * 1.002f),
                                   glm::vec3(0.46f, 0.48f, 0.079f), patchColor,
                                   side * -22.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            primitives_.drawSphere(shader, bobRoot,
                                   glm::vec3(1.36f, bodyY - 0.18f, side * 0.54f),
                                   glm::vec3(0.48f, 0.56f, 0.30f), patchColor);
        }
        primitives_.drawSphere(shader, bobRoot,
                               glm::vec3(-0.48f, bodyY + 0.80f, 0.16f),
                               glm::vec3(0.96f, 0.075f, 0.66f), patchColor,
                               11.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else
    {
        // The two calves retain a simpler two-patch coat.
        primitives_.drawSphere(shader, bobRoot, glm::vec3(-0.35f, bodyY + 0.10f, 0.595f),
                               glm::vec3(0.88f, 0.65f, 0.07f), patchColor, 14.0f,
                               glm::vec3(0.0f, 0.0f, 1.0f));
        primitives_.drawSphere(shader, bobRoot, glm::vec3(0.62f, bodyY - 0.20f, -0.595f),
                               glm::vec3(0.68f, 0.52f, 0.07f), patchColor, -18.0f,
                               glm::vec3(0.0f, 0.0f, 1.0f));
    }

    const float hipY = juvenile ? 1.05f : (adultOx ? 1.32f : 1.25f);
    const float foreX = juvenile ? 0.82f : (adultOx ? 1.16f : 1.08f);
    const float rearX = juvenile ? -0.82f : (adultOx ? -1.16f : -1.08f);
    const float hipZ = juvenile ? 0.38f : (adultOx ? 0.50f : 0.45f);
    drawBovineLeg(shader, bobRoot, glm::vec3(foreX, hipY, hipZ), gaitAngle, darkLeg, juvenile);
    drawBovineLeg(shader, bobRoot, glm::vec3(foreX, hipY, -hipZ), -gaitAngle, darkLeg, juvenile);
    drawBovineLeg(shader, bobRoot, glm::vec3(rearX, hipY, hipZ), -gaitAngle, darkLeg, juvenile);
    drawBovineLeg(shader, bobRoot, glm::vec3(rearX, hipY, -hipZ), gaitAngle, darkLeg, juvenile);

    glm::mat4 neck = glm::translate(bobRoot,
                                    glm::vec3(adultOx ? 1.34f : 1.22f,
                                              bodyY + (adultOx ? 0.10f : 0.12f), 0.0f));
    neck = glm::rotate(neck, glm::radians(-headDrop), glm::vec3(0.0f, 0.0f, 1.0f));
    neck = glm::rotate(neck, glm::radians(headTurn), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 headColor = milchCow ? patchColor
                                         : (adultOx ? bodyColor * 0.88f : bodyColor);
    cubes_.drawColored(shader, neck, glm::vec3(0.18f, 0.0f, 0.0f),
                       adultOx ? glm::vec3(0.72f, 0.88f, 0.92f)
                               : glm::vec3(0.55f, 0.68f, 0.74f),
                       headColor, -18.0f,
                       glm::vec3(0.0f, 0.0f, 1.0f));
    const float headScale = juvenile ? 1.08f : (adultOx ? 1.12f : 1.0f);
    primitives_.drawSphere(shader, neck, glm::vec3(0.68f, -0.04f, 0.0f),
                           glm::vec3(1.05f, 0.82f, 0.82f) * headScale, headColor);
    primitives_.drawSphere(shader, neck, glm::vec3(1.08f, -0.16f, 0.0f),
                           (adultOx ? glm::vec3(0.58f, 0.43f, 0.58f)
                                    : glm::vec3(0.67f, 0.46f, 0.62f)) * headScale,
                           adultOx ? glm::vec3(0.16f, 0.10f, 0.075f)
                                   : glm::vec3(0.66f, 0.56f, 0.52f));

    if (milchCow)
    {
        // White blaze down the otherwise black Holstein face.
        primitives_.drawSphere(shader, neck, glm::vec3(1.145f, 0.13f, 0.0f),
                               glm::vec3(0.075f, 0.58f, 0.32f), bodyColor);
        primitives_.drawSphere(shader, neck, glm::vec3(0.73f, 0.36f, 0.0f),
                               glm::vec3(0.64f, 0.075f, 0.36f), bodyColor);
    }

    for (float side : {-1.0f, 1.0f})
    {
        if (adultOx)
            primitives_.drawSphere(shader, neck,
                                   glm::vec3(0.46f, -0.01f, side * 0.68f),
                                   glm::vec3(0.40f, 0.22f, 0.86f), bodyColor * 0.82f,
                                   side * 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        else
            cubes_.drawColored(shader, neck, glm::vec3(0.56f, 0.28f, side * 0.46f),
                               glm::vec3(0.34f, 0.10f, 0.32f),
                               milchCow ? patchColor : bodyColor,
                               side * 18.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        primitives_.drawSphere(shader, neck, glm::vec3(0.98f, 0.02f, side * 0.30f),
                               glm::vec3(0.09f), glm::vec3(0.03f));
        if (!juvenile && hornScale > 0.0f)
            primitives_.drawCone(shader, neck,
                                 glm::vec3(0.66f, 0.47f, side * 0.27f),
                                 adultOx ? glm::vec3(0.18f, 0.56f * hornScale, 0.18f)
                                         : glm::vec3(0.15f, 0.42f * hornScale, 0.15f),
                                 glm::vec3(0.90f, 0.82f, 0.62f),
                                 side * (adultOx ? -32.0f : -10.0f),
                                 glm::vec3(1.0f, 0.0f, 0.0f));

        if (milchCow && side > 0.0f)
            cubes_.drawColored(shader, neck, glm::vec3(0.50f, 0.05f, 0.67f),
                               glm::vec3(0.16f, 0.24f, 0.055f),
                               glm::vec3(0.78f, 0.05f, 0.035f), 12.0f,
                               glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::mat4 tail = glm::translate(bobRoot, glm::vec3(-1.42f, bodyY + 0.18f, 0.0f));
    tail = glm::rotate(tail, glm::radians(-38.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    tail = glm::rotate(tail, glm::radians(tailAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    cubes_.drawColored(shader, tail, glm::vec3(0.0f, -0.34f, 0.0f),
                       glm::vec3(0.11f, 0.68f, 0.11f), darkLeg);
    primitives_.drawSphere(shader, tail, glm::vec3(0.0f, -0.73f, 0.0f),
                           glm::vec3(0.24f, 0.30f, 0.24f), patchColor);

    if (showUdder)
    {
        primitives_.drawSphere(shader, bobRoot, glm::vec3(-0.35f, 0.72f, 0.0f),
                               glm::vec3(0.72f, 0.46f, 0.54f),
                               glm::vec3(0.84f, 0.57f, 0.58f));
        for (float xOffset : {-0.55f, -0.20f})
            for (float zOffset : {-0.18f, 0.18f})
                primitives_.drawCone(shader, bobRoot,
                                     glm::vec3(xOffset, 0.43f, zOffset),
                                     glm::vec3(0.12f, 0.32f, 0.12f),
                                     glm::vec3(0.78f, 0.48f, 0.50f),
                                     180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    if (adultOx)
    {
        // A restrained underside detail reinforces the adult male silhouette.
        primitives_.drawSphere(shader, bobRoot, glm::vec3(-0.20f, 0.69f, 0.0f),
                               glm::vec3(0.42f, 0.42f, 0.34f), bodyColor * 0.62f);
    }
}

void EntityRenderer::drawBovineLeg(
    const Shader& shader, const glm::mat4& root, const glm::vec3& hip,
    float swing, const glm::vec3& legColor, bool juvenile) const
{
    const float upperLength = juvenile ? 0.52f : 0.62f;
    const float lowerLength = juvenile ? 0.44f : 0.52f;
    const float thickness = juvenile ? 0.20f : 0.23f;
    glm::mat4 upper = glm::translate(root, hip);
    upper = glm::rotate(upper, glm::radians(swing), glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawColored(shader, upper, glm::vec3(0.0f, -upperLength * 0.5f, 0.0f),
                       glm::vec3(thickness, upperLength, thickness), legColor);

    glm::mat4 knee = glm::translate(upper, glm::vec3(0.0f, -upperLength, 0.0f));
    knee = glm::rotate(knee, glm::radians(-swing * 0.36f), glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawColored(shader, knee, glm::vec3(0.0f, -lowerLength * 0.5f, 0.0f),
                       glm::vec3(thickness * 0.82f, lowerLength, thickness * 0.82f),
                       legColor * 0.78f);
    cubes_.drawColored(shader, knee, glm::vec3(0.06f, -lowerLength - 0.03f, 0.0f),
                       glm::vec3(0.31f, 0.14f, 0.26f), glm::vec3(0.10f, 0.08f, 0.06f));
}

void EntityRenderer::drawWorker(const Shader& shader, const Worker& worker) const
{
    const bool walking = worker.state == WorkerState::WalkingToCows ||
                         worker.state == WorkerState::WalkingHome;
    const bool feeding = worker.state == WorkerState::Feeding;
    const float cycle = std::sin((worker.animationTime + worker.phase) * 6.0f);
    const float legSwing = walking ? cycle * 31.0f : 0.0f;
    const float workSwing = walking ? -legSwing
                                    : (feeding ? 30.0f + cycle * 33.0f : 0.0f);
    glm::mat4 root = makeRoot(worker.position, worker.yaw, 1.0f);
    if (walking)
        root = glm::translate(root, glm::vec3(0.0f, std::abs(cycle) * 0.035f, 0.0f));

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

        const float armAngle = (side > 0.0f) ? workSwing
                                             : -workSwing * (walking ? 1.0f : 0.35f);
        glm::mat4 shoulder = glm::translate(root, glm::vec3(0.0f, 1.52f, side * 0.38f));
        shoulder = glm::rotate(shoulder, glm::radians(armAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        drawHumanLimb(shader, shoulder, 0.36f, 0.34f,
                      feeding ? -38.0f : 0.0f, worker.shirtColor, skin, 0.14f);
    }

    if (worker.state == WorkerState::WalkingToCows ||
        worker.state == WorkerState::ReadyAtCows || feeding)
    {
        // A small handled feed bucket makes the working pose easy to read.
        cubes_.drawColored(shader, root, glm::vec3(0.42f, 0.48f, 0.0f),
                           glm::vec3(0.38f, 0.40f, 0.38f),
                           glm::vec3(0.28f, 0.34f, 0.38f));
        cubes_.drawColored(shader, root, glm::vec3(0.42f, 0.75f, 0.0f),
                           glm::vec3(0.06f, 0.28f, 0.42f),
                           glm::vec3(0.16f, 0.18f, 0.19f), 90.0f,
                           glm::vec3(1.0f, 0.0f, 0.0f));
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
