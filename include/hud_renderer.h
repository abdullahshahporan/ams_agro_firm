#ifndef AMS_AGRO_FARM_HUD_RENDERER_H
#define AMS_AGRO_FARM_HUD_RENDERER_H

#include "shader.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

using namespace std;

// Small code-native 2D UI renderer.  It deliberately has no font or image
// dependency, so the classroom project remains portable and easy to build.
class HudRenderer
{
public:
    HudRenderer();
    ~HudRenderer();
    HudRenderer(const HudRenderer&) = delete;
    HudRenderer& operator=(const HudRenderer&) = delete;

    void begin();
    void rectangle(float x, float y, float width, float height,
                   const glm::vec4& color);
    void text(const string& value, float x, float y, float pixel,
              const glm::vec4& color);
    void flush(int width, int height);

private:
    Shader shader_;
    unsigned int vao_{0};
    unsigned int vbo_{0};
    vector<float> vertices_;
};

#endif
