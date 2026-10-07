// Shared curved-mesh upload/draw code; each curved item lives in src/curves/.
#include "curved_renderer.h"
#include "material.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

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
    drawCurvedRoof(shader, textures);
    drawMilkCan(shader, textures);
    drawIrrigationPipe(shader, textures);
}
