#include "wet_reflection_renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <stdexcept>
using namespace std;

WetReflectionRenderer::WetReflectionRenderer()
    :shader_("shaders/raytrace.vs","shaders/wet_reflection.fs")
{
    glGenFramebuffers(2,framebuffers_);glGenTextures(2,colors_);
    glGenTextures(2,depths_);glGenVertexArrays(1,&vao_);
}
WetReflectionRenderer::~WetReflectionRenderer()
{
    glDeleteFramebuffers(2,framebuffers_);glDeleteTextures(2,colors_);
    glDeleteTextures(2,depths_);glDeleteVertexArrays(1,&vao_);
}
void WetReflectionRenderer::resize(int width,int height)
{
    if(width==width_&&height==height_)return;
    width_=width;height_=height;
    for(int index=0;index<2;++index)
    {
        glBindTexture(GL_TEXTURE_2D,colors_[index]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,width,height,0,GL_RGBA,GL_FLOAT,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D,depths_[index]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER,framebuffers_[index]);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colors_[index],0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depths_[index],0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
            throw runtime_error("Wet-reflection framebuffer is incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void WetReflectionRenderer::compose(const glm::mat4& view,const glm::mat4& projection,
    const glm::mat4& reflectionView,const glm::vec3& camera,float time,float wetness) const
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);glViewport(0,0,width_,height_);
    glDisable(GL_DEPTH_TEST);shader_.use();
    shader_.setInt("sceneColor",0);shader_.setInt("sceneDepth",1);shader_.setInt("reflectedColor",2);
    shader_.setMat4("inverseViewProjection",glm::inverse(projection*view));
    shader_.setMat4("viewProjection",projection*view);
    shader_.setMat4("view",view);
    shader_.setMat4("reflectionViewProjection",projection*reflectionView);
    shader_.setVec3("cameraPosition",camera);shader_.setFloat("time",time);
    shader_.setFloat("wetness",wetness);
    glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,colors_[0]);
    glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,depths_[0]);
    glActiveTexture(GL_TEXTURE2);glBindTexture(GL_TEXTURE_2D,colors_[1]);
    glBindVertexArray(vao_);glDrawArrays(GL_TRIANGLES,0,3);glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);glEnable(GL_DEPTH_TEST);
}
