#ifndef AMS_AGRO_FARM_INTERACTION_H
#define AMS_AGRO_FARM_INTERACTION_H
#include <glm/glm.hpp>
#include <array>
#include <cmath>
namespace FarmInteraction
{
constexpr int HayCount = 3;
inline glm::vec3 hayPosition(int index) { return { -4.1f, 0.19f, 3.4f + index * 1.05f }; }
// The worker stands beside a bundle instead of walking through its centre.
inline glm::vec3 hayApproachPosition(int index) { return { -2.95f, 0.0f, 3.4f + index * 1.05f }; }

// The rigid B-spline pipe ends here. A flexible hose continues from this
// point and can reach the cattle-shed water trough.
constexpr float HoseMaximumLength = 22.5f;
const glm::vec3 PipeEnd(8.07f, 0.54f, 3.30f);
const glm::vec3 HoseAnchor(7.35f, 0.98f, 3.30f);
const glm::vec3 HoseRest(6.85f, 0.27f, 3.72f);
const glm::vec3 ValvePosition(8.07f, 1.48f, 3.30f);
const glm::vec3 WaterTroughInteraction(-8.0f, 0.0f, -3.75f);
const glm::vec3 WaterTroughNozzle(-8.0f, 1.05f, -4.96f);
const glm::vec3 PoultryFeedPosition(5.5f, 0.0f, 7.1f);
const glm::vec3 FeedStorePosition(11.35f, 0.0f, -12.05f);
const glm::vec3 HoeStorePosition(10.05f, 0.0f, -12.05f);
// The cultivated patch sits beside (not inside) the hay area, leaving a clear
// walking lane while making the farmer's hoe work visible in the same shot.
const glm::vec3 HoeWorkPosition(-1.65f, 0.0f, 5.70f);
inline float workerFloor(float x, float z)
{
    if(x>6.25f&&x<9.75f&&z>=10.4f&&z<13.6f)return .22f;
    if(x>7.2f&&x<8.8f&&z>=10.0f&&z<10.4f)return .02f+.2f*glm::clamp((z-10.0f)/.4f,0.0f,1.0f);
    return 0.0f;
}
inline float turnToward(float current, float target, float dt)
{
    float difference = fmod(target-current,360.0f);
    if(difference>180)difference-=360;
    if(difference<-180)difference+=360;
    return fmod(current + glm::clamp(difference,-270.0f*dt,270.0f*dt),360.0f);
}
}
#endif
