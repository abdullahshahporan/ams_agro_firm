#ifndef AMS_AGRO_FARM_RAY_TRACE_RENDERER_H
#define AMS_AGRO_FARM_RAY_TRACE_RENDERER_H

#include "shader.h"

#include <glm/glm.hpp>

class RayTraceRenderer
{
public:
    RayTraceRenderer();
    ~RayTraceRenderer();

    RayTraceRenderer(const RayTraceRenderer&) = delete;
    RayTraceRenderer& operator=(const RayTraceRenderer&) = delete;

    void render(const Shader& shader, const glm::mat4& view,
                const glm::mat4& projection) const;

private:
    unsigned int vao_{0};
};

#endif
