// Item: block text.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;
using namespace FarmStyle;

namespace
{
using Glyph = array<const char*, 7>;

Glyph glyphFor(char character)
{
    switch (character)
    {
    case 'A': return {"01110", "10001", "10001", "11111", "10001", "10001", "10001"};
    case 'C': return {"01111", "10000", "10000", "10000", "10000", "10000", "01111"};
    case 'D': return {"11110", "10001", "10001", "10001", "10001", "10001", "11110"};
    case 'E': return {"11111", "10000", "10000", "11110", "10000", "10000", "11111"};
    case 'F': return {"11111", "10000", "10000", "11110", "10000", "10000", "10000"};
    case 'G': return {"01110", "10001", "10000", "10111", "10001", "10001", "01110"};
    case 'H': return {"10001", "10001", "10001", "11111", "10001", "10001", "10001"};
    case 'L': return {"10000", "10000", "10000", "10000", "10000", "10000", "11111"};
    case 'M': return {"10001", "11011", "10101", "10101", "10001", "10001", "10001"};
    case 'N': return {"10001", "11001", "11001", "10101", "10011", "10011", "10001"};
    case 'O': return {"01110", "10001", "10001", "10001", "10001", "10001", "01110"};
    case 'P': return {"11110", "10001", "10001", "11110", "10000", "10000", "10000"};
    case 'R': return {"11110", "10001", "10001", "11110", "10100", "10010", "10001"};
    case 'S': return {"01111", "10000", "10000", "01110", "00001", "00001", "11110"};
    case 'U': return {"10001", "10001", "10001", "10001", "10001", "10001", "01110"};
    case 'W': return {"10001", "10001", "10001", "10101", "10101", "11011", "10001"};
    case ':': return {"00000", "00100", "00100", "00000", "00100", "00100", "00000"};
    case '.': return {"00000", "00000", "00000", "00000", "00000", "00110", "00110"};
    default:  return {"00000", "00000", "00000", "00000", "00000", "00000", "00000"};
    }
}
}

void FarmScene::drawBlockText(const Shader& shader, const char* text,
                              const glm::vec3& center, float scale) const
{
    const float step = 0.082f * scale;
    const float size = 0.074f * scale;
    const float advance = 6.0f * step;
    const float space = 3.0f * step;
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
                    cubes_.drawColored(shader,
                                       glm::vec3(penX + column * step,
                                                 center.y + (3.0f - row) * step,
                                                 center.z),
                                       glm::vec3(size, size, 0.055f),
                                       glm::vec3(1.0f, 0.86f, 0.34f));
        penX += advance;
    }
}

void FarmScene::drawCurvedBlockText(const Shader& shader, const char* text,
                                    const glm::vec3& center, float scale,
                                    float halfWidth, float curveDepth,
                                    float xOffset) const
{
    const float step = 0.082f * scale;
    const float size = 0.074f * scale;
    const float advance = 6.0f * step;
    const float space = 3.0f * step;
    float width = 0.0f;
    for (const char* cursor = text; *cursor; ++cursor)
        width += (*cursor == ' ') ? space : advance;

    float penX = center.x - width * 0.5f + xOffset;
    for (const char* cursor = text; *cursor; ++cursor)
    {
        if (*cursor == ' ')
        {
            penX += space;
            continue;
        }
        const Glyph glyph = glyphFor(*cursor);
        for (int row = 0; row < 7; ++row)
        {
            for (int column = 0; column < 5; ++column)
            {
                if (glyph[row][column] != '1')
                    continue;
                const float x = penX + column * step;
                const float localX = glm::clamp(x - center.x, -halfWidth, halfWidth);
                const float normalized = localX / halfWidth;
                const float z = center.z + curveDepth *
                    (1.0f - normalized * normalized) + 0.19f;
                const float slope = -2.0f * curveDepth * localX /
                                    (halfWidth * halfWidth);
                const float angle = -glm::degrees(atan(slope));
                cubes_.drawColored(shader,
                                   glm::vec3(x, center.y + (3.0f - row) * step, z),
                                   glm::vec3(size, size, 0.055f),
                                   glm::vec3(1.0f, 0.86f, 0.34f), angle,
                                   glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }
        penX += advance;
    }
}
