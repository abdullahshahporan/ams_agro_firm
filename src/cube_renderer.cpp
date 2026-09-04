#include "cube_renderer.h"

#include <glad/glad.h>

CubeRenderer::CubeRenderer()
{
    // One shared cube mesh: position (3), normal (3), and UV (2).
    // The normals and normal-matrix shader output are ready for Module 4.
    constexpr float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0, 0,-1,  0,0,
         0.5f,-0.5f,-0.5f,  0, 0,-1,  1,0,
         0.5f, 0.5f,-0.5f,  0, 0,-1,  1,1,
        -0.5f, 0.5f,-0.5f,  0, 0,-1,  0,1,

        -0.5f,-0.5f, 0.5f,  0, 0, 1,  0,0,
         0.5f,-0.5f, 0.5f,  0, 0, 1,  1,0,
         0.5f, 0.5f, 0.5f,  0, 0, 1,  1,1,
        -0.5f, 0.5f, 0.5f,  0, 0, 1,  0,1,

        -0.5f,-0.5f,-0.5f, -1, 0, 0,  0,0,
        -0.5f,-0.5f, 0.5f, -1, 0, 0,  1,0,
        -0.5f, 0.5f, 0.5f, -1, 0, 0,  1,1,
        -0.5f, 0.5f,-0.5f, -1, 0, 0,  0,1,

         0.5f,-0.5f,-0.5f,  1, 0, 0,  0,0,
         0.5f,-0.5f, 0.5f,  1, 0, 0,  1,0,
         0.5f, 0.5f, 0.5f,  1, 0, 0,  1,1,
         0.5f, 0.5f,-0.5f,  1, 0, 0,  0,1,

        -0.5f,-0.5f,-0.5f,  0,-1, 0,  0,0,
         0.5f,-0.5f,-0.5f,  0,-1, 0,  1,0,
         0.5f,-0.5f, 0.5f,  0,-1, 0,  1,1,
        -0.5f,-0.5f, 0.5f,  0,-1, 0,  0,1,

        -0.5f, 0.5f,-0.5f,  0, 1, 0,  0,0,
         0.5f, 0.5f,-0.5f,  0, 1, 0,  1,0,
         0.5f, 0.5f, 0.5f,  0, 1, 0,  1,1,
        -0.5f, 0.5f, 0.5f,  0, 1, 0,  0,1
    };

    constexpr unsigned int indices[] = {
         0, 1, 2,  2, 3, 0,
         4, 5, 6,  6, 7, 4,
         8, 9,10, 10,11, 8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

CubeRenderer::~CubeRenderer()
{
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void CubeRenderer::drawColored(
    const Shader& shader,
    const glm::mat4& parent,
    const glm::vec3& localPosition,
    const glm::vec3& scale,
    const glm::vec3& color,
    float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    glm::mat4 model = glm::translate(parent, localPosition);
    if (rotationDegrees != 0.0f)
        model = glm::rotate(model, glm::radians(rotationDegrees), rotationAxis);
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    shader.setVec3("objectTint", color);
    shader.setVec2("uvScale", glm::vec2(1.0f));
    shader.setBool("useTexture", false);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void CubeRenderer::drawColored(
    const Shader& shader,
    const glm::vec3& position,
    const glm::vec3& scale,
    const glm::vec3& color,
    float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    drawColored(shader, glm::mat4(1.0f), position, scale, color, rotationDegrees, rotationAxis);
}

void CubeRenderer::drawTextured(
    const Shader& shader,
    const glm::mat4& parent,
    const glm::vec3& localPosition,
    const glm::vec3& scale,
    unsigned int texture,
    const glm::vec3& tint,
    const glm::vec2& uvScale,
    float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    glm::mat4 model = glm::translate(parent, localPosition);
    if (rotationDegrees != 0.0f)
        model = glm::rotate(model, glm::radians(rotationDegrees), rotationAxis);
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    shader.setVec3("objectTint", tint);
    shader.setVec2("uvScale", uvScale);
    shader.setBool("useTexture", texturesEnabled_);
    shader.setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void CubeRenderer::drawTextured(
    const Shader& shader,
    const glm::vec3& position,
    const glm::vec3& scale,
    unsigned int texture,
    const glm::vec3& tint,
    const glm::vec2& uvScale,
    float rotationDegrees,
    const glm::vec3& rotationAxis) const
{
    drawTextured(shader, glm::mat4(1.0f), position, scale, texture, tint, uvScale, rotationDegrees, rotationAxis);
}
