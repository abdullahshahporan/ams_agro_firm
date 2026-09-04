#ifndef AMS_AGRO_FARM_CUBE_RENDERER_H
#define AMS_AGRO_FARM_CUBE_RENDERER_H

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CubeRenderer
{
public:
    CubeRenderer();
    ~CubeRenderer();

    CubeRenderer(const CubeRenderer&) = delete;
    CubeRenderer& operator=(const CubeRenderer&) = delete;

    void setTexturesEnabled(bool enabled) { texturesEnabled_ = enabled; }

    void drawColored(
        const Shader& shader,
        const glm::mat4& parent,
        const glm::vec3& localPosition,
        const glm::vec3& scale,
        const glm::vec3& color,
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

    void drawColored(
        const Shader& shader,
        const glm::vec3& position,
        const glm::vec3& scale,
        const glm::vec3& color,
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

    void drawTextured(
        const Shader& shader,
        const glm::mat4& parent,
        const glm::vec3& localPosition,
        const glm::vec3& scale,
        unsigned int texture,
        const glm::vec3& tint = glm::vec3(1.0f),
        const glm::vec2& uvScale = glm::vec2(1.0f),
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

    void drawTextured(
        const Shader& shader,
        const glm::vec3& position,
        const glm::vec3& scale,
        unsigned int texture,
        const glm::vec3& tint = glm::vec3(1.0f),
        const glm::vec2& uvScale = glm::vec2(1.0f),
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

private:
    unsigned int vao_{0};
    unsigned int vbo_{0};
    unsigned int ebo_{0};
    bool texturesEnabled_{true};
};

#endif
