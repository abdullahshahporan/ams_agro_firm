#ifndef AMS_AGRO_FARM_COLLISION_SYSTEM_H
#define AMS_AGRO_FARM_COLLISION_SYSTEM_H

#include <glm/glm.hpp>

#include <array>
#include <cmath>

class CollisionSystem
{
public:
    static bool canOccupy(const glm::vec3& position, float gateAngleDegrees,
                          float stallGateAngleDegrees,
                          float calfShedDoorAngleDegrees)
    {
        constexpr float radius = 0.38f;
        if (position.y < 0.45f || position.y > 30.0f ||
            std::abs(position.x) > 25.0f || std::abs(position.z) > 28.0f)
            return false;

        struct Box { float minX, maxX, minZ, maxZ, height; };
        const std::array<Box, 29> obstacles = {{
            {-14.3f,-5.7f,-11.8f,-11.0f,4.8f}, // shed back wall
            {-14.3f,-13.45f,-11.8f,-5.0f,4.2f},
            { -6.55f,-5.7f,-11.8f,-10.22f,4.2f}, // ox-side wall behind calf door
            { -6.55f,-5.7f, -8.98f, -5.0f,4.2f}, // ox-side wall ahead of door
            {-14.4f,-13.6f, -8.4f,-7.1f,4.2f},
            {-10.4f, -9.6f, -8.4f,-7.1f,4.2f},
            { -6.4f, -5.6f, -8.4f,-7.1f,4.2f},
            {-13.6f,-10.4f, -6.05f,-4.85f,1.2f}, // indoor troughs
            { -9.6f, -6.4f, -6.05f,-4.85f,1.2f},
            {  7.5f,  7.95f,-14.2f,-7.8f,5.2f}, // barn walls
            { 14.05f,14.5f,-14.2f,-7.8f,5.2f},
            {  7.5f,14.5f,-14.3f,-13.75f,5.2f},
            {  7.5f, 9.7f, -8.35f,-7.75f,5.2f},
            { 12.3f,14.5f, -8.35f,-7.75f,5.2f},
            { 10.1f,15.9f, -2.1f, 3.1f,5.5f}, // water facility
            {  8.0f, 8.8f,  5.5f, 9.2f,2.8f}, // poultry side
            { 11.2f,12.0f,  5.5f, 9.2f,2.8f},
            {  8.0f,12.0f,  8.5f, 9.3f,2.8f},
            {-11.5f,-4.8f,  1.0f, 5.0f,2.8f}, // hay stacks
            {-12.8f,-11.2f, -9.45f,-6.55f,2.4f}, // milch cow
            { -8.8f, -7.2f, -9.55f,-6.45f,2.5f}, // ox
            {  6.2f,  6.65f,10.2f,13.7f,3.5f},  // relocated worker house
            {  9.35f, 9.8f,10.2f,13.7f,3.5f},
            {  6.2f,  9.8f,13.25f,13.7f,3.5f},
            {  6.2f,  7.3f,10.2f,10.7f,3.5f},
            {  8.7f,  9.8f,10.2f,10.7f,3.5f},
            {  6.4f, 10.2f,-2.4f, 0.4f,2.2f},   // milk collection platform
            {-14.3f,-13.8f,10.4f,11.0f,4.3f},   // billboard supports
            { -6.2f, -5.7f,10.4f,11.0f,4.3f}
        }};
        for (const Box& box : obstacles)
        {
            // Position is the viewer's eye, so include an approximate body
            // height below it when testing low troughs, cattle, and rails.
            if (position.y <= box.height + 1.8f &&
                position.x + radius > box.minX && position.x - radius < box.maxX &&
                position.z + radius > box.minZ && position.z - radius < box.maxZ)
                return false;
        }

        const std::array<glm::vec2, 8> trees = {{
            {-15.8f,-14.5f}, {-16.4f,7.2f}, {-16.6f,13.0f}, {15.8f,-15.4f},
            {16.0f,6.0f}, {13.6f,12.5f}, {-2.0f,-16.0f}, {5.5f,-16.1f}
        }};
        if (position.y < 7.6f)
            for (const glm::vec2& tree : trees)
                if (glm::length(glm::vec2(position.x, position.z) - tree) < 0.72f)
                    return false;

        // The perimeter is a thin collision plane with only the entrance gap.
        if (position.y < 5.3f)
        {
            if ((std::abs(position.x - 17.5f) < radius ||
                 std::abs(position.x + 17.5f) < radius) &&
                position.z > -17.8f && position.z < 15.8f)
                return false;
            if (std::abs(position.z + 17.5f) < radius && std::abs(position.x) < 17.8f)
                return false;
            if (std::abs(position.z - 15.5f) < radius && std::abs(position.x) >= 3.72f)
                return false;
        }

        // Entrance masonry pillars.
        if (position.y < 6.9f)
            for (float pillarX : {-4.3f, 4.3f})
                if (std::abs(position.x - pillarX) < 0.82f &&
                    std::abs(position.z - 15.5f) < 0.82f)
                    return false;

        const auto distanceToSegment = [](const glm::vec2& point,
                                          const glm::vec2& start,
                                          const glm::vec2& end)
        {
            const glm::vec2 segment = end - start;
            const float lengthSquared = glm::dot(segment, segment);
            if (lengthSquared < 0.000001f)
                return glm::length(point - start);
            const float t = glm::clamp(glm::dot(point - start, segment) / lengthSquared,
                                       0.0f, 1.0f);
            return glm::length(point - (start + segment * t));
        };

        if (position.y < 5.1f)
        {
            const float angle = glm::radians(gateAngleDegrees);
            const glm::vec2 leftStart(-3.82f, 15.47f);
            const glm::vec2 rightStart(3.82f, 15.47f);
            const glm::vec2 leftEnd = leftStart + glm::vec2(std::cos(angle) * 3.82f,
                                                            -std::sin(angle) * 3.82f);
            const glm::vec2 rightEnd = rightStart + glm::vec2(-std::cos(angle) * 3.82f,
                                                              -std::sin(angle) * 3.82f);
            const glm::vec2 point(position.x, position.z);
            if (distanceToSegment(point, leftStart, leftEnd) < radius + 0.10f ||
                distanceToSegment(point, rightStart, rightEnd) < radius + 0.10f)
                return false;
        }

        if (position.y < 3.4f)
        {
            const float angle = glm::radians(stallGateAngleDegrees);
            const glm::vec2 point(position.x, position.z);
            for (float hingeX : {-14.0f, -10.0f})
            {
                const glm::vec2 start(hingeX, -9.65f);
                const glm::vec2 end = start + glm::vec2(std::cos(angle) * 4.0f,
                                                        -std::sin(angle) * 4.0f);
                if (distanceToSegment(point, start, end) < radius + 0.08f)
                    return false;
            }
        }

        // The calf door opens outward from the wall beside the ox. Collision
        // follows the same hinge and swing used by the visible wooden leaf.
        if (position.y < 3.8f)
        {
            const float angle = glm::radians(calfShedDoorAngleDegrees);
            const glm::vec2 point(position.x, position.z);
            const glm::vec2 start(-6.06f, -10.24f);
            const glm::vec2 end = start +
                glm::vec2(std::sin(angle) * 1.24f, std::cos(angle) * 1.24f);
            if (distanceToSegment(point, start, end) < radius + 0.08f)
                return false;
        }
        return true;
    }
};

#endif
