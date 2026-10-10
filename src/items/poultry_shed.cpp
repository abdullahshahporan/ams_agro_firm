// Item: poultry shed.
#include "farm_scene.h"
#include "lighting_system.h"
#include "shared/farm_style.h"
#include "material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cmath>

using namespace std;
using namespace FarmStyle;

void FarmScene::drawPoultryShed(const Shader& shader, float doorAngleDegrees) const
{
    const glm::vec3 origin(10.0f, 0.0f, 7.3f);
    applyMaterial(shader, Materials::wood());
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 0.08f, 0.0f),
                        glm::vec3(3.4f, 0.16f, 3.4f), textures_.wood,
                        glm::vec3(0.72f, 0.58f, 0.38f), glm::vec2(3.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.0f, 1.05f, 1.55f),
                        glm::vec3(3.3f, 2.0f, 0.18f), textures_.wood,
                        WarmWood, glm::vec2(3.0f, 2.0f));
    for (float x : {-1.58f, 1.58f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.0f, 0.25f),
                            glm::vec3(0.18f, 1.9f, 2.8f), textures_.wood,
                            WarmWood, glm::vec2(2.0f));
    for (float x : {-1.55f, 1.55f})
        cubes_.drawTextured(shader, origin + glm::vec3(x, 1.25f, -1.48f),
                            glm::vec3(0.16f, 2.5f, 0.16f), textures_.wood, WarmWood);

    // A real slatted gate closes the open front of the poultry room.  It
    // swings outward before the staged doorway routes release the birds and
    // closes only after every mobile hen and duck has returned.
    // The east hinge is offset beyond the side wall. Swing toward the back
    // of the coop, so the open leaf clears both the west hose and north tank.
    cubes_.drawTextured(shader,origin+glm::vec3(1.65f,.72f,-1.58f),
                        glm::vec3(.28f,.15f,.12f),textures_.metal,DarkMetal);
    glm::mat4 gate=glm::translate(glm::mat4(1.0f),origin+glm::vec3(1.76f,0.0f,-1.58f));
    gate=glm::rotate(gate,glm::radians(180.0f+doorAngleDegrees),glm::vec3(0,1,0));
    for(float x:{0.08f,3.20f})
        cubes_.drawTextured(shader,gate,glm::vec3(x,0.72f,0),
                            glm::vec3(0.13f,1.34f,0.13f),textures_.wood,WarmWood);
    for(float y:{0.24f,0.72f,1.20f})
        cubes_.drawTextured(shader,gate,glm::vec3(1.64f,y,0),
                            glm::vec3(3.28f,0.11f,0.11f),textures_.wood,WarmWood,
                            glm::vec2(3.0f,.5f));

    applyMaterial(shader, Materials::metal());
    cubes_.drawTextured(shader, origin + glm::vec3(-0.78f, 2.18f, 0.0f),
                        glm::vec3(1.95f, 0.16f, 3.7f), textures_.roof,
                        RoofTint, glm::vec2(2.0f, 3.0f), 18.0f,
                        glm::vec3(0.0f, 0.0f, 1.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(0.78f, 2.18f, 0.0f),
                        glm::vec3(1.95f, 0.16f, 3.7f), textures_.roof,
                        RoofTint, glm::vec2(2.0f, 3.0f), -18.0f,
                        glm::vec3(0.0f, 0.0f, 1.0f));

    applyMaterial(shader, Materials::hay());
    cubes_.drawTextured(shader, origin + glm::vec3(0.20f, 0.28f, 0.70f),
                        glm::vec3(1.55f, 0.30f, 1.05f), textures_.hay,
                        HayTint, glm::vec2(2.0f));
    cubes_.drawTextured(shader, origin + glm::vec3(-0.95f, 0.32f, -0.75f),
                        glm::vec3(0.70f, 0.38f, 0.70f), textures_.hay,
                        glm::vec3(0.64f, 0.82f, 0.32f));
}
