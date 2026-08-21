#include "primitive_renderer.h"

#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <vector>

namespace
{
void appendVertex(std::vector<float>& vertices, const glm::vec3& position,
                  const glm::vec3& normal, const glm::vec2& uv)
{
    vertices.insert(vertices.end(), {
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        uv.x, uv.y});
}

}

PrimitiveRenderer::PrimitiveRenderer()
    : sphere_(createSphere()), cone_(createCone())
{
}

PrimitiveRenderer::~PrimitiveRenderer()
{
    destroyMesh(cone_);
    destroyMesh(sphere_);
}

PrimitiveRenderer::Mesh PrimitiveRenderer::createSphere()
{
    constexpr unsigned int sectors = 24;
    constexpr unsigned int stacks = 14;
    constexpr float radius = 0.5f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve((sectors + 1) * (stacks + 1) * 8);

    for (unsigned int stack = 0; stack <= stacks; ++stack)
    {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float latitude = glm::half_pi<float>() - v * glm::pi<float>();
        const float ring = std::cos(latitude);
        const float y = std::sin(latitude);

        for (unsigned int sector = 0; sector <= sectors; ++sector)
        {
            const float u = static_cast<float>(sector) / static_cast<float>(sectors);
            const float longitude = u * glm::two_pi<float>();
            const glm::vec3 normal(ring * std::cos(longitude), y,
                                   ring * std::sin(longitude));
            appendVertex(vertices, normal * radius, normal, glm::vec2(u, v));
        }
    }

    for (unsigned int stack = 0; stack < stacks; ++stack)
    {
        for (unsigned int sector = 0; sector < sectors; ++sector)
        {
            const unsigned int current = stack * (sectors + 1) + sector;
            const unsigned int next = current + sectors + 1;
            indices.insert(indices.end(), {
                current, next, current + 1,
                current + 1, next, next + 1});
        }
    }
    return uploadMesh(vertices.data(), static_cast<unsigned int>(vertices.size()),
                      indices.data(), static_cast<unsigned int>(indices.size()));
}

PrimitiveRenderer::Mesh PrimitiveRenderer::createCone()
{
    constexpr unsigned int segments = 24;
    constexpr float radius = 0.5f;
    constexpr float halfHeight = 0.5f;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Separate side vertices give the cone a correct sloped normal.
    for (unsigned int segment = 0; segment <= segments; ++segment)
    {
        const float u = static_cast<float>(segment) / static_cast<float>(segments);
        const float angle = u * glm::two_pi<float>();
        const float cosine = std::cos(angle);
        const float sine = std::sin(angle);
        const glm::vec3 sideNormal = glm::normalize(glm::vec3(cosine, radius, sine));
        appendVertex(vertices, glm::vec3(radius * cosine, -halfHeight, radius * sine),
                     sideNormal, glm::vec2(u, 0.0f));
        appendVertex(vertices, glm::vec3(0.0f, halfHeight, 0.0f),
                     sideNormal, glm::vec2(u, 1.0f));
    }
    for (unsigned int segment = 0; segment < segments; ++segment)
    {
        const unsigned int base = segment * 2;
        indices.insert(indices.end(), {base, base + 2, base + 1});
    }

    const unsigned int centerIndex = static_cast<unsigned int>(vertices.size() / 8);
    appendVertex(vertices, glm::vec3(0.0f, -halfHeight, 0.0f),
                 glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.5f));
    for (unsigned int segment = 0; segment <= segments; ++segment)
    {
        const float u = static_cast<float>(segment) / static_cast<float>(segments);
        const float angle = u * glm::two_pi<float>();
        const float cosine = std::cos(angle);
        const float sine = std::sin(angle);
        appendVertex(vertices, glm::vec3(radius * cosine, -halfHeight, radius * sine),
                     glm::vec3(0.0f, -1.0f, 0.0f),
                     glm::vec2(0.5f + 0.5f * cosine, 0.5f + 0.5f * sine));
    }
    for (unsigned int segment = 0; segment < segments; ++segment)
        indices.insert(indices.end(), {centerIndex, centerIndex + segment + 2,
                                       centerIndex + segment + 1});

    return uploadMesh(vertices.data(), static_cast<unsigned int>(vertices.size()),
                      indices.data(), static_cast<unsigned int>(indices.size()));
}

PrimitiveRenderer::Mesh PrimitiveRenderer::uploadMesh(
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
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertexFloatCount * sizeof(float)),
                 vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indexCount * sizeof(unsigned int)),
                 indices, GL_STATIC_DRAW);

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

void PrimitiveRenderer::destroyMesh(Mesh& mesh)
{
    glDeleteBuffers(1, &mesh.ebo);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);
    mesh = Mesh{};
}

void PrimitiveRenderer::drawMesh(
    const Mesh& mesh, const Shader& shader, const glm::mat4& parent,
    const glm::vec3& localPosition, const glm::vec3& scale,
    const glm::vec3& color, float rotationDegrees,
    const glm::vec3& rotationAxis)
{
    glm::mat4 model = glm::translate(parent, localPosition);
    if (rotationDegrees != 0.0f)
        model = glm::rotate(model, glm::radians(rotationDegrees), rotationAxis);
    model = glm::scale(model, scale);
    shader.setMat4("model", model);
    shader.setVec3("objectTint", color);
    shader.setVec2("uvScale", glm::vec2(1.0f));
    shader.setBool("useTexture", false);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
}

void PrimitiveRenderer::drawSphere(
    const Shader& shader, const glm::mat4& parent,
    const glm::vec3& localPosition, const glm::vec3& scale,
    const glm::vec3& color, float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    drawMesh(sphere_, shader, parent, localPosition, scale, color,
             rotationDegrees, rotationAxis);
}

void PrimitiveRenderer::drawCone(
    const Shader& shader, const glm::mat4& parent,
    const glm::vec3& localPosition, const glm::vec3& scale,
    const glm::vec3& color, float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    drawMesh(cone_, shader, parent, localPosition, scale, color,
             rotationDegrees, rotationAxis);
}
