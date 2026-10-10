#include "lighting_system.h"
#include "terrain_renderer.h"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include <string>
#include <cmath>

using namespace std;

namespace
{
const glm::vec3 AfternoonSky(.73f,.70f,.62f);
const glm::vec3 NightSky(.020f,.035f,.090f);
const glm::vec3 NightPointAmbient(.085f,.065f,.040f);
const glm::vec3 NightPointDiffuse(2.0f,1.55f,.95f);
const glm::vec3 NightPointSpecular(1.65f,1.40f,1.05f);
const glm::vec3 NightSpotAmbient(.055f,.055f,.035f);
const glm::vec3 NightSpotDiffuse(2.10f,1.80f,1.25f);
const glm::vec3 NightSpotSpecular(1.75f,1.60f,1.25f);
constexpr float NightFixtureEmission=2.10f;
constexpr float NightSpotEmission=2.40f;
}

LightingSystem::LightingSystem() = default;

void LightingSystem::update(float deltaTime)
{
    if (spotlightSweep_)
        spotlightPhase_ = fmod(spotlightPhase_ + deltaTime * 0.75f,
                               glm::two_pi<float>());
}

void LightingSystem::aimSpotlight(float yawDegrees, float pitchDegrees)
{
    // Manual input takes over from the automatic sweep.
    if (spotlightSweep_)
        spotlightYaw_ += sin(spotlightPhase_) * 35.0f;
    spotlightSweep_ = false;
    spotlightYaw_ = glm::clamp(spotlightYaw_ + yawDegrees, -55.0f, 55.0f);
    spotlightPitch_ = glm::clamp(spotlightPitch_ + pitchDegrees, -75.0f, -10.0f);
}

glm::vec3 LightingSystem::activeSpotlightDirection() const
{
    const float yaw = glm::radians(spotlightYaw_ +
        (spotlightSweep_ ? sin(spotlightPhase_) * 35.0f : 0.0f));
    const float pitch = glm::radians(spotlightPitch_);
    return glm::normalize(glm::vec3(sin(yaw) * cos(pitch), sin(pitch),
                                    -cos(yaw) * cos(pitch)));
}

bool LightingSystem::pointLightsEnabled() const
{
    return shedLightsEnabled_ || fenceLightsEnabled_ ||
           bannerLightEnabled_ || billboardLightEnabled_;
}

void LightingSystem::togglePointLights()
{
    const bool newState = !pointLightsEnabled();
    shedLightsEnabled_ = newState;
    fenceLightsEnabled_ = newState;
    bannerLightEnabled_ = newState;
    billboardLightEnabled_ = newState;
}

bool LightingSystem::pointLightEnabled(size_t index) const
{
    if (index <= 2)
        return shedLightsEnabled_;
    if (index == 3)
        return bannerLightEnabled_;
    return billboardLightEnabled_;
}

const array<glm::vec3, 5>& LightingSystem::pointLightPositions()
{
    static const array<glm::vec3, 5> positions = {
        glm::vec3(-12.1f, 3.00f, -8.50f),
        glm::vec3( -7.9f, 3.00f, -8.50f),
        glm::vec3( 11.0f, 3.15f,-11.00f),
        glm::vec3(  0.0f, 5.55f, 16.10f), // entrance banner downlight
        glm::vec3(-10.0f, 5.35f, 11.65f)  // owner billboard downlight
    };
    return positions;
}

glm::vec3 LightingSystem::spotlightPosition()
{
    return glm::vec3(0.0f, 4.35f, 15.15f);
}

glm::vec3 LightingSystem::spotlightDirection()
{
    // Aim from the entrance fixture down the road and into the farm.  The old
    // positive-Z direction illuminated the empty area outside the front fence.
    return glm::normalize(glm::vec3(0.0f, -4.35f, -7.25f));
}

void LightingSystem::setupShader(const Shader& shader, const glm::vec3& viewPosition) const
{
    const float nightBlend=duskProgress_>=0.0f?duskProgress_:(nightMode_?1.0f:0.0f);
    shader.setVec3("viewPos", viewPosition);
    // The last terrain ring is already fully sky-colored before it can be
    // seen, so elevated cameras never reveal the edge of a finite ground slab.
    shader.setVec3("fogColor", clearColor());
    shader.setFloat("fogStart", TerrainRenderer::FogStart);
    shader.setFloat("fogEnd", TerrainRenderer::FogEnd);
    shader.setBool("directionalLightEnabled", directionalEnabled_);
    shader.setBool("pointLightsEnabled",
                   shedLightsEnabled_ || bannerLightEnabled_ || billboardLightEnabled_);
    shader.setBool("spotLightEnabled", spotlightEnabled_);
    shader.setBool("ambientEnabled", ambientEnabled_);
    shader.setBool("diffuseEnabled", diffuseEnabled_);
    shader.setBool("specularEnabled", specularEnabled_);
    shader.setVec3("sceneAmbient",glm::mix(afternoonMode_?glm::vec3(.17f,.155f,.135f):glm::vec3(.21f,.22f,.19f),
                                          glm::vec3(.075f,.085f,.13f),nightBlend));
    shader.setVec3("dirLight.direction",glm::normalize(glm::mix(afternoonMode_?glm::vec3(-.8f,-.32f,-.4f):
        glm::vec3(-.38f,-1,-.28f),glm::vec3(-.38f,-1,-.28f),nightBlend)));
    shader.setVec3("dirLight.ambient",glm::mix(afternoonMode_?glm::vec3(.28f,.24f,.19f):glm::vec3(.46f,.47f,.42f),
                                             glm::vec3(.018f,.023f,.04f),nightBlend));
    shader.setVec3("dirLight.diffuse",glm::mix(afternoonMode_?glm::vec3(.86f,.72f,.55f):glm::vec3(1.24f,1.20f,1.07f),
                                             glm::vec3(.075f,.095f,.17f),nightBlend));
    shader.setVec3("dirLight.specular",glm::mix(afternoonMode_?glm::vec3(.76f,.68f,.55f):glm::vec3(.72f,.72f,.66f),
                                              glm::vec3(.11f,.15f,.25f),nightBlend));

    const auto& positions = pointLightPositions();
    for (size_t index = 0; index < positions.size(); ++index)
    {
        const string prefix = "pointLights[" + to_string(index) + "].";
        const bool enabled = pointLightEnabled(index);
        shader.setVec3(prefix + "position", positions[index]);
        shader.setFloat(prefix + "constant", 1.0f);
        shader.setFloat(prefix + "linear",glm::mix(afternoonMode_?.055f:.09f,.040f,nightBlend));
        shader.setFloat(prefix + "quadratic",glm::mix(afternoonMode_?.012f:.032f,.008f,nightBlend));
        if (!enabled)
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.0f));
            shader.setVec3(prefix + "diffuse", glm::vec3(0.0f));
            shader.setVec3(prefix + "specular", glm::vec3(0.0f));
        }
        else if (nightMode_)
        {
            shader.setVec3(prefix + "ambient",NightPointAmbient);
            shader.setVec3(prefix + "diffuse",NightPointDiffuse);
            shader.setVec3(prefix + "specular",NightPointSpecular);
        }
        else
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.012f, 0.010f, 0.008f));
            shader.setVec3(prefix + "diffuse", glm::vec3(0.13f, 0.10f, 0.07f));
            shader.setVec3(prefix + "specular", glm::vec3(0.16f, 0.13f, 0.10f));
        }
        if(duskProgress_>=0.0f&&enabled)
        {
            const float glow=lampBlend(index<3?.35f:(index==3?.18f:.10f),index<3?.75f:(index==3?.55f:.40f));
            shader.setVec3(prefix+"ambient",glm::mix(glm::vec3(.012f,.010f,.008f),NightPointAmbient,glow));
            shader.setVec3(prefix+"diffuse",glm::mix(glm::vec3(.13f,.10f,.07f),NightPointDiffuse,glow));
            shader.setVec3(prefix+"specular",glm::mix(glm::vec3(.16f,.13f,.10f),NightPointSpecular,glow));
        }
    }

    shader.setVec3("spotLight.position", spotlightPosition());
    shader.setVec3("spotLight.direction", activeSpotlightDirection());
    shader.setFloat("spotLight.cutOff", cos(glm::radians(19.0f)));
    shader.setFloat("spotLight.outerCutOff", cos(glm::radians(29.0f)));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear",glm::mix(.07f,.030f,nightBlend));
    shader.setFloat("spotLight.quadratic",glm::mix(.017f,.006f,nightBlend));
    if (nightMode_)
    {
        shader.setVec3("spotLight.ambient",NightSpotAmbient);
        shader.setVec3("spotLight.diffuse",NightSpotDiffuse);
        shader.setVec3("spotLight.specular",NightSpotSpecular);
    }
    else
    {
        shader.setVec3("spotLight.ambient", glm::vec3(0.005f));
        shader.setVec3("spotLight.diffuse", glm::vec3(0.08f, 0.075f, 0.055f));
        shader.setVec3("spotLight.specular", glm::vec3(0.10f));
    }
    if(duskProgress_>=0.0f)
    {
        const float glow=lampBlend(.55f,.90f);
        shader.setVec3("spotLight.ambient",glm::mix(glm::vec3(.005f),NightSpotAmbient,glow));
        shader.setVec3("spotLight.diffuse",glm::mix(glm::vec3(.08f,.075f,.055f),NightSpotDiffuse,glow));
        shader.setVec3("spotLight.specular",glm::mix(glm::vec3(.10f),NightSpotSpecular,glow));
    }
}

glm::vec3 LightingSystem::clearColor() const
{
    if(duskProgress_>=0.0f)return glm::mix(AfternoonSky,NightSky,duskProgress_);
    if(nightMode_)return NightSky;
    return afternoonMode_ ? AfternoonSky
                          : glm::vec3(0.57f,0.82f,0.97f);
}

float LightingSystem::fixtureEmission(bool enabled) const
{
    if (!enabled)
        return 0.0f;
    return nightMode_ ? NightFixtureEmission : 0.24f;
}

float LightingSystem::lampBlend(float start,float end) const
{
    return duskProgress_>=0.0f?glm::smoothstep(start,end,duskProgress_):(nightMode_?1.0f:0.0f);
}

array<float, 5> LightingSystem::pointFixtureEmissions() const
{
    array<float, 5> emissions{};
    for (size_t index = 0; index < emissions.size(); ++index)
        emissions[index] = duskProgress_>=0.0f
            ?(pointLightEnabled(index)?glm::mix(.24f,NightFixtureEmission,lampBlend(index<3?.35f:(index==3?.18f:.10f),index<3?.75f:(index==3?.55f:.40f))):0.0f)
            :fixtureEmission(pointLightEnabled(index));
    return emissions;
}

float LightingSystem::fenceFixtureEmission() const
{
    if(duskProgress_>=0.0f)return fenceLightsEnabled_?glm::mix(.24f,NightFixtureEmission,lampBlend(.05f,.35f)):0.0f;
    return fixtureEmission(fenceLightsEnabled_);
}

float LightingSystem::spotlightFixtureEmission() const
{
    if (!spotlightEnabled_)
        return 0.0f;
    if(duskProgress_>=0.0f)return glm::mix(.18f,NightSpotEmission,lampBlend(.55f,.90f));
    return nightMode_ ? NightSpotEmission : 0.18f;
}
