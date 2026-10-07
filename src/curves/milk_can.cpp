// Item: milk can curved mesh, equations and placement.
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

namespace
{
glm::vec2 cubicBezier(const array<glm::vec2, 4>& points, float t)
{
    const float s = 1.0f - t;
    return s * s * s * points[0]
         + 3.0f * s * s * t * points[1]
         + 3.0f * s * t * t * points[2]
         + t * t * t * points[3];
}

glm::vec2 cubicBezierDerivative(const array<glm::vec2, 4>& points, float t)
{
    const float s = 1.0f - t;
    return 3.0f * s * s * (points[1] - points[0])
         + 6.0f * s * t * (points[2] - points[1])
         + 3.0f * t * t * (points[3] - points[2]);
}
}

CurvedRenderer::Mesh CurvedRenderer::createMilkCan()
{
    // Three joined cubic Bézier segments define radius against height.
    const array<array<glm::vec2, 4>, 3> segments = {{
        {{glm::vec2(0.34f, 0.00f), glm::vec2(0.47f, 0.05f),
          glm::vec2(0.50f, 0.30f), glm::vec2(0.50f, 0.76f)}},
        {{glm::vec2(0.50f, 0.76f), glm::vec2(0.50f, 1.02f),
          glm::vec2(0.33f, 1.12f), glm::vec2(0.29f, 1.35f)}},
        {{glm::vec2(0.29f, 1.35f), glm::vec2(0.27f, 1.52f),
          glm::vec2(0.39f, 1.58f), glm::vec2(0.36f, 1.70f)}}
    }};
    constexpr unsigned int profileSamples = 12;
    constexpr unsigned int radialSamples = 32;
    vector<glm::vec4> profile; // radius, height, dr, dy
    for (unsigned int segment = 0; segment < segments.size(); ++segment)
    {
        for (unsigned int sample = (segment == 0 ? 0U : 1U);
             sample <= profileSamples; ++sample)
        {
            const float t = static_cast<float>(sample) / profileSamples;
            const glm::vec2 point = cubicBezier(segments[segment], t);
            const glm::vec2 tangent = cubicBezierDerivative(segments[segment], t);
            profile.push_back(glm::vec4(point.x, point.y, tangent.x, tangent.y));
        }
    }

    vector<float> vertices;
    vector<unsigned int> indices;
    for (unsigned int row = 0; row < profile.size(); ++row)
    {
        for (unsigned int ring = 0; ring <= radialSamples; ++ring)
        {
            const float u = static_cast<float>(ring) / radialSamples;
            const float angle = u * glm::two_pi<float>();
            const float c = cos(angle);
            const float s = sin(angle);
            const glm::vec4 p = profile[row];
            glm::vec3 normal(p.w * c, -p.z, p.w * s);
            normal = glm::normalize(normal);
            vertex(vertices, glm::vec3(p.x * c, p.y, p.x * s), normal,
                   glm::vec2(u, static_cast<float>(row) / (profile.size() - 1)));
        }
    }
    for (unsigned int row = 0; row + 1 < profile.size(); ++row)
    {
        for (unsigned int ring = 0; ring < radialSamples; ++ring)
        {
            const unsigned int a = row * (radialSamples + 1) + ring;
            const unsigned int b = a + radialSamples + 1;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    return upload(vertices.data(), static_cast<unsigned int>(vertices.size()),
                  indices.data(), static_cast<unsigned int>(indices.size()));
}

void CurvedRenderer::drawMilkCan(const Shader& shader, const FarmTextures& textures) const
{
    glm::mat4 milkCan(1.0f);
    milkCan = glm::translate(milkCan, glm::vec3(8.3f, 0.13f, -1.0f));
    milkCan = glm::scale(milkCan, glm::vec3(1.25f));
    draw(milkCan_, shader, milkCan, textures.metal, glm::vec3(0.78f, 0.82f, 0.84f), glm::vec2(2.0f, 1.0f));
}
