// Coordinator only: draws the individual items in the original order.
#include "farm_scene.h"
#include "lighting_system.h"
#include "material.h"

void FarmScene::render(const Shader& shader, float gateAngleDegrees,
                       float stallGateAngleDegrees, float calfShedDoorAngleDegrees,
                       float poultryDoorAngleDegrees, float fanAngleDegrees,
                       const LightingSystem& lighting,
                       float workerDoorAngle, unsigned int hayAvailableMask,
                       int deliveredHayLoads, bool waterValveOpen,
                       float waterValveAngle, bool workerHoldingHose,
                       bool hosePlacedAtTrough, const glm::vec3& hoseEndPosition,
                       float waterTroughFill, bool feedBagAvailable,
                       bool hoeAvailable, float secondWaterFill, float hayWorkProgress) const
{
    applyMaterial(shader, Materials::grass());
    drawGround(shader);
    drawSurroundings(shader);
    applyMaterial(shader, Materials::soil());
    drawRoad(shader);
    applyMaterial(shader, Materials::wood());
    drawFarmBoundary(shader, lighting.fenceFixtureEmission());
    applyMaterial(shader, Materials::brick());
    drawEntrance(shader, gateAngleDegrees);
    drawBillboard(shader);
    applyMaterial(shader, Materials::brick());
    drawCowShed(shader, calfShedDoorAngleDegrees);
    drawBarn(shader);
    drawIndoorCowStalls(shader, stallGateAngleDegrees, deliveredHayLoads,
                       waterTroughFill,secondWaterFill);
    applyMaterial(shader, Materials::hay());
    drawHayArea(shader, hayAvailableMask,hayWorkProgress);
    applyMaterial(shader, Materials::metal());
    drawWaterFacility(shader);
    drawWaterHose(shader,waterValveOpen,waterValveAngle,workerHoldingHose,
                  hosePlacedAtTrough,hoseEndPosition,waterTroughFill);
    applyMaterial(shader, Materials::wood());
    drawTrees(shader);
    drawPoultryShed(shader, poultryDoorAngleDegrees);
    drawWorkerHouse(shader, workerDoorAngle);
    applyMaterial(shader, Materials::metal());
    drawShedFans(shader, fanAngleDegrees);
    drawLightFixtures(shader, lighting);
    applyMaterial(shader, Materials::wood());
    drawFarmProps(shader);
    drawFarmTools(shader,feedBagAvailable,hoeAvailable);
}
