// Item: interactive valve wheel, flexible hose and visible water stream.
#include "farm_scene.h"
#include "material.h"
#include "shared/farm_interaction.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;

namespace
{
void drawSegment(const CubeRenderer& cubes,const Shader& shader,
                 const glm::vec3& start,const glm::vec3& end,float thickness,
                 const glm::vec3& color)
{
    const glm::vec3 difference=end-start;
    const float length=glm::length(difference);
    if(length<0.0001f)return;
    glm::mat4 transform=glm::translate(glm::mat4(1.0f),(start+end)*0.5f);
    transform*=glm::mat4_cast(glm::rotation(glm::vec3(0,1,0),difference/length));
    cubes.drawColored(shader,transform,glm::vec3(0),
                      glm::vec3(thickness,length,thickness),color);
}
}

void FarmScene::drawWaterHose(const Shader& shader,bool valveOpen,float valveAngle,
                              bool workerHoldingHose,bool hosePlacedAtTrough,
                              const glm::vec3& hoseEndPosition,float) const
{
    applyMaterial(shader,Materials::metal());

    // A real tap assembly grows out of the low end of the rigid B-spline
    // pipe: vertical riser, elbow, side spout and downward hose coupling.
    const glm::vec3 pipeEnd=FarmInteraction::PipeEnd;
    const glm::vec3 riserTop(8.07f,1.10f,3.30f);
    const glm::vec3 spoutEnd(7.35f,1.10f,3.30f);
    drawSegment(cubes_,shader,pipeEnd,riserTop,0.19f,glm::vec3(0.34f,0.39f,0.42f));
    drawSegment(cubes_,shader,riserTop,spoutEnd,0.19f,glm::vec3(0.34f,0.39f,0.42f));
    drawSegment(cubes_,shader,spoutEnd,FarmInteraction::HoseAnchor,0.17f,glm::vec3(0.30f,0.35f,0.38f));
    primitives_.drawSphere(shader,glm::mat4(1),riserTop,glm::vec3(0.16f),glm::vec3(0.38f,0.43f,0.46f));
    primitives_.drawSphere(shader,glm::mat4(1),FarmInteraction::HoseAnchor,
                           glm::vec3(0.15f,0.12f,0.15f),glm::vec3(0.28f,0.33f,0.36f));

    // The red hand-wheel sits above the pipe and rotates around the tap stem.
    const glm::vec3 valve=FarmInteraction::ValvePosition;
    drawSegment(cubes_,shader,riserTop,valve,0.105f,glm::vec3(0.30f,0.34f,0.36f));
    constexpr int wheelSegments=16;
    constexpr float radius=0.30f;
    const float phase=glm::radians(valveAngle);
    for(int index=0;index<wheelSegments;++index)
    {
        const float a=phase+glm::two_pi<float>()*index/wheelSegments;
        const float b=phase+glm::two_pi<float>()*(index+1)/wheelSegments;
        drawSegment(cubes_,shader,
                    valve+glm::vec3(cos(a)*radius,0.02f,sin(a)*radius),
                    valve+glm::vec3(cos(b)*radius,0.02f,sin(b)*radius),
                    0.055f,glm::vec3(0.76f,0.07f,0.04f));
    }
    for(int spoke=0;spoke<4;++spoke)
    {
        const float angle=phase+glm::half_pi<float>()*spoke;
        drawSegment(cubes_,shader,valve,
                    valve+glm::vec3(cos(angle)*radius,0.02f,sin(angle)*radius),
                    0.045f,glm::vec3(0.82f,0.10f,0.05f));
    }
    primitives_.drawSphere(shader,glm::mat4(1),valve+glm::vec3(0,0.02f,0),
                           glm::vec3(0.11f),glm::vec3(0.30f,0.31f,0.32f));

    // The flexible hose is sampled as a hanging curve.  Both ends remain
    // connected while the middle settles naturally toward the ground.
    const glm::vec3 start=FarmInteraction::HoseAnchor;
    const glm::vec3 end=hoseEndPosition;
    const float horizontal=glm::length(glm::vec2(end.x-start.x,end.z-start.z));
    const float sag=min(0.85f,0.10f+horizontal*0.035f);
    glm::vec3 previous=start;
    constexpr int hoseSegments=22;
    for(int index=1;index<=hoseSegments;++index)
    {
        const float t=static_cast<float>(index)/hoseSegments;
        glm::vec3 current=glm::mix(start,end,t);
        current.y=max(0.16f,current.y-sin(glm::pi<float>()*t)*sag);
        drawSegment(cubes_,shader,previous,current,0.105f,
                    glm::vec3(0.035f,0.11f,0.14f));
        previous=current;
    }
    primitives_.drawSphere(shader,glm::mat4(1),end,glm::vec3(0.15f,0.12f,0.15f),
                           workerHoldingHose?glm::vec3(0.18f,0.24f,0.26f):glm::vec3(0.12f,0.18f,0.20f));

    if(valveOpen)
    {
        applyMaterial(shader,Materials::metal());
        const glm::vec3 landing=hosePlacedAtTrough
            ? glm::vec3(-8.0f,0.72f,-5.45f)
            : glm::vec3(end.x,0.055f,end.z);
        drawSegment(cubes_,shader,end,landing,0.085f,glm::vec3(0.12f,0.66f,1.00f));

        // A running hose held above the lawn must visibly wet the ground.
        // The shallow translucent-looking patch and radial splash streaks make
        // the contact point readable even from the cinematic follow camera.
        if(!hosePlacedAtTrough)
        {
            cubes_.drawColored(shader,landing+glm::vec3(0,-0.025f,0),
                               glm::vec3(0.92f,0.025f,0.64f),
                               glm::vec3(0.055f,0.34f,0.62f));
            for(int splash=0;splash<8;++splash)
            {
                const float angle=glm::two_pi<float>()*splash/8.0f;
                const glm::vec3 edge=landing+glm::vec3(cos(angle)*0.46f,
                                                       0.075f+0.035f*(splash%2),
                                                       sin(angle)*0.32f);
                drawSegment(cubes_,shader,landing+glm::vec3(0,0.05f,0),edge,
                            0.025f,glm::vec3(0.18f,0.72f,1.0f));
            }
        }
    }
}
