#ifndef AMS_AGRO_FARM_COLLISION_SYSTEM_H
#define AMS_AGRO_FARM_COLLISION_SYSTEM_H

#include <glm/glm.hpp>

#include <array>
#include <cmath>

class CollisionSystem
{
public:
    static bool canOccupy(const glm::vec3& position, float gateAngleDegrees)
    {
        constexpr float radius = 0.38f;
        if (position.y < 0.45f || position.y > 30.0f ||
            std::abs(position.x) > 25.0f || std::abs(position.z) > 28.0f)
            return false;

        struct Box { float minX, maxX, minZ, maxZ, height; };
        const std::array<Box, 18> obstacles = {{
            {-14.3f,-5.7f,-11.8f,-11.0f,4.8f}, // shed back wall
            {-14.3f,-13.45f,-11.8f,-5.0f,4.2f},
            { -6.55f,-5.7f,-11.8f,-5.0f,4.2f},
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
            {-11.5f,-4.8f,  1.0f, 5.0f,2.8f}  // hay stacks
        }};
        for (const Box& box : obstacles)
        {
            if (position.y <= box.height &&
                position.x + radius > box.minX && position.x - radius < box.maxX &&
                position.z + radius > box.minZ && position.z - radius < box.maxZ)
                return false;
        }

        const std::array<glm::vec2, 8> trees = {{
            {-15.8f,-14.5f}, {-15.7f,7.5f}, {-14.2f,13.0f}, {15.8f,-15.4f},
            {16.0f,6.0f}, {13.6f,12.5f}, {-2.0f,-16.0f}, {5.5f,-16.1f}
        }};
        if (position.y < 5.8f)
            for (const glm::vec2& tree : trees)
                if (glm::length(glm::vec2(position.x, position.z) - tree) < 0.72f)
                    return false;

        // The perimeter is a thin collision plane with only the entrance gap.
        if (position.y < 3.5f)
        {
            if ((std::abs(position.x - 17.5f) < radius ||
                 std::abs(position.x + 17.5f) < radius) &&
                position.z > -17.8f && position.z < 15.8f)
                return false;
            if (std::abs(position.z + 17.5f) < radius && std::abs(position.x) < 17.8f)
                return false;
            if (std::abs(position.z - 15.5f) < radius)
            {
                const bool inEntrance = std::abs(position.x) < 3.72f;
                if (!inEntrance || gateAngleDegrees < 58.0f)
                    return false;
            }
        }

        // Entrance masonry pillars.
        if (position.y < 5.1f)
            for (float pillarX : {-4.3f, 4.3f})
                if (std::abs(position.x - pillarX) < 0.82f &&
                    std::abs(position.z - 15.5f) < 0.82f)
                    return false;
        return true;
    }
};

#endif
