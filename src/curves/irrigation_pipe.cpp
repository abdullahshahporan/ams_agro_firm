// Item: irrigation pipe curved mesh, equations and placement.
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
glm::vec3 cubicBSpline(const array<glm::vec3, 4>& points, float t)
{
    const float t2 = t * t;
    const float t3 = t2 * t;
    const float b0 = (-t3 + 3.0f * t2 - 3.0f * t + 1.0f) / 6.0f;
    const float b1 = ( 3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f;
    const float b2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f;
    const float b3 = t3 / 6.0f;
    return b0 * points[0] + b1 * points[1] + b2 * points[2] + b3 * points[3];
}
}

CurvedRenderer::Mesh CurvedRenderer::createSplinePipe()
{
    const array<glm::vec3, 7> controls = {{
        glm::vec3(7.2f, 0.45f, 3.8f), glm::vec3(8.0f, 0.52f, 3.3f),
        glm::vec3(9.2f, 0.65f, 2.8f), glm::vec3(10.7f, 0.95f, 2.3f),
        glm::vec3(12.0f, 1.45f, 1.8f), glm::vec3(12.8f, 2.20f, 1.2f),
        glm::vec3(13.0f, 3.10f, 0.6f)
    }};
    constexpr unsigned int samplesPerSegment = 10;
    constexpr unsigned int ringSamples = 12;
    vector<glm::vec3> centers;
    for (unsigned int segment = 0; segment + 3 < controls.size(); ++segment)
    {
        const array<glm::vec3, 4> points = {{
            controls[segment], controls[segment + 1],
            controls[segment + 2], controls[segment + 3]}};
        for (unsigned int sample = (segment == 0 ? 0U : 1U);
             sample <= samplesPerSegment; ++sample)
            centers.push_back(cubicBSpline(points,
                static_cast<float>(sample) / samplesPerSegment));
    }

    vector<float> vertices;
    vector<unsigned int> indices;
    for (unsigned int row = 0; row < centers.size(); ++row)
    {
        const glm::vec3 before = centers[row == 0 ? row : row - 1];
        const glm::vec3 after = centers[row + 1 < centers.size() ? row + 1 : row];
        const glm::vec3 tangent = glm::normalize(after - before);
        const glm::vec3 reference = abs(tangent.y) > 0.92f
            ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::vec3 side = glm::normalize(glm::cross(tangent, reference));
        const glm::vec3 up = glm::normalize(glm::cross(side, tangent));
        for (unsigned int ring = 0; ring <= ringSamples; ++ring)
        {
            const float u = static_cast<float>(ring) / ringSamples;
            const float angle = u * glm::two_pi<float>();
            const glm::vec3 radial = side * cos(angle) + up * sin(angle);
            vertex(vertices, centers[row] + radial * 0.13f, radial,
                   glm::vec2(u, static_cast<float>(row) * 0.24f));
        }
    }
    for (unsigned int row = 0; row + 1 < centers.size(); ++row)
    {
        for (unsigned int ring = 0; ring < ringSamples; ++ring)
        {
            const unsigned int a = row * (ringSamples + 1) + ring;
            const unsigned int b = a + ringSamples + 1;
            indices.insert(indices.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    return upload(vertices.data(), static_cast<unsigned int>(vertices.size()),
                  indices.data(), static_cast<unsigned int>(indices.size()));
}

void CurvedRenderer::drawIrrigationPipe(const Shader& shader, const FarmTextures& textures) const
{
    draw(splinePipe_, shader, glm::mat4(1.0f), textures.metal,
         glm::vec3(0.18f, 0.48f, 0.64f), glm::vec2(1.0f, 1.0f));
}
