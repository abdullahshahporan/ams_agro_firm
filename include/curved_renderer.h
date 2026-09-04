#ifndef AMS_AGRO_FARM_CURVED_RENDERER_H
#define AMS_AGRO_FARM_CURVED_RENDERER_H

#include "shader.h"
#include "texture_manager.h"

#include <glm/glm.hpp>

class CurvedRenderer
{
public:
    CurvedRenderer();
    ~CurvedRenderer();

    CurvedRenderer(const CurvedRenderer&) = delete;
    CurvedRenderer& operator=(const CurvedRenderer&) = delete;

    void setTexturesEnabled(bool enabled) { texturesEnabled_ = enabled; }
    void render(const Shader& shader, const FarmTextures& textures) const;

private:
    struct Mesh
    {
        unsigned int vao{0};
        unsigned int vbo{0};
        unsigned int ebo{0};
        int indexCount{0};
    };

    static Mesh createMilkCan();
    static Mesh createSplinePipe();
    static Mesh createRuledRoof();
    static Mesh upload(const float* vertices, unsigned int vertexFloatCount,
                       const unsigned int* indices, unsigned int indexCount);
    static void destroy(Mesh& mesh);
    void draw(const Mesh& mesh, const Shader& shader, const glm::mat4& model,
              unsigned int texture, const glm::vec3& tint,
              const glm::vec2& uvScale) const;

    Mesh milkCan_;
    Mesh splinePipe_;
    Mesh ruledRoof_;
    bool texturesEnabled_{true};
};

#endif
