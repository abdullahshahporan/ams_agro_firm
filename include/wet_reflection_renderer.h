#ifndef AMS_WET_REFLECTION_RENDERER_H
#define AMS_WET_REFLECTION_RENDERER_H
#include "shader.h"
#include <glm/glm.hpp>

// Hybrid screen-space ray marching plus an off-screen planar fallback.
// Both inputs are rendered from the real textured farm and existing lights.
class WetReflectionRenderer
{
public:
    WetReflectionRenderer();
    ~WetReflectionRenderer();
    WetReflectionRenderer(const WetReflectionRenderer&)=delete;
    WetReflectionRenderer& operator=(const WetReflectionRenderer&)=delete;
    void resize(int width,int height);
    unsigned int sceneFramebuffer() const { return framebuffers_[0]; }
    unsigned int reflectionFramebuffer() const { return framebuffers_[1]; }
    void compose(const glm::mat4& view,const glm::mat4& projection,
                 const glm::mat4& reflectionView,const glm::vec3& camera,
                 float time,float wetness) const;
private:
    Shader shader_;
    unsigned int framebuffers_[2]{},colors_[2]{},depths_[2]{},vao_{0};
    int width_{0},height_{0};
};
#endif
