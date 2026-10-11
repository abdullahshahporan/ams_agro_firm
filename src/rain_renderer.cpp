#include "rain_renderer.h"
#include <vector>
#include <cmath>
#include <algorithm>

namespace {
bool covered(float x,float z) {
    const float bounds[][4]={ {-14.5f,-5.5f,-11.8f,-4}, {7.5f,14.5f,-14.4f,-7.6f},
        {8,12,5.3f,9.5f}, {6,10,10,14}, {10,16,-2.1f,3.3f},
        {-11.8f,-4.5f,.5f,5.5f}, {6.5f,10,-2.4f,.4f} };
    for(const auto& b:bounds)if(x>b[0]&&x<b[1]&&z>b[2]&&z<b[3])return true;
    return false;
}
float ground(float x,float z) {
    if(std::abs(x)<3.5f&&z>18&&z<20)return .0785f;
    if(std::abs(x)<2.4f&&z>-2&&z<20)return .075f;
    if(std::abs(x)<4&&z>16)return 0;
    return x>-18&&x<18&&z>-18&&z<15.5f ? -.015f : -.16f;
}
}
RainRenderer::RainRenderer():shader_("shaders/rain_world.vs","shaders/rain_world.fs") {
    glGenVertexArrays(1,&vao_);glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7*sizeof(float),nullptr);glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,7*sizeof(float),reinterpret_cast<void*>(3*sizeof(float)));glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}
RainRenderer::~RainRenderer(){glDeleteBuffers(1,&vbo_);glDeleteVertexArrays(1,&vao_);}
void RainRenderer::render(const glm::mat4& view,const glm::mat4& projection,
                         const glm::vec3& camera,double time,const glm::vec3& worker,bool umbrella) {
    if(!std::isfinite(time))return;
    std::vector<float> vertices;vertices.reserve(Columns*126*7);
    const auto vertex=[&](glm::vec3 p,float alpha){vertices.insert(vertices.end(),{p.x,p.y,p.z,.65f,.82f,.96f,alpha});};
    const auto quad=[&](glm::vec3 a,glm::vec3 b,glm::vec3 c,glm::vec3 d,float alpha){
        vertex(a,alpha);vertex(b,alpha);vertex(c,alpha);vertex(a,alpha);vertex(c,alpha);vertex(d,alpha);
    };
    const auto streak=[&](glm::vec3 a,glm::vec3 b,float alpha){
        glm::vec3 right=glm::cross(b-a,camera-(a+b)*.5f);
        if(glm::length(right)<.001f)right=glm::vec3(1,0,0);
        right=glm::normalize(right)*.022f;
        quad(a-right,a+right,b+right,b-right,alpha);
    };
    for(int i=0;i<Columns;++i) {
        // Permanent world positions: no screen-space spawning or camera-following pools.
        float x,z;
        if(i<24){x=-3.6f+std::fmod(i*2.71f,7.2f);z=16.5f+std::fmod(i*4.93f,20.0f);}
        else{x=-17.5f+std::fmod(i*6.73f,35.0f);z=-17.5f+std::fmod(i*9.31f,32.0f);}
        if(covered(x,z))continue;
        if(umbrella&&glm::length(glm::vec2(x-worker.x,z-worker.z))<1.2f)continue;
        const float y=ground(x,z),phase=static_cast<float>(std::fmod(time/(.8+(i%7)*.075)+i*.137,1.0));
        const glm::vec3 hit(x,y,z);
        const float dropY=y+(1-phase)*6.5f;
        streak({x,dropY,z},{x+.05f,std::max(y,dropY-.48f),z},.65f);
        // A ring follows a preceding drop's impact; falling drops continue above it.
        const float age=phase;
        if(age<.40f){
            const float radius=.045f+age*.78f,alpha=.70f*(1-age/.40f);
            for(int s=0;s<16;++s){
                const float a=s*6.2831853f/16,b=(s+1)*6.2831853f/16;
                const auto point=[&](float angle,float r){return hit+glm::vec3(std::cos(angle)*r,0,std::sin(angle)*r);};
                quad(point(a,radius),point(a,radius+.018f),point(b,radius+.018f),point(b,radius),alpha);
            }
            if(age<.18f)for(int s=0;s<4;++s){
                const float a=s*1.5707963f+i;
                const glm::vec3 tip=hit+glm::vec3(std::cos(a)*age*.75f,.23f*std::sin(age/.18f*3.1415927f),std::sin(a)*age*.75f);
                streak(hit,tip,.72f*(1-age/.18f));
            }
        }
    }
    shader_.use();shader_.setMat4("view",view);shader_.setMat4("projection",projection);
    glEnable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<int>(vertices.size()/7));glBindVertexArray(0);
    glDepthMask(GL_TRUE);glDisable(GL_BLEND);
}
