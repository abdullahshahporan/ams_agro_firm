#ifndef AMS_AGRO_FARM_TERRAIN_RENDERER_H
#define AMS_AGRO_FARM_TERRAIN_RENDERER_H

#include "shader.h"

// Ground implementation stays with the ground item in src/items/ground.cpp.
class TerrainRenderer
{
public:
    static constexpr float Radius = 1400.0f;
    static constexpr float FarPlane = 1800.0f;
    static constexpr float FogStart = 130.0f;
    static constexpr float FogEnd = 700.0f;

    TerrainRenderer();
    ~TerrainRenderer();
    TerrainRenderer(const TerrainRenderer&) = delete;
    TerrainRenderer& operator=(const TerrainRenderer&) = delete;

    static float height(float x, float z);
    void draw(const Shader& shader, unsigned int grassTexture, bool textured) const;

private:
    unsigned int vao_{0}, vbo_{0}, ebo_{0};
    int indexCount_{0};
};

#endif
