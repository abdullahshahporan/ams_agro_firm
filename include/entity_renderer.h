#ifndef AMS_AGRO_FARM_ENTITY_RENDERER_H
#define AMS_AGRO_FARM_ENTITY_RENDERER_H

#include "animation_system.h"
#include "cube_renderer.h"
#include "primitive_renderer.h"

class EntityRenderer
{
public:
    EntityRenderer(const CubeRenderer& cubes, const PrimitiveRenderer& primitives)
        : cubes_(cubes), primitives_(primitives) {}

    void render(const Shader& shader, const AnimationSystem& animations) const;

private:
    void drawCow(const Shader& shader, const Cow& cow) const;
    void drawTether(const Shader& shader, const Cow& cow) const;
    void drawCalf(const Shader& shader, const Calf& calf) const;
    void drawCalfTether(const Shader& shader, const Calf& calf) const;
    void drawBird(const Shader& shader, const Bird& bird) const;
    void drawBovine(const Shader& shader, const glm::mat4& root,
                    const glm::vec3& bodyColor, const glm::vec3& patchColor,
                    float gaitAngle, float headDrop, float headTurn,
                    float tailAngle, float bodyBob, float hornScale,
                    bool juvenile, bool showUdder, bool ox) const;
    void drawBovineLeg(const Shader& shader, const glm::mat4& root,
                       const glm::vec3& hip, float swing,
                       const glm::vec3& legColor, bool juvenile) const;
    void drawWorker(const Shader& shader, const Worker& worker) const;
    void drawHumanLimb(const Shader& shader, const glm::mat4& joint,
                       float upperLength, float lowerLength, float bend,
                       const glm::vec3& upperColor, const glm::vec3& lowerColor,
                       float thickness) const;

    const CubeRenderer& cubes_;
    const PrimitiveRenderer& primitives_;
};

#endif
