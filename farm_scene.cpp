#include "farm_scene.h"
#include "lighting_system.h"
#include "material.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <array>
#include <cmath>

namespace
{
constexpr float EntranceZ = 15.5f;
constexpr float LeftHingeX = -3.82f;
constexpr float RightHingeX = 3.82f;

const glm::vec3 WarmWood(0.88f, 0.77f, 0.62f);
const glm::vec3 DarkMetal(0.70f, 0.74f, 0.75f);
const glm::vec3 BrickTint(0.88f, 0.72f, 0.62f);
const glm::vec3 RoofTint(0.68f, 0.74f, 0.78f);
const glm::vec3 ConcreteTint(0.88f, 0.88f, 0.86f);
const glm::vec3 HayTint(1.00f, 0.84f, 0.48f);
const glm::vec3 LeafTint(0.68f, 0.92f, 0.62f);

using Glyph = std::array<const char*, 7>;

Glyph glyphFor(char character)
{
    switch (character)
    {
    case 'A': return {"01110", "10001", "10001", "11111", "10001", "10001", "10001"};
    case 'F': return {"11111", "10000", "10000", "11110", "10000", "10000", "10000"};
    case 'G': return {"01110", "10001", "10000", "10111", "10001", "10001", "01110"};
    case 'M': return {"10001", "11011", "10101", "10101", "10001", "10001", "10001"};
    case 'O': return {"01110", "10001", "10001", "10001", "10001", "10001", "01110"};
    case 'R': return {"11110", "10001", "10001", "11110", "10100", "10010", "10001"};
    case 'S': return {"01111", "10000", "10000", "01110", "00001", "00001", "11110"};
    default:  return {"00000", "00000", "00000", "00000", "00000", "00000", "00000"};
    }
}
}

const std::array<glm::vec3, 4>& FarmScene::futureCowPositions()
{
    static const std::array<glm::vec3, 4> positions = {
        glm::vec3(-12.0f, 0.0f, -7.0f),
        glm::vec3( -9.0f, 0.0f, -7.5f),
        glm::vec3(  2.0f, 0.0f, -7.0f),
        glm::vec3(  6.0f, 0.0f, -4.5f)
    };
    return positions;
}

void FarmScene::render(const Shader& shader, float gateAngleDegrees, float fanAngleDegrees,
                       float pointFixtureEmission, float spotlightFixtureEmission) const
{
    applyMaterial(shader, Materials::grass());
    drawGround(shader);
    applyMaterial(shader, Materials::soil());
    drawRoad(shader);
    applyMaterial(shader, Materials::wood());
    drawFarmBoundary(shader);
    applyMaterial(shader, Materials::brick());
    drawEntrance(shader, gateAngleDegrees);
    drawCowShed(shader);
    drawBarn(shader);
    applyMaterial(shader, Materials::concrete());
    drawFeedingArea(shader);
    drawIndoorCowStalls(shader);
    applyMaterial(shader, Materials::hay());
    drawHayArea(shader);
    applyMaterial(shader, Materials::metal());
    drawWaterFacility(shader);
    applyMaterial(shader, Materials::wood());
    drawTrees(shader);
    applyMaterial(shader, Materials::metal());
    drawShedFans(shader, fanAngleDegrees);
    drawLightFixtures(shader, pointFixtureEmission, spotlightFixtureEmission);
    applyMaterial(shader, Materials::wood());
    drawFarmProps(shader);
}

void FarmScene::drawGround(const Shader& shader) const
{
    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, -0.15f, 0.0f),
        glm::vec3(36.0f, 0.25f, 36.0f),
        textures_.grass,
        glm::vec3(0.82f, 0.92f, 0.78f),
        glm::vec2(14.0f, 14.0f));

    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, -0.16f, 19.0f),
        glm::vec3(14.0f, 0.20f, 2.0f),
        textures_.grass,
        glm::vec3(0.76f, 0.88f, 0.72f),
        glm::vec2(5.0f, 1.0f));
}

void FarmScene::drawRoad(const Shader& shader) const
{
    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, 0.025f, 9.0f),
        glm::vec3(4.8f, 0.08f, 22.0f),
        textures_.soil,
        glm::vec3(0.88f, 0.80f, 0.68f),
        glm::vec2(2.0f, 11.0f));
    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, 0.026f, 19.0f),
        glm::vec3(7.0f, 0.085f, 2.0f),
        textures_.soil,
        glm::vec3(0.84f, 0.76f, 0.64f),
        glm::vec2(3.0f, 1.0f));
}

void FarmScene::drawFarmBoundary(const Shader& shader) const
{
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, EntranceZ), glm::vec3(-4.75f, 0.0f, EntranceZ));
    drawFenceRun(shader, glm::vec3(4.75f, 0.0f, EntranceZ), glm::vec3(18.0f, 0.0f, EntranceZ));
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, -18.0f), glm::vec3(-18.0f, 0.0f, EntranceZ));
    drawFenceRun(shader, glm::vec3(18.0f, 0.0f, -18.0f), glm::vec3(18.0f, 0.0f, EntranceZ));
    drawFenceRun(shader, glm::vec3(-18.0f, 0.0f, -18.0f), glm::vec3(18.0f, 0.0f, -18.0f));
}

void FarmScene::drawFenceRun(const Shader& shader, const glm::vec3& start, const glm::vec3& end) const
{
    const glm::vec3 difference = end - start;
    const float length = glm::length(difference);
    const glm::vec3 direction = difference / length;
    const int sectionCount = std::max(1, static_cast<int>(std::ceil(length / 2.7f)));
    const float sectionLength = length / static_cast<float>(sectionCount);
    const bool runsAlongX = std::abs(difference.x) > std::abs(difference.z);

    for (int i = 0; i <= sectionCount; ++i)
    {
        const glm::vec3 point = start + direction * (sectionLength * static_cast<float>(i));
        cubes_.drawTextured(
            shader,
            glm::vec3(point.x, 0.86f, point.z),
            glm::vec3(0.23f, 1.72f, 0.23f),
            textures_.wood,
            WarmWood,
            glm::vec2(0.6f, 2.2f));

        cubes_.drawTextured(
            shader,
            glm::vec3(point.x, 1.78f, point.z),
            glm::vec3(0.31f, 0.16f, 0.31f),
            textures_.wood,
            glm::vec3(0.72f, 0.60f, 0.45f));
    }

    for (int i = 0; i < sectionCount; ++i)
    {
        const glm::vec3 center = start + direction * (sectionLength * (static_cast<float>(i) + 0.5f));
        for (const float height : {0.55f, 1.15f})
        {
            const glm::vec3 scale = runsAlongX
                ? glm::vec3(sectionLength + 0.04f, 0.15f, 0.16f)
                : glm::vec3(0.16f, 0.15f, sectionLength + 0.04f);
            cubes_.drawTextured(
                shader,
                glm::vec3(center.x, height, center.z),
                scale,
                textures_.wood,
                WarmWood,
                glm::vec2(sectionLength, 0.5f));
        }
    }
}

void FarmScene::drawEntrance(const Shader& shader, float gateAngleDegrees) const
{
    applyMaterial(shader, Materials::brick());
    drawPillar(shader, -4.30f);
    drawPillar(shader, 4.30f);
    applyMaterial(shader, Materials::wood());
    drawFarmSign(shader);

    applyMaterial(shader, Materials::metal());

    glm::mat4 leftParent(1.0f);
    leftParent = glm::translate(leftParent, glm::vec3(LeftHingeX, 0.0f, EntranceZ - 0.03f));
    leftParent = glm::rotate(leftParent, glm::radians(gateAngleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    drawGateLeaf(shader, leftParent, 1.0f);

    glm::mat4 rightParent(1.0f);
    rightParent = glm::translate(rightParent, glm::vec3(RightHingeX, 0.0f, EntranceZ - 0.03f));
    rightParent = glm::rotate(rightParent, glm::radians(-gateAngleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    drawGateLeaf(shader, rightParent, -1.0f);
}

void FarmScene::drawPillar(const Shader& shader, float xPosition) const
{
    cubes_.drawTextured(shader, glm::vec3(xPosition, 0.16f, EntranceZ), glm::vec3(1.18f, 0.32f, 1.18f), textures_.concrete, ConcreteTint);
    cubes_.drawTextured(shader, glm::vec3(xPosition, 2.25f, EntranceZ), glm::vec3(0.86f, 4.20f, 0.86f), textures_.brick, BrickTint, glm::vec2(2.0f, 6.0f));
    cubes_.drawTextured(shader, glm::vec3(xPosition, 4.42f, EntranceZ), glm::vec3(1.12f, 0.28f, 1.12f), textures_.concrete, ConcreteTint);
    cubes_.drawTextured(shader, glm::vec3(xPosition, 4.68f, EntranceZ), glm::vec3(0.78f, 0.28f, 0.78f), textures_.brick, BrickTint);
}

void FarmScene::drawGateLeaf(const Shader& shader, const glm::mat4& hingeTransform, float direction) const
{
    constexpr float leafWidth = 3.82f;
    constexpr float bottom = 0.35f;
    constexpr float height = 2.55f;

    for (const float xDistance : {0.12f, leafWidth - 0.12f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * xDistance, bottom + height * 0.5f, 0.0f), glm::vec3(0.16f, height, 0.16f), textures_.metal, DarkMetal, glm::vec2(0.5f, 3.0f));

    for (const float y : {bottom + 0.12f, bottom + height * 0.50f, bottom + height - 0.12f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * leafWidth * 0.5f, y, 0.0f), glm::vec3(leafWidth, 0.16f, 0.16f), textures_.metal, DarkMetal, glm::vec2(4.0f, 0.5f));

    for (int bar = 1; bar <= 5; ++bar)
    {
        const float distance = leafWidth * static_cast<float>(bar) / 6.0f;
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(direction * distance, bottom + height * 0.5f, 0.0f), glm::vec3(0.09f, height - 0.18f, 0.09f), textures_.metal, glm::vec3(0.86f, 0.72f, 0.34f), glm::vec2(0.4f, 3.0f));
    }

    for (const float y : {0.72f, 2.42f})
        cubes_.drawTextured(shader, hingeTransform, glm::vec3(0.0f, y, 0.0f), glm::vec3(0.24f, 0.28f, 0.26f), textures_.metal, glm::vec3(0.38f));
}

void FarmScene::drawFarmSign(const Shader& shader) const
{
    cubes_.drawTextured(shader, glm::vec3(0.0f, 4.72f, EntranceZ), glm::vec3(8.0f, 1.20f, 0.32f), textures_.wood, WarmWood, glm::vec2(4.0f, 1.0f));
    cubes_.drawTextured(shader, glm::vec3(0.0f, 4.72f, EntranceZ + 0.18f), glm::vec3(7.70f, 0.90f, 0.08f), textures_.wood, glm::vec3(0.64f, 0.50f, 0.32f), glm::vec2(3.0f, 1.0f));
    drawBlockText(shader, "AMS AGRO FARM", glm::vec3(0.0f, 4.72f, EntranceZ + 0.245f));
}

void FarmScene::drawBlockText(const Shader& shader, const char* text, const glm::vec3& center) const
{
    constexpr float step = 0.082f;
    constexpr float size = 0.074f;
    constexpr float advance = 6.0f * step;
    constexpr float space = 3.0f * step;
    float width = 0.0f;
    for (const char* cursor = text; *cursor; ++cursor)
        width += (*cursor == ' ') ? space : advance;

    float penX = center.x - width * 0.5f;
    for (const char* cursor = text; *cursor; ++cursor)
    {
        if (*cursor == ' ')
        {
            penX += space;
            continue;
        }
        const Glyph glyph = glyphFor(*cursor);
        for (int row = 0; row < 7; ++row)
            for (int column = 0; column < 5; ++column)
                if (glyph[row][column] == '1')
                    cubes_.drawColored(shader, glm::vec3(penX + column * step, center.y + (3.0f - row) * step, center.z), glm::vec3(size, size, 0.055f), glm::vec3(1.0f, 0.86f, 0.34f));
        penX += advance;
    }
}

void FarmScene::drawCowShed(const Shader& shader) const
{
    const glm::vec3 origin(-10.0f, 0.0f, -8.0f);

    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.09f, 0.0f), glm::vec3(8.2f, 0.18f, 7.0f), textures_.concrete, ConcreteTint, glm::vec2(5.0f, 4.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 1.55f, -3.18f), glm::vec3(7.5f, 3.0f, 0.24f), textures_.brick, BrickTint, glm::vec2(5.0f, 2.5f));
    cubes_.drawTextured(shader, origin + glm::vec3(-3.78f, 1.0f, -1.7f), glm::vec3(0.24f, 1.8f, 2.8f), textures_.brick, BrickTint, glm::vec2(2.0f, 2.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(3.78f, 1.0f, -1.7f), glm::vec3(0.24f, 1.8f, 2.8f), textures_.brick, BrickTint, glm::vec2(2.0f, 2.0f));

    for (const float x : {-3.65f, 0.0f, 3.65f})
        for (const float z : {-3.0f, 3.0f})
            cubes_.drawTextured(shader, origin + glm::vec3(x, 1.85f, z), glm::vec3(0.28f, 3.7f, 0.28f), textures_.metal, glm::vec3(0.76f), glm::vec2(0.6f, 4.0f));

    for (const float z : {-3.0f, 3.0f})
        cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.55f, z), glm::vec3(7.7f, 0.28f, 0.28f), textures_.wood, WarmWood, glm::vec2(6.0f, 0.5f));
    for (const float x : {-3.5f, 0.0f, 3.5f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 3.55f, 0.0f), glm::vec3(0.25f, 0.25f, 6.2f), textures_.wood, WarmWood, glm::vec2(5.0f, 0.5f));

    cubes_.drawTextured(shader, origin + glm::vec3(-1.95f, 4.10f, 0.0f), glm::vec3(4.35f, 0.24f, 7.2f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), 14.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(1.95f, 4.10f, 0.0f), glm::vec3(4.35f, 0.24f, 7.2f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), -14.0f, glm::vec3(0.0f, 0.0f, 1.0f));
}

void FarmScene::drawBarn(const Shader& shader) const
{
    const glm::vec3 origin(11.0f, 0.0f, -11.0f);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f), glm::vec3(7.0f, 0.20f, 6.5f), textures_.concrete, ConcreteTint, glm::vec2(4.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(-3.15f, 2.1f, 0.0f), glm::vec3(0.32f, 4.0f, 6.0f), textures_.brick, BrickTint, glm::vec2(4.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(3.15f, 2.1f, 0.0f), glm::vec3(0.32f, 4.0f, 6.0f), textures_.brick, BrickTint, glm::vec2(4.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 2.1f, -3.0f), glm::vec3(6.0f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(5.0f, 3.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(-2.25f, 2.1f, 3.0f), glm::vec3(1.8f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(2.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(2.25f, 2.1f, 3.0f), glm::vec3(1.8f, 4.0f, 0.32f), textures_.brick, BrickTint, glm::vec2(2.0f, 3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.50f, 3.0f), glm::vec3(2.7f, 1.2f, 0.32f), textures_.brick, BrickTint, glm::vec2(3.0f, 1.0f));

    cubes_.drawColored(shader, origin + glm::vec3(0.0f, 1.45f, 3.18f), glm::vec3(2.35f, 2.70f, 0.08f), glm::vec3(0.075f, 0.055f, 0.035f));
    for (const float x : {-1.28f, 1.28f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.50f, 3.25f), glm::vec3(0.16f, 3.0f, 0.14f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 2.95f, 3.25f), glm::vec3(2.72f, 0.16f, 0.14f), textures_.wood, WarmWood);

    cubes_.drawTextured(shader, origin + glm::vec3(-1.65f, 4.45f, 0.0f), glm::vec3(3.75f, 0.28f, 6.8f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), 27.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(1.65f, 4.45f, 0.0f), glm::vec3(3.75f, 0.28f, 6.8f), textures_.roof, RoofTint, glm::vec2(3.0f, 5.0f), -27.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    // Simple shuttered windows visually distinguish the enclosed barn.
    for (const float x : {-2.15f, 2.15f})
    {
        cubes_.drawColored(shader, origin + glm::vec3(x, 2.65f, 3.19f), glm::vec3(0.80f, 0.65f, 0.07f), glm::vec3(0.20f, 0.34f, 0.38f));
        cubes_.drawTextured(shader, origin + glm::vec3(x, 2.65f, 3.24f), glm::vec3(0.08f, 0.82f, 0.09f), textures_.wood, WarmWood);
    }
}

void FarmScene::drawFeedingArea(const Shader& shader) const
{
    drawTrough(shader, glm::vec3(-11.8f, 0.0f, -3.75f), 3.0f);
    drawTrough(shader, glm::vec3(-8.2f, 0.0f, -3.75f), 3.0f);
}

void FarmScene::drawTrough(const Shader& shader, const glm::vec3& position, float length) const
{
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.18f, 0.0f), glm::vec3(length, 0.18f, 0.90f), textures_.concrete, ConcreteTint, glm::vec2(3.0f, 1.0f));
    for (const float z : {-0.48f, 0.48f})
        cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.53f, z), glm::vec3(length, 0.70f, 0.14f), textures_.concrete, ConcreteTint, glm::vec2(3.0f, 1.0f));
    for (const float x : {-0.5f * length, 0.5f * length})
        cubes_.drawTextured(shader, position + glm::vec3(x, 0.53f, 0.0f), glm::vec3(0.14f, 0.70f, 1.10f), textures_.concrete, ConcreteTint);
    cubes_.drawColored(shader, position + glm::vec3(0.0f, 0.38f, 0.0f), glm::vec3(length - 0.22f, 0.08f, 0.72f), glm::vec3(0.30f, 0.22f, 0.08f));
}

void FarmScene::drawIndoorCowStalls(const Shader& shader) const
{
    // Two open-front stalls align with the tied cows and can be inspected by
    // walking into the shed from its southern/front opening.
    applyMaterial(shader, Materials::wood());
    for (const float x : {-14.0f, -10.0f, -6.0f})
    {
        cubes_.drawTextured(shader, glm::vec3(x, 0.82f, -7.65f),
                            glm::vec3(0.16f, 1.55f, 0.16f),
                            textures_.wood, WarmWood);
        cubes_.drawTextured(shader, glm::vec3(x, 0.88f, -6.55f),
                            glm::vec3(0.12f, 0.12f, 2.20f),
                            textures_.wood, WarmWood, glm::vec2(2.0f, 0.5f));
    }
    cubes_.drawTextured(shader, glm::vec3(-10.0f, 1.05f, -6.10f),
                        glm::vec3(7.9f, 0.14f, 0.14f),
                        textures_.wood, WarmWood, glm::vec2(6.0f, 0.5f));

    applyMaterial(shader, Materials::concrete());
    drawTrough(shader, glm::vec3(-12.0f, 0.0f, -5.45f), 2.55f);
    drawTrough(shader, glm::vec3( -8.0f, 0.0f, -5.45f), 2.55f);

    // Fresh cut grass/fodder is deliberately green and visibly raised above
    // the trough base so it is obvious when entering the shed.
    applyMaterial(shader, Materials::grass());
    for (const float xCenter : {-12.0f, -8.0f})
    {
        for (int strip = -2; strip <= 2; ++strip)
        {
            cubes_.drawTextured(
                shader,
                glm::vec3(xCenter + strip * 0.42f, 0.48f + std::abs(strip) * 0.015f, -5.45f),
                glm::vec3(0.50f, 0.15f, 0.62f), textures_.hay,
                glm::vec3(0.30f, 0.72f, 0.18f), glm::vec2(1.0f),
                static_cast<float>(strip) * 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
}

void FarmScene::drawHayArea(const Shader& shader) const
{
    drawHayBale(shader, glm::vec3(-10.2f, 0.52f, 2.0f), -6.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 2.2f), 4.0f);
    drawHayBale(shader, glm::vec3(-5.9f, 0.52f, 2.0f), -3.0f);
    drawHayBale(shader, glm::vec3(-9.1f, 1.48f, 2.1f), 3.0f);
    drawHayBale(shader, glm::vec3(-6.9f, 1.48f, 2.1f), -5.0f);
    drawHayBale(shader, glm::vec3(-8.0f, 0.52f, 4.0f), 8.0f);
    drawHayBale(shader, glm::vec3(-6.0f, 0.52f, 4.1f), -7.0f);
}

void FarmScene::drawHayBale(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(1.9f, 0.90f, 1.15f), textures_.hay, HayTint, glm::vec2(2.0f, 1.0f));
    cubes_.drawColored(shader, parent, glm::vec3(-0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
    cubes_.drawColored(shader, parent, glm::vec3(0.45f, 0.0f, 0.585f), glm::vec3(0.06f, 0.94f, 0.035f), glm::vec3(0.28f, 0.18f, 0.06f));
}

void FarmScene::drawTrees(const Shader& shader) const
{
    const std::array<std::pair<glm::vec3, float>, 8> trees = {{
        {glm::vec3(-15.8f, 0.0f, -14.5f), 1.05f},
        {glm::vec3(-15.7f, 0.0f,  7.5f), 0.90f},
        {glm::vec3(-14.2f, 0.0f, 13.0f), 0.82f},
        {glm::vec3( 15.8f, 0.0f,-15.4f), 1.00f},
        {glm::vec3( 16.0f, 0.0f,  6.0f), 0.90f},
        {glm::vec3( 13.6f, 0.0f, 12.5f), 0.78f},
        {glm::vec3( -2.0f, 0.0f,-16.0f), 0.92f},
        {glm::vec3(  5.5f, 0.0f,-16.1f), 0.82f}
    }};
    for (const auto& tree : trees)
        drawTree(shader, tree.first, tree.second);
}

void FarmScene::drawTree(const Shader& shader, const glm::vec3& position, float scale) const
{
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 1.9f * scale, 0.0f), glm::vec3(0.55f, 3.8f, 0.55f) * scale, textures_.bark, glm::vec3(0.88f, 0.78f, 0.66f), glm::vec2(1.0f, 3.0f));
    cubes_.drawTextured(shader, position + glm::vec3(0.0f, 4.2f * scale, 0.0f), glm::vec3(3.2f, 2.2f, 3.0f) * scale, textures_.leaves, LeafTint, glm::vec2(1.0f));
    cubes_.drawTextured(shader, position + glm::vec3(-1.15f * scale, 4.0f * scale, 0.35f * scale), glm::vec3(2.1f, 1.8f, 2.1f) * scale, textures_.leaves, glm::vec3(0.62f, 0.86f, 0.56f), glm::vec2(1.0f), 18.0f);
    cubes_.drawTextured(shader, position + glm::vec3(1.05f * scale, 4.35f * scale, -0.30f * scale), glm::vec3(2.0f, 1.7f, 2.2f) * scale, textures_.leaves, glm::vec3(0.72f, 0.94f, 0.64f), glm::vec2(1.0f), -15.0f);
}

void FarmScene::drawWaterFacility(const Shader& shader) const
{
    const glm::vec3 origin(13.0f, 0.0f, 0.5f);
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.10f, 0.0f), glm::vec3(5.0f, 0.20f, 4.5f), textures_.concrete, ConcreteTint, glm::vec2(3.0f));

    for (const float x : {-1.45f, 1.45f})
        for (const float z : {-1.25f, 1.25f})
            cubes_.drawTextured(shader, origin + glm::vec3(x, 2.0f, z), glm::vec3(0.28f, 3.8f, 0.28f), textures_.metal, DarkMetal, glm::vec2(0.5f, 4.0f));

    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 3.85f, 0.0f), glm::vec3(4.0f, 2.5f, 3.4f), textures_.metal, glm::vec3(0.56f, 0.76f, 0.88f), glm::vec2(3.0f, 2.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 5.18f, 0.0f), glm::vec3(4.25f, 0.20f, 3.65f), textures_.roof, RoofTint, glm::vec2(3.0f));
    cubes_.drawColored(shader, origin + glm::vec3(0.0f, 3.85f, 1.73f), glm::vec3(3.7f, 0.32f, 0.08f), glm::vec3(0.16f, 0.42f, 0.65f));

    // Ladder and lower cross-bracing make the block-built tank read as a tower.
    for (const float x : {-0.38f, 0.38f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 2.05f, 1.70f), glm::vec3(0.10f, 4.0f, 0.10f), textures_.metal, glm::vec3(0.68f));
    for (int rung = 0; rung < 7; ++rung)
        cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.55f + rung * 0.52f, 1.72f), glm::vec3(0.86f, 0.09f, 0.09f), textures_.metal, glm::vec3(0.68f));
}

void FarmScene::drawShedFans(const Shader& shader, float fanAngleDegrees) const
{
    drawFan(shader, glm::vec3(-12.1f, 3.15f, -7.0f), fanAngleDegrees);
    drawFan(shader, glm::vec3(-7.9f, 3.15f, -7.0f), -fanAngleDegrees);
}

void FarmScene::drawFan(const Shader& shader, const glm::vec3& position, float parentAngle) const
{
    glm::mat4 mount(1.0f);
    mount = glm::translate(mount, position);
    cubes_.drawTextured(shader, mount, glm::vec3(0.0f, 0.48f, 0.0f),
                        glm::vec3(0.12f, 0.95f, 0.12f), textures_.metal, DarkMetal);

    // Only the rotor inherits this transform; the ceiling mount stays fixed.
    glm::mat4 rotor = glm::rotate(mount, glm::radians(parentAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f), glm::vec3(0.42f, 0.22f, 0.42f), textures_.metal, glm::vec3(0.58f));
    cubes_.drawTextured(shader, rotor, glm::vec3(1.05f, 0.0f, 0.0f), glm::vec3(1.85f, 0.10f, 0.38f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(-1.05f, 0.0f, 0.0f), glm::vec3(1.85f, 0.10f, 0.38f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f, 0.0f, 1.05f), glm::vec3(0.38f, 0.10f, 1.85f), textures_.metal, DarkMetal);
    cubes_.drawTextured(shader, rotor, glm::vec3(0.0f, 0.0f, -1.05f), glm::vec3(0.38f, 0.10f, 1.85f), textures_.metal, DarkMetal);
}

void FarmScene::drawLightFixtures(const Shader& shader, float pointEmission,
                                  float spotlightEmission) const
{
    const glm::vec3 warmBulb(1.0f, 0.78f, 0.40f);
    applyMaterial(shader, Materials::metal());
    for (const glm::vec3& position : LightingSystem::pointLightPositions())
    {
        cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.30f, 0.0f),
                            glm::vec3(0.09f, 0.58f, 0.09f),
                            textures_.metal, glm::vec3(0.55f));
        cubes_.drawTextured(shader, position + glm::vec3(0.0f, 0.07f, 0.0f),
                            glm::vec3(0.56f, 0.18f, 0.56f),
                            textures_.metal, DarkMetal);

        Material bulbMaterial = Materials::metal();
        bulbMaterial.emission = warmBulb * pointEmission;
        applyMaterial(shader, bulbMaterial);
        cubes_.drawColored(shader, position + glm::vec3(0.0f, -0.12f, 0.0f),
                           glm::vec3(0.34f, 0.24f, 0.34f), warmBulb);
        applyMaterial(shader, Materials::metal());
    }

    const glm::vec3 spotPosition = LightingSystem::spotlightPosition();
    cubes_.drawTextured(shader, spotPosition + glm::vec3(0.0f, 0.20f, -0.12f),
                        glm::vec3(0.18f, 0.42f, 0.52f),
                        textures_.metal, DarkMetal, glm::vec2(1.0f),
                        -38.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    Material spotMaterial = Materials::metal();
    spotMaterial.emission = glm::vec3(1.0f, 0.92f, 0.64f) * spotlightEmission;
    applyMaterial(shader, spotMaterial);
    cubes_.drawColored(shader, spotPosition, glm::vec3(0.42f, 0.26f, 0.42f),
                       glm::vec3(1.0f, 0.92f, 0.64f), -38.0f,
                       glm::vec3(1.0f, 0.0f, 0.0f));
    applyMaterial(shader, Materials::metal());
}

void FarmScene::drawFarmProps(const Shader& shader) const
{
    drawCrate(shader, glm::vec3(7.1f, 0.55f, -7.3f), -7.0f);
    drawCrate(shader, glm::vec3(8.5f, 0.55f, -7.0f), 5.0f);
    drawCrate(shader, glm::vec3(7.8f, 1.62f, -7.2f), 2.0f);
    drawFeedSack(shader, glm::vec3(10.0f, 0.55f, -7.2f), -8.0f);
    drawFeedSack(shader, glm::vec3(11.0f, 0.55f, -7.1f), 7.0f);
    drawFeedSack(shader, glm::vec3(10.5f, 1.45f, -7.15f), 1.0f);

    // Tool rack near the barn and an empty collection pad reserved for Module 5.
    cubes_.drawTextured(shader, glm::vec3(15.2f, 1.2f, -6.9f), glm::vec3(0.18f, 2.4f, 0.18f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(16.2f, 1.2f, -6.9f), glm::vec3(0.18f, 2.4f, 0.18f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(15.7f, 1.65f, -6.9f), glm::vec3(1.25f, 0.16f, 0.16f), textures_.wood, WarmWood);
    cubes_.drawTextured(shader, glm::vec3(8.3f, 0.06f, -1.0f), glm::vec3(3.4f, 0.12f, 2.6f), textures_.concrete, ConcreteTint, glm::vec2(2.0f));
}

void FarmScene::drawCrate(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(1.2f, 1.0f, 1.1f), textures_.wood, WarmWood, glm::vec2(1.5f));
    for (const float x : {-0.52f, 0.52f})
        cubes_.drawTextured(shader, parent, glm::vec3(x, 0.0f, 0.56f), glm::vec3(0.10f, 1.08f, 0.08f), textures_.wood, glm::vec3(0.62f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f, 0.0f, 0.57f), glm::vec3(1.12f, 0.10f, 0.08f), textures_.wood, glm::vec3(0.62f));
}

void FarmScene::drawFeedSack(const Shader& shader, const glm::vec3& position, float rotation) const
{
    glm::mat4 parent(1.0f);
    parent = glm::translate(parent, position);
    parent = glm::rotate(parent, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    cubes_.drawTextured(shader, parent, glm::vec3(0.0f), glm::vec3(0.78f, 1.0f, 0.50f), textures_.hay, glm::vec3(0.94f, 0.86f, 0.68f));
    cubes_.drawColored(shader, parent, glm::vec3(0.0f, 0.0f, 0.26f), glm::vec3(0.58f, 0.10f, 0.04f), glm::vec3(0.36f, 0.24f, 0.10f));
}
