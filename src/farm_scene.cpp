// Coordinator only: draws the individual items in the original order.
#include "farm_scene.h"
#include "lighting_system.h"
#include "material.h"

void FarmScene::render(const Shader& shader, float gateAngleDegrees,
                       float stallGateAngleDegrees, float calfShedDoorAngleDegrees,
                       float fanAngleDegrees,
                       const LightingSystem& lighting,
                       bool workerDoorOpen, bool pickupHayAvailable,
                       int deliveredHayLoads) const
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
    drawIndoorCowStalls(shader, stallGateAngleDegrees, deliveredHayLoads);
    applyMaterial(shader, Materials::hay());
    drawHayArea(shader, pickupHayAvailable);
    applyMaterial(shader, Materials::metal());
    drawWaterFacility(shader);
    applyMaterial(shader, Materials::wood());
    drawTrees(shader);
    drawPoultryShed(shader);
    drawWorkerHouse(shader, workerDoorOpen);
    applyMaterial(shader, Materials::metal());
    drawShedFans(shader, fanAngleDegrees);
    drawLightFixtures(shader, lighting);
    applyMaterial(shader, Materials::wood());
    drawFarmProps(shader);
}
