#ifndef AMS_AGRO_FARM_ENTITY_TRANSFORM_H
#define AMS_AGRO_FARM_ENTITY_TRANSFORM_H

#include <glm/gtc/matrix_transform.hpp>

namespace EntityTransform
{
inline glm::mat4 makeRoot(const glm::vec3& position, float yaw, float scale)
{
    glm::mat4 root(1.0f);
    root = glm::translate(root, position);
    root = glm::rotate(root, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::scale(root, glm::vec3(scale));
}
}

#endif
