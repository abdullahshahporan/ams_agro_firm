#ifndef AMS_RAIN_RENDERER_H
#define AMS_RAIN_RENDERER_H
#include "shader.h"
#include <glm/glm.hpp>

// Depth-tested falling drops and ground-contact ripples, batched in one draw.
class RainRenderer {
public:
    static constexpr int Columns=80;
    RainRenderer();
    ~RainRenderer();
    RainRenderer(const RainRenderer&)=delete;
    RainRenderer& operator=(const RainRenderer&)=delete;
    void render(const glm::mat4& view,const glm::mat4& projection,
                const glm::vec3& camera,double time,const glm::vec3& worker,bool umbrella);
private:
    Shader shader_;
    unsigned int vao_{0},vbo_{0};
};
#endif
