#include "ray_trace_renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>

RayTraceRenderer::RayTraceRenderer()
{
    // A core-profile context requires a VAO even though the full-screen
    // triangle positions are generated from gl_VertexID in the vertex shader.
    glGenVertexArrays(1, &vao_);
}

RayTraceRenderer::~RayTraceRenderer()
{
    glDeleteVertexArrays(1, &vao_);
}

void RayTraceRenderer::render(const Shader& shader, const glm::mat4& view,
                              const glm::mat4& projection) const
{
    shader.setMat4("inverseViewProjection", glm::inverse(projection * view));
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
