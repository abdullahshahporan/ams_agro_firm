#ifndef AMS_AGRO_FARM_MATERIAL_H
#define AMS_AGRO_FARM_MATERIAL_H

#include "shader.h"

#include <glm/glm.hpp>

struct Material
{
    glm::vec3 specularColor;
    float shininess;
    glm::vec3 emission;
};

namespace Materials
{
inline Material grass()    { return {glm::vec3(0.05f),  6.0f, glm::vec3(0.0f)}; }
inline Material soil()     { return {glm::vec3(0.04f),  4.0f, glm::vec3(0.0f)}; }
inline Material wood()     { return {glm::vec3(0.14f), 12.0f, glm::vec3(0.0f)}; }
inline Material brick()    { return {glm::vec3(0.10f), 10.0f, glm::vec3(0.0f)}; }
inline Material concrete() { return {glm::vec3(0.16f), 18.0f, glm::vec3(0.0f)}; }
inline Material metal()    { return {glm::vec3(0.82f), 64.0f, glm::vec3(0.0f)}; }
inline Material cow()      { return {glm::vec3(0.14f), 12.0f, glm::vec3(0.0f)}; }
inline Material cloth()    { return {glm::vec3(0.09f),  8.0f, glm::vec3(0.0f)}; }
inline Material hay()      { return {glm::vec3(0.05f),  5.0f, glm::vec3(0.0f)}; }
}

inline void applyMaterial(const Shader& shader, const Material& material)
{
    shader.setVec3("material.specularColor", material.specularColor);
    shader.setFloat("material.shininess", material.shininess);
    shader.setVec3("material.emission", material.emission);
}

#endif
