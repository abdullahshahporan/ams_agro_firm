#ifndef AMS_AGRO_FARM_CURVE_VERTEX_H
#define AMS_AGRO_FARM_CURVE_VERTEX_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;

namespace CurveGeometry
{
inline void vertex(vector<float>& output, const glm::vec3& position,
            const glm::vec3& normal, const glm::vec2& uv)
{
    output.insert(output.end(), {
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        uv.x, uv.y});
}
}

#endif
