#include "lighting_system.h"

#include <glm/geometric.hpp>

#include <string>
#include <cmath>

LightingSystem::LightingSystem() = default;

const std::array<glm::vec3, 3>& LightingSystem::pointLightPositions()
{
    static const std::array<glm::vec3, 3> positions = {
        glm::vec3(-12.1f, 3.00f, -8.50f),
        glm::vec3( -7.9f, 3.00f, -8.50f),
        glm::vec3( 11.0f, 3.15f,-11.00f)
    };
    return positions;
}

glm::vec3 LightingSystem::spotlightPosition()
{
    return glm::vec3(0.0f, 4.35f, 15.15f);
}

glm::vec3 LightingSystem::spotlightDirection()
{
    return glm::normalize(glm::vec3(0.0f, -4.35f, 5.75f));
}

void LightingSystem::setupShader(const Shader& shader, const glm::vec3& viewPosition) const
{
    shader.setVec3("viewPos", viewPosition);
    shader.setBool("directionalLightEnabled", directionalEnabled_);
    shader.setBool("pointLightsEnabled", pointLightsEnabled_);
    shader.setBool("spotLightEnabled", spotlightEnabled_);
    shader.setBool("ambientEnabled", ambientEnabled_);
    shader.setBool("diffuseEnabled", diffuseEnabled_);
    shader.setBool("specularEnabled", specularEnabled_);

    shader.setVec3("dirLight.direction", glm::vec3(-0.38f, -1.0f, -0.28f));
    if (nightMode_)
    {
        shader.setVec3("dirLight.ambient", glm::vec3(0.035f, 0.045f, 0.085f));
        shader.setVec3("dirLight.diffuse", glm::vec3(0.075f, 0.095f, 0.18f));
        shader.setVec3("dirLight.specular", glm::vec3(0.10f, 0.13f, 0.24f));
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
        shader.setVec3(prefix + "position", positions[index]);
        shader.setFloat(prefix + "constant", 1.0f);
        shader.setFloat(prefix + "linear", 0.09f);
        shader.setFloat(prefix + "quadratic", 0.032f);
        if (nightMode_)
        {
            shader.setVec3(prefix + "ambient", glm::vec3(0.055f, 0.040f, 0.025f));
            shader.setVec3(prefix + "diffuse", glm::vec3(1.10f, 0.78f, 0.42f));
            shader.setVec3(prefix + "specular", glm::vec3(1.00f, 0.82f, 0.56f));
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
    shader.setFloat("spotLight.cutOff", std::cos(glm::radians(16.0f)));
    shader.setFloat("spotLight.outerCutOff", std::cos(glm::radians(24.0f)));
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.07f);
    shader.setFloat("spotLight.quadratic", 0.017f);
    if (nightMode_)
    {
        shader.setVec3("spotLight.ambient", glm::vec3(0.025f, 0.025f, 0.018f));
        shader.setVec3("spotLight.diffuse", glm::vec3(1.20f, 1.08f, 0.72f));
        shader.setVec3("spotLight.specular", glm::vec3(1.0f, 0.95f, 0.78f));
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

float LightingSystem::pointFixtureEmission() const
{
    if (!pointLightsEnabled_)
        return 0.02f;
    return nightMode_ ? 1.65f : 0.24f;
}

float LightingSystem::spotlightFixtureEmission() const
{
    if (!spotlightEnabled_)
        return 0.02f;
    return nightMode_ ? 1.85f : 0.18f;
}
