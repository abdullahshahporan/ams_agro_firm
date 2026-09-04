#include "lighting_system.h"

#include <glm/geometric.hpp>

#include <string>
#include <cmath>

LightingSystem::LightingSystem() = default;

bool LightingSystem::pointLightsEnabled() const
{
    return shedLightsEnabled_ || fenceLightsEnabled_ ||
           bannerLightEnabled_ || billboardLightEnabled_;
}

void LightingSystem::togglePointLights()
{
    const bool allEnabled = shedLightsEnabled_ && fenceLightsEnabled_ &&
                            bannerLightEnabled_ && billboardLightEnabled_;
    const bool newState = !allEnabled;
    shedLightsEnabled_ = newState;
    fenceLightsEnabled_ = newState;
    bannerLightEnabled_ = newState;
    billboardLightEnabled_ = newState;
}

bool LightingSystem::pointLightEnabled(std::size_t index) const
{
    if (index <= 2)
        return shedLightsEnabled_;
    if (index == 3)
        return bannerLightEnabled_;
    return billboardLightEnabled_;
}

const std::array<glm::vec3, 5>& LightingSystem::pointLightPositions()
{
    static const std::array<glm::vec3, 5> positions = {
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
    shader.setVec3("viewPos", viewPosition);
    shader.setBool("directionalLightEnabled", directionalEnabled_);
    shader.setBool("pointLightsEnabled",
                   shedLightsEnabled_ || bannerLightEnabled_ || billboardLightEnabled_);
    shader.setBool("spotLightEnabled", spotlightEnabled_);
    shader.setBool("ambientEnabled", ambientEnabled_);
    shader.setBool("diffuseEnabled", diffuseEnabled_);
    shader.setBool("specularEnabled", specularEnabled_);
    shader.setVec3("sceneAmbient", nightMode_
        ? glm::vec3(0.030f, 0.040f, 0.075f)
        : glm::vec3(0.080f, 0.085f, 0.075f));

    shader.setVec3("dirLight.direction", glm::vec3(-0.38f, -1.0f, -0.28f));
    if (nightMode_)
    {
        shader.setVec3("dirLight.ambient", glm::vec3(0.008f, 0.012f, 0.025f));
        shader.setVec3("dirLight.diffuse", glm::vec3(0.035f, 0.050f, 0.11f));
        shader.setVec3("dirLight.specular", glm::vec3(0.06f, 0.09f, 0.19f));
    }
    else
    {
        shader.setVec3("dirLight.ambient", glm::vec3(0.25f, 0.25f, 0.23f));
        shader.setVec3("dirLight.diffuse", glm::vec3(0.78f, 0.75f, 0.67f));
        shader.setVec3("dirLight.specular", glm::vec3(0.55f, 0.55f, 0.50f));
    }

    const auto& positions = pointLightPositions();
    for (std::size_t index = 0; index < positions.size(); ++index)
    {
        const std::string prefix = "pointLights[" + std::to_string(index) + "].";
        const bool enabled = pointLightEnabled(index);
        shader.setVec3(prefix + "position", positions[index]);
        shader.setFloat(prefix + "constant", 1.0f);
        shader.setFloat(prefix + "linear", nightMode_ ? 0.055f : 0.09f);
        shader.setFloat(prefix + "quadratic", nightMode_ ? 0.012f : 0.032f);
        if (!enabled)
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.0f));
            shader.setVec3(prefix + "diffuse", glm::vec3(0.0f));
            shader.setVec3(prefix + "specular", glm::vec3(0.0f));
        }
        else if (nightMode_)
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.045f, 0.032f, 0.018f));
            shader.setVec3(prefix + "diffuse", glm::vec3(1.38f, 0.94f, 0.48f));
            shader.setVec3(prefix + "specular", glm::vec3(1.20f, 0.95f, 0.60f));
        }
        else
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.012f, 0.010f, 0.008f));
            shader.setVec3(prefix + "diffuse", glm::vec3(0.13f, 0.10f, 0.07f));
            shader.setVec3(prefix + "specular", glm::vec3(0.16f, 0.13f, 0.10f));
        }
    }

    shader.setVec3("spotLight.position", spotlightPosition());
    shader.setVec3("spotLight.direction", spotlightDirection());
    shader.setFloat("spotLight.cutOff", std::cos(glm::radians(19.0f)));
    shader.setFloat("spotLight.outerCutOff", std::cos(glm::radians(29.0f)));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", nightMode_ ? 0.045f : 0.07f);
    shader.setFloat("spotLight.quadratic", nightMode_ ? 0.009f : 0.017f);
    if (nightMode_)
    {
        shader.setVec3("spotLight.ambient", glm::vec3(0.025f, 0.025f, 0.018f));
        shader.setVec3("spotLight.diffuse", glm::vec3(1.45f, 1.24f, 0.72f));
        shader.setVec3("spotLight.specular", glm::vec3(1.20f, 1.08f, 0.80f));
    }
    else
    {
        shader.setVec3("spotLight.ambient", glm::vec3(0.005f));
        shader.setVec3("spotLight.diffuse", glm::vec3(0.08f, 0.075f, 0.055f));
        shader.setVec3("spotLight.specular", glm::vec3(0.10f));
    }
}

glm::vec3 LightingSystem::clearColor() const
{
    return nightMode_ ? glm::vec3(0.012f, 0.022f, 0.070f)
                      : glm::vec3(0.52f, 0.78f, 0.94f);
}

float LightingSystem::fixtureEmission(bool enabled) const
{
    if (!enabled)
        return 0.0f;
    return nightMode_ ? 1.65f : 0.24f;
}

std::array<float, 5> LightingSystem::pointFixtureEmissions() const
{
    std::array<float, 5> emissions{};
    for (std::size_t index = 0; index < emissions.size(); ++index)
        emissions[index] = fixtureEmission(pointLightEnabled(index));
    return emissions;
}

float LightingSystem::fenceFixtureEmission() const
{
    return fixtureEmission(fenceLightsEnabled_);
}

float LightingSystem::spotlightFixtureEmission() const
{
    if (!spotlightEnabled_)
        return 0.0f;
    return nightMode_ ? 1.85f : 0.18f;
}
