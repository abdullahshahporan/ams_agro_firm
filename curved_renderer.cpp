#include "curved_renderer.h"

#include "material.h"

#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <cmath>
#include <vector>

namespace
{
void vertex(std::vector<float>& output, const glm::vec3& position,
            const glm::vec3& normal, const glm::vec2& uv)
{
    output.insert(output.end(), {
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        uv.x, uv.y});
}

glm::vec2 cubicBezier(const std::array<glm::vec2, 4>& points, float t)
{
    const float s = 1.0f - t;
    return s * s * s * points[0]
         + 3.0f * s * s * t * points[1]
         + 3.0f * s * t * t * points[2]
         + t * t * t * points[3];
}

glm::vec2 cubicBezierDerivative(const std::array<glm::vec2, 4>& points, float t)
{
    const float s = 1.0f - t;
    return 3.0f * s * s * (points[1] - points[0])
         + 6.0f * s * t * (points[2] - points[1])
         + 3.0f * t * t * (points[3] - points[2]);
}

glm::vec3 cubicBSpline(const std::array<glm::vec3, 4>& points, float t)
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

CurvedRenderer::CurvedRenderer()
    : milkCan_(createMilkCan()),
      splinePipe_(createSplinePipe()),
      ruledRoof_(createRuledRoof())
{
}

CurvedRenderer::~CurvedRenderer()
{
    destroy(ruledRoof_);
    destroy(splinePipe_);
    destroy(milkCan_);
}

CurvedRenderer::Mesh CurvedRenderer::createMilkCan()
{
    // Three joined cubic Bézier segments define radius against height.
    const std::array<std::array<glm::vec2, 4>, 3> segments = {{
        {{glm::vec2(0.34f, 0.00f), glm::vec2(0.47f, 0.05f),
          glm::vec2(0.50f, 0.30f), glm::vec2(0.50f, 0.76f)}},
        {{glm::vec2(0.50f, 0.76f), glm::vec2(0.50f, 1.02f),
          glm::vec2(0.33f, 1.12f), glm::vec2(0.29f, 1.35f)}},
        {{glm::vec2(0.29f, 1.35f), glm::vec2(0.27f, 1.52f),
          glm::vec2(0.39f, 1.58f), glm::vec2(0.36f, 1.70f)}}
    }};
    constexpr unsigned int profileSamples = 12;
    constexpr unsigned int radialSamples = 32;
    std::vector<glm::vec4> profile; // radius, height, dr, dy
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

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int row = 0; row < profile.size(); ++row)
    {
        for (unsigned int ring = 0; ring <= radialSamples; ++ring)
        {
            const float u = static_cast<float>(ring) / radialSamples;
            const float angle = u * glm::two_pi<float>();
            const float c = std::cos(angle);
            const float s = std::sin(angle);
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

CurvedRenderer::Mesh CurvedRenderer::createSplinePipe()
{
    const std::array<glm::vec3, 7> controls = {{
        glm::vec3(7.2f, 0.45f, 3.8f), glm::vec3(8.0f, 0.52f, 3.3f),
        glm::vec3(9.2f, 0.65f, 2.8f), glm::vec3(10.7f, 0.95f, 2.3f),
        glm::vec3(12.0f, 1.45f, 1.8f), glm::vec3(12.8f, 2.20f, 1.2f),
        glm::vec3(13.0f, 3.10f, 0.6f)
    }};
    constexpr unsigned int samplesPerSegment = 10;
    constexpr unsigned int ringSamples = 12;
    std::vector<glm::vec3> centers;
    for (unsigned int segment = 0; segment + 3 < controls.size(); ++segment)
    {
        const std::array<glm::vec3, 4> points = {{
            controls[segment], controls[segment + 1],
            controls[segment + 2], controls[segment + 3]}};
        for (unsigned int sample = (segment == 0 ? 0U : 1U);
             sample <= samplesPerSegment; ++sample)
            centers.push_back(cubicBSpline(points,
                static_cast<float>(sample) / samplesPerSegment));
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int row = 0; row < centers.size(); ++row)
    {
        const glm::vec3 before = centers[row == 0 ? row : row - 1];
        const glm::vec3 after = centers[row + 1 < centers.size() ? row + 1 : row];
        const glm::vec3 tangent = glm::normalize(after - before);
        const glm::vec3 reference = std::abs(tangent.y) > 0.92f
            ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::vec3 side = glm::normalize(glm::cross(tangent, reference));
        const glm::vec3 up = glm::normalize(glm::cross(side, tangent));
        for (unsigned int ring = 0; ring <= ringSamples; ++ring)
        {
            const float u = static_cast<float>(ring) / ringSamples;
            const float angle = u * glm::two_pi<float>();
            const glm::vec3 radial = side * std::cos(angle) + up * std::sin(angle);
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

CurvedRenderer::Mesh CurvedRenderer::createRuledRoof()
{
    constexpr unsigned int curveSamples = 28;
    constexpr unsigned int rulingSamples = 10;
    constexpr float halfWidth = 4.25f;
    constexpr float frontZ = 3.62f;
    constexpr float backZ = -3.62f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
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

CurvedRenderer::Mesh CurvedRenderer::upload(
    const float* vertices, unsigned int vertexFloatCount,
    const unsigned int* indices, unsigned int indexCount)
{
    Mesh mesh;
    mesh.indexCount = static_cast<int>(indexCount);
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexFloatCount * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return mesh;
}

void CurvedRenderer::destroy(Mesh& mesh)
{
    glDeleteBuffers(1, &mesh.ebo);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);
    mesh = Mesh{};
}

void CurvedRenderer::draw(
    const Mesh& mesh, const Shader& shader, const glm::mat4& model,
    unsigned int texture, const glm::vec3& tint, const glm::vec2& uvScale) const
{
    shader.setMat4("model", model);
    shader.setVec3("objectTint", tint);
    shader.setVec2("uvScale", uvScale);
    shader.setBool("useTexture", texturesEnabled_);
    shader.setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
}

void CurvedRenderer::render(const Shader& shader, const FarmTextures& textures) const
{
    applyMaterial(shader, Materials::metal());
    glm::mat4 roof(1.0f);
    roof = glm::translate(roof, glm::vec3(-10.0f, 0.0f, -8.0f));
    draw(ruledRoof_, shader, roof, textures.roof, glm::vec3(0.68f, 0.74f, 0.78f), glm::vec2(1.0f));

    glm::mat4 milkCan(1.0f);
    milkCan = glm::translate(milkCan, glm::vec3(8.3f, 0.13f, -1.0f));
    milkCan = glm::scale(milkCan, glm::vec3(1.25f));
    draw(milkCan_, shader, milkCan, textures.metal, glm::vec3(0.78f, 0.82f, 0.84f), glm::vec2(2.0f, 1.0f));

    draw(splinePipe_, shader, glm::mat4(1.0f), textures.metal,
         glm::vec3(0.18f, 0.48f, 0.64f), glm::vec2(1.0f));
}
