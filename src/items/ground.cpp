// Item: ground.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include <glm/gtc/constants.hpp>

using namespace std;
using namespace FarmStyle;

float TerrainRenderer::height(float x, float z)
{
    const float radius = sqrt(x * x + z * z);
    // All existing fields, roads and the entire collision/movement square stay
    // at their old ground height. Only the distant countryside is sculpted.
    if (radius <= 180.0f) return -0.105f;
    const float angle = atan2(z, x);
    const float blend = glm::smoothstep(180.0f, 230.0f, radius);
    const float nearRidge = exp(-pow((radius - 285.0f) / 78.0f, 2.0f));
    const float farRidge = exp(-pow((radius - 490.0f) / 145.0f, 2.0f));
    const float rolling = nearRidge * (18.0f + 7.0f * sin(3.0f * angle + 0.4f)
                                      + 4.0f * cos(5.0f * angle - 0.7f))
                        + farRidge * (30.0f + 10.0f * sin(4.0f * angle + 1.2f));
    const float beyondFlat = radius - 180.0f;
    // Gentle horizon curvature, not a visibly spherical farm or a bowl wall.
    const float curvature = beyondFlat * beyondFlat / 50000.0f;
    return -0.105f + blend * (rolling - curvature);
}

TerrainRenderer::TerrainRenderer()
{
    constexpr unsigned int rings = 160;
    constexpr unsigned int sectors = 192;
    vector<float> vertices;
    vector<unsigned int> indices;
    vertices.reserve((rings + 1) * (sectors + 1) * 8);
    indices.reserve(rings * sectors * 6);
    for (unsigned int row = 0; row <= rings; ++row)
    {
        const float u = static_cast<float>(row) / rings;
        // More samples near the farm; distant rings can be wider.
        const float radius = Radius * u * u;
        for (unsigned int sector = 0; sector <= sectors; ++sector)
        {
            const float angle = glm::two_pi<float>() * sector / sectors;
            const float x = radius * cos(angle), z = radius * sin(angle);
            constexpr float epsilon = 0.25f;
            const float dx = (height(x + epsilon, z) - height(x - epsilon, z)) / (2.0f * epsilon);
            const float dz = (height(x, z + epsilon) - height(x, z - epsilon)) / (2.0f * epsilon);
            const glm::vec3 normal = glm::normalize(glm::vec3(-dx, 1.0f, -dz));
            vertices.insert(vertices.end(), {x, height(x, z), z,
                normal.x, normal.y, normal.z, x / 2.9f, z / 2.9f});
        }
    }
    for (unsigned int row = 0; row < rings; ++row)
        for (unsigned int sector = 0; sector < sectors; ++sector)
        {
            const unsigned int a = row * (sectors + 1) + sector;
            const unsigned int b = a + sectors + 1;
            // Upward winding; the first ring's collapsed triangles are omitted.
            if (row != 0) indices.insert(indices.end(), {a, a + 1, b});
            indices.insert(indices.end(), {a + 1, b + 1, b});
        }
    indexCount_ = static_cast<int>(indices.size());
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    for (unsigned int attribute = 0; attribute < 3; ++attribute)
    {
        const unsigned int offset = attribute == 0 ? 0 : (attribute == 1 ? 3 : 6);
        glVertexAttribPointer(attribute, attribute == 2 ? 2 : 3, GL_FLOAT, GL_FALSE,
                              8 * sizeof(float), reinterpret_cast<void*>(offset * sizeof(float)));
        glEnableVertexAttribArray(attribute);
    }
    glBindVertexArray(0);
}

TerrainRenderer::~TerrainRenderer()
{
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void TerrainRenderer::draw(const Shader& shader, unsigned int grassTexture, bool textured) const
{
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setVec3("objectTint", glm::vec3(0.60f, 0.76f, 0.53f));
    shader.setVec2("uvScale", glm::vec2(1.0f));
    shader.setBool("useTexture", textured);
    shader.setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void FarmScene::drawGround(const Shader& shader) const
{
    // One continuous round landscape instead of an exposed square slab edge.
    terrain_.draw(shader, textures_.grass, cubes_.texturesEnabled());

    cubes_.drawTextured(
        shader,
        glm::vec3(0.0f, -0.15f, -1.25f),
        glm::vec3(36.0f, 0.25f, 33.5f),
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
