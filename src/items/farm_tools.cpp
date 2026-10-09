// Item: practical tools stored inside the previously empty barn.
#include "farm_scene.h"
#include "material.h"
#include "shared/farm_style.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace FarmStyle;

void FarmScene::drawFarmTools(const Shader& shader, bool feedBagAvailable,
                              bool hoeAvailable) const
{
    applyMaterial(shader,Materials::wood());

    // Back-wall rack, visible through the wide barn opening.
    cubes_.drawTextured(shader,glm::vec3(10.55f,1.52f,-13.48f),
                        glm::vec3(5.0f,0.13f,0.16f),textures_.wood,WarmWood);
    for(float x:{8.45f,9.55f,10.65f,11.75f})
        cubes_.drawTextured(shader,glm::vec3(x,1.46f,-13.34f),
                            glm::vec3(0.10f,0.34f,0.18f),textures_.wood,WarmWood);

    // Shovel: long wooden shaft, metal grip and a broad digging blade.
    glm::mat4 shovel=glm::translate(glm::mat4(1),glm::vec3(8.55f,0.18f,-13.10f));
    shovel=glm::rotate(shovel,glm::radians(-8.0f),glm::vec3(0,0,1));
    cubes_.drawTextured(shader,shovel,glm::vec3(0,1.05f,0),glm::vec3(0.10f,2.10f,0.10f),textures_.wood,WarmWood);
    cubes_.drawColored(shader,shovel,glm::vec3(0,0.18f,0),glm::vec3(0.42f,0.38f,0.12f),glm::vec3(0.34f,0.37f,0.38f));
    cubes_.drawColored(shader,shovel,glm::vec3(0,2.16f,0),glm::vec3(0.42f,0.10f,0.12f),glm::vec3(0.28f,0.30f,0.31f));

    // Kodal/hoe disappears from the rack while the farmer is using it.
    if(hoeAvailable)
    {
        glm::mat4 hoe=glm::translate(glm::mat4(1),glm::vec3(9.70f,0.20f,-13.08f));
        hoe=glm::rotate(hoe,glm::radians(7.0f),glm::vec3(0,0,1));
        cubes_.drawTextured(shader,hoe,glm::vec3(0,0.92f,0),glm::vec3(0.09f,1.85f,0.09f),textures_.wood,WarmWood);
        cubes_.drawColored(shader,hoe,glm::vec3(0,0.10f,0),glm::vec3(0.62f,0.16f,0.28f),glm::vec3(0.30f,0.32f,0.33f),-12.0f,glm::vec3(0,0,1));
    }

    // One working feed sack is stored inside; it is removed when carried.
    if(feedBagAvailable)
        drawFeedSack(shader,glm::vec3(11.35f,.55f,-12.25f),0.0f);

    // Rake and pitchfork give the tool rack a recognisable farm silhouette.
    for(float x:{10.75f,11.80f})
        cubes_.drawTextured(shader,glm::vec3(x,1.18f,-13.10f),glm::vec3(0.09f,2.18f,0.09f),textures_.wood,WarmWood);
    cubes_.drawColored(shader,glm::vec3(10.75f,0.20f,-13.10f),glm::vec3(0.78f,0.12f,0.18f),glm::vec3(0.27f,0.29f,0.30f));
    for(int tine=-3;tine<=3;++tine)
        cubes_.drawColored(shader,glm::vec3(10.75f+tine*0.11f,0.08f,-13.10f),glm::vec3(0.035f,0.28f,0.07f),glm::vec3(0.25f,0.27f,0.28f));
    cubes_.drawColored(shader,glm::vec3(11.80f,0.25f,-13.10f),glm::vec3(0.52f,0.08f,0.10f),glm::vec3(0.25f,0.27f,0.28f));
    for(float x:{11.58f,11.80f,12.02f})
        cubes_.drawColored(shader,glm::vec3(x,0.10f,-13.10f),glm::vec3(0.035f,0.34f,0.06f),glm::vec3(0.25f,0.27f,0.28f));

    // Two open square buckets with raised handles near the right wall.
    applyMaterial(shader,Materials::metal());
    for(const glm::vec3 centre:{glm::vec3(12.75f,0.0f,-12.85f),glm::vec3(13.55f,0.0f,-12.55f)})
    {
        cubes_.drawColored(shader,centre+glm::vec3(0,0.08f,0),glm::vec3(0.62f,0.12f,0.62f),glm::vec3(0.38f,0.42f,0.44f));
        for(float side:{-1.0f,1.0f})
        {
            cubes_.drawColored(shader,centre+glm::vec3(side*0.29f,0.36f,0),glm::vec3(0.07f,0.58f,0.62f),glm::vec3(0.45f,0.49f,0.51f));
            cubes_.drawColored(shader,centre+glm::vec3(0,0.36f,side*0.29f),glm::vec3(0.62f,0.58f,0.07f),glm::vec3(0.45f,0.49f,0.51f));
        }
        cubes_.drawColored(shader,centre+glm::vec3(-0.28f,0.82f,0),glm::vec3(0.05f,0.62f,0.05f),glm::vec3(0.22f,0.24f,0.25f));
        cubes_.drawColored(shader,centre+glm::vec3(0.28f,0.82f,0),glm::vec3(0.05f,0.62f,0.05f),glm::vec3(0.22f,0.24f,0.25f));
        cubes_.drawColored(shader,centre+glm::vec3(0,1.11f,0),glm::vec3(0.58f,0.05f,0.05f),glm::vec3(0.22f,0.24f,0.25f));
    }
}
