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
    void togglePointLights();
    void toggleShedLights() { shedLightsEnabled_ = !shedLightsEnabled_; }
    void toggleFenceLights() { fenceLightsEnabled_ = !fenceLightsEnabled_; }
    void toggleBannerLight() { bannerLightEnabled_ = !bannerLightEnabled_; }
    void toggleBillboardLight() { billboardLightEnabled_ = !billboardLightEnabled_; }
    void toggleSpotlight() { spotlightEnabled_ = !spotlightEnabled_; }
    void toggleDayNight() { nightMode_ = !nightMode_; }
    void toggleAmbient() { ambientEnabled_ = !ambientEnabled_; }
    void toggleDiffuse() { diffuseEnabled_ = !diffuseEnabled_; }
    void toggleSpecular() { specularEnabled_ = !specularEnabled_; }

    bool directionalEnabled() const { return directionalEnabled_; }
    bool pointLightsEnabled() const;
    bool shedLightsEnabled() const { return shedLightsEnabled_; }
    bool fenceLightsEnabled() const { return fenceLightsEnabled_; }
    bool bannerLightEnabled() const { return bannerLightEnabled_; }
    bool billboardLightEnabled() const { return billboardLightEnabled_; }
    bool spotlightEnabled() const { return spotlightEnabled_; }
    bool nightMode() const { return nightMode_; }
    bool ambientEnabled() const { return ambientEnabled_; }
    bool diffuseEnabled() const { return diffuseEnabled_; }
    bool specularEnabled() const { return specularEnabled_; }

    glm::vec3 clearColor() const;
    std::array<float, 5> pointFixtureEmissions() const;
    float fenceFixtureEmission() const;
    float spotlightFixtureEmission() const;

    static const std::array<glm::vec3, 5>& pointLightPositions();
    static glm::vec3 spotlightPosition();
    static glm::vec3 spotlightDirection();

private:
    bool pointLightEnabled(std::size_t index) const;
    float fixtureEmission(bool enabled) const;

    bool directionalEnabled_{true};
    bool shedLightsEnabled_{true};
    bool fenceLightsEnabled_{true};
    bool bannerLightEnabled_{true};
    bool billboardLightEnabled_{true};
    bool spotlightEnabled_{true};
    bool nightMode_{false};
    bool ambientEnabled_{true};
    bool diffuseEnabled_{true};
    bool specularEnabled_{true};
};

#endif
