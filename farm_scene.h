#ifndef AMS_AGRO_FARM_SCENE_H
#define AMS_AGRO_FARM_SCENE_H

#include "cube_renderer.h"
#include "texture_manager.h"

#include <array>
#include <glm/glm.hpp>

class FarmScene
{
public:
    FarmScene(const CubeRenderer& cubes, const FarmTextures& textures)
        : cubes_(cubes), textures_(textures) {}

    void render(const Shader& shader, float gateAngleDegrees, float fanAngleDegrees,
                float pointFixtureEmission, float spotlightFixtureEmission) const;
    static const std::array<glm::vec3, 4>& futureCowPositions();

private:
    void drawGround(const Shader& shader) const;
    void drawRoad(const Shader& shader) const;
    void drawFarmBoundary(const Shader& shader) const;
    void drawFenceRun(const Shader& shader, const glm::vec3& start, const glm::vec3& end) const;

    void drawEntrance(const Shader& shader, float gateAngleDegrees) const;
    void drawPillar(const Shader& shader, float xPosition) const;
    void drawGateLeaf(const Shader& shader, const glm::mat4& hingeTransform, float direction) const;
    void drawFarmSign(const Shader& shader) const;
    void drawBlockText(const Shader& shader, const char* text, const glm::vec3& center) const;

    void drawCowShed(const Shader& shader) const;
    void drawBarn(const Shader& shader) const;
    void drawFeedingArea(const Shader& shader) const;
    void drawTrough(const Shader& shader, const glm::vec3& position, float length) const;
    void drawIndoorCowStalls(const Shader& shader) const;

    void drawHayArea(const Shader& shader) const;
    void drawHayBale(const Shader& shader, const glm::vec3& position, float rotation) const;
    void drawTrees(const Shader& shader) const;
    void drawTree(const Shader& shader, const glm::vec3& position, float scale) const;
    void drawWaterFacility(const Shader& shader) const;

    void drawShedFans(const Shader& shader, float fanAngleDegrees) const;
    void drawFan(const Shader& shader, const glm::vec3& position, float parentAngle) const;
    void drawLightFixtures(const Shader& shader, float pointEmission,
                           float spotlightEmission) const;

    void drawFarmProps(const Shader& shader) const;
    void drawCrate(const Shader& shader, const glm::vec3& position, float rotation) const;
    void drawFeedSack(const Shader& shader, const glm::vec3& position, float rotation) const;

    const CubeRenderer& cubes_;
    const FarmTextures& textures_;
};

#endif
