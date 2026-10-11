#ifndef AMS_AGRO_FARM_RAIN_GEOMETRY_H
#define AMS_AGRO_FARM_RAIN_GEOMETRY_H

#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

// One deterministic rain layer for the live scene and documentation captures.
namespace RainGeometry
{
constexpr int StreakCount = 110;
constexpr float BaseLength = 18.0f;
constexpr float BaseWidth = 1.6f;
constexpr float Opacity = .62f;

inline void append(vector<float>& vertices, double time, int width, int height)
{
    if(width<=0||height<=0||!isfinite(time))return;
    const float scale=min(width/1200.0f,height/800.0f);
    for(int index=0;index<StreakCount;++index)
    {
        const float x=fmod(index*97.0f+static_cast<float>(time)*61.0f,width+80.0f)-40.0f;
        const float y=fmod(index*53.0f+static_cast<float>(time)*335.0f,height+90.0f)-45.0f;
        const float length=(BaseLength+index%17)*scale;
        const float thickness=(BaseWidth+(index%3)*.50f)*scale;
        const float lean=5.0f*scale;
        for(const glm::vec2& p:{glm::vec2(x,y),{x+thickness,y},{x-lean+thickness,y+length},
                                {x,y},{x-lean+thickness,y+length},{x-lean,y+length}})
            vertices.insert(vertices.end(),{p.x,p.y,.72f,.86f,1.0f,Opacity});
    }
}
}
#endif
