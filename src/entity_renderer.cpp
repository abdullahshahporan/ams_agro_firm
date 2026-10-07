// Coordinator only: chooses which animal/farmer models to draw.
#include "entity_renderer.h"
#include "material.h"

void EntityRenderer::render(const Shader& shader, const AnimationSystem& animations) const
{
    for (const Cow& cow : animations.cows())
    {
        applyMaterial(shader, Materials::cow());
        drawCow(shader, cow);
        if (cow.tied)
            drawTether(shader, cow);
    }
    for (const Calf& calf : animations.calves())
    {
        applyMaterial(shader, Materials::cow());
        drawCalf(shader, calf);
        if (animations.nightMode() &&
            calf.routeState == Calf::RouteState::Sheltered)
            drawCalfTether(shader, calf);
    }
    for (const Worker& worker : animations.workers())
    {
        if (worker.state == WorkerState::HomeIdle)
            continue;
        applyMaterial(shader, Materials::cloth());
        drawWorker(shader, worker);
    }
    for (const Bird& bird : animations.birds())
    {
        applyMaterial(shader, Materials::cow());
        drawBird(shader, bird);
    }
}
