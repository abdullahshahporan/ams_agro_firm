#ifndef AMS_AGRO_FARM_LIGHTING_SYSTEM_H
#define AMS_AGRO_FARM_LIGHTING_SYSTEM_H

#include "shader.h"

#include <array>
#include <glm/glm.hpp>

class LightingSystem
{
public:
    LightingSystem();

    void setupShader(const Shader& shader, const glm::vec3& viewPosition) const;

    void toggleDirectional() { directionalEnabled_ = !directionalEnabled_; }
    void togglePointLights() { pointLightsEnabled_ = !pointLightsEnabled_; }
    void toggleSpotlight() { spotlightEnabled_ = !spotlightEnabled_; }
    void toggleDayNight() { nightMode_ = !nightMode_; }
    void toggleAmbient() { ambientEnabled_ = !ambientEnabled_; }
    void toggleDiffuse() { diffuseEnabled_ = !diffuseEnabled_; }
    void toggleSpecular() { specularEnabled_ = !specularEnabled_; }

    bool directionalEnabled() const { return directionalEnabled_; }
    bool pointLightsEnabled() const { return pointLightsEnabled_; }
    bool spotlightEnabled() const { return spotlightEnabled_; }
    bool nightMode() const { return nightMode_; }
    bool ambientEnabled() const { return ambientEnabled_; }
    bool diffuseEnabled() const { return diffuseEnabled_; }
    bool specularEnabled() const { return specularEnabled_; }

    glm::vec3 clearColor() const;
    float pointFixtureEmission() const;
    float spotlightFixtureEmission() const;

    static const std::array<glm::vec3, 3>& pointLightPositions();
    static glm::vec3 spotlightPosition();
    static glm::vec3 spotlightDirection();

private:
    bool directionalEnabled_{true};
    bool pointLightsEnabled_{true};
    bool spotlightEnabled_{true};
    bool nightMode_{false};
    bool ambientEnabled_{true};
    bool diffuseEnabled_{true};
    bool specularEnabled_{true};
};

#endif
