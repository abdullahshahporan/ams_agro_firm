#ifndef AMS_AGRO_FARM_PRIMITIVE_RENDERER_H
#define AMS_AGRO_FARM_PRIMITIVE_RENDERER_H

#include "shader.h"

#include <glm/glm.hpp>

class PrimitiveRenderer
{
public:
    PrimitiveRenderer();
    ~PrimitiveRenderer();

    PrimitiveRenderer(const PrimitiveRenderer&) = delete;
    PrimitiveRenderer& operator=(const PrimitiveRenderer&) = delete;

    void drawSphere(
        const Shader& shader,
        const glm::mat4& parent,
        const glm::vec3& localPosition,
        const glm::vec3& scale,
        const glm::vec3& color,
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

    void drawCone(
        const Shader& shader,
        const glm::mat4& parent,
        const glm::vec3& localPosition,
        const glm::vec3& scale,
        const glm::vec3& color,
        float rotationDegrees = 0.0f,
        const glm::vec3& rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f)) const;

private:
    struct Mesh
    {
        unsigned int vao{0};
        unsigned int vbo{0};
        unsigned int ebo{0};
        int indexCount{0};
    };

    static Mesh createSphere();
    static Mesh createCone();
    static Mesh uploadMesh(const float* vertices, unsigned int vertexFloatCount,
                           const unsigned int* indices, unsigned int indexCount);
    static void destroyMesh(Mesh& mesh);
    static void drawMesh(
        const Mesh& mesh,
        const Shader& shader,
        const glm::mat4& parent,
        const glm::vec3& localPosition,
        const glm::vec3& scale,
        const glm::vec3& color,
        float rotationDegrees,
        const glm::vec3& rotationAxis);

    Mesh sphere_;
    Mesh cone_;
};

#endif
