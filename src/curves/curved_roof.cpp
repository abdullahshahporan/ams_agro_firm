// Item: curved roof curved mesh, equations and placement.
#include "curved_renderer.h"
#include "shared/curve_vertex.h"
#include "material.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <cmath>
#include <vector>

using namespace std;
using namespace CurveGeometry;

CurvedRenderer::Mesh CurvedRenderer::createRuledRoof()
{
    constexpr unsigned int curveSamples = 28;
    constexpr unsigned int rulingSamples = 10;
    constexpr float halfWidth = 4.25f;
    constexpr float frontZ = 3.62f;
    constexpr float backZ = -3.62f;
    vector<float> vertices;
    vector<unsigned int> indices;
    for (unsigned int uIndex = 0; uIndex <= curveSamples; ++uIndex)
    {
        const float u = static_cast<float>(uIndex) / curveSamples;
        const float x = -halfWidth + u * 2.0f * halfWidth;
        const float normalizedX = x / halfWidth;
        const float y = 3.82f + 0.72f * (1.0f - normalizedX * normalizedX);
        const float slope = -1.44f * x / (halfWidth * halfWidth);
        const glm::vec3 normal = glm::normalize(glm::vec3(-slope, 1.0f, 0.0f));
        for (unsigned int vIndex = 0; vIndex <= rulingSamples; ++vIndex)
        {
            const float v = static_cast<float>(vIndex) / rulingSamples;
            const float z = frontZ * (1.0f - v) + backZ * v;
            vertex(vertices, glm::vec3(x, y, z), normal,
                   glm::vec2(u * 4.0f, v * 5.0f));
        }
    }
    for (unsigned int u = 0; u < curveSamples; ++u)
    {
        for (unsigned int v = 0; v < rulingSamples; ++v)
        {
            const unsigned int a = u * (rulingSamples + 1) + v;
            const unsigned int b = a + rulingSamples + 1;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    return upload(vertices.data(), static_cast<unsigned int>(vertices.size()),
                  indices.data(), static_cast<unsigned int>(indices.size()));
}

void CurvedRenderer::drawCurvedRoof(const Shader& shader, const FarmTextures& textures) const
{
    glm::mat4 roof(1.0f);
    roof = glm::translate(roof, glm::vec3(-10.0f, 0.0f, -8.0f));
    draw(ruledRoof_, shader, roof, textures.roof, glm::vec3(0.68f, 0.74f, 0.78f), glm::vec2(1.0f));
}
