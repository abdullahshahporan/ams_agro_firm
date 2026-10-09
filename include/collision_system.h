#ifndef AMS_AGRO_FARM_COLLISION_SYSTEM_H
#define AMS_AGRO_FARM_COLLISION_SYSTEM_H

#include <glm/glm.hpp>

#include <array>
#include <cmath>
#include "shared/farm_interaction.h"

using namespace std;

class CollisionSystem
{
public:
    static bool canOccupy(const glm::vec3& position, float gateAngleDegrees,
                          float stallGateAngleDegrees,
                          float calfShedDoorAngleDegrees,
                          float workerDoorAngle = 0.0f,
                          unsigned int hayAvailableMask = 0u)
    {
        constexpr float radius = 0.38f;
        if (position.y < 0.45f || position.y > 60.0f ||
            abs(position.x) > 125.0f || abs(position.z) > 125.0f)
            return false;

        struct Box { float minX, maxX, minZ, maxZ, height; };
        const array<Box, 29> obstacles = {{
            {-14.3f,-13.45f,-11.8f,-5.0f,4.2f},
            { -6.34f,-6.10f,-11.18f,-10.26f,4.2f}, // actual side-wall footprint
            { -6.34f,-6.10f, -9.00f, -8.16f,4.2f},
            {-14.4f,-13.6f, -8.4f,-7.1f,4.2f},
            {-10.4f, -9.6f, -8.4f,-7.1f,4.2f},
            { -6.08f,-5.92f,-7.73f,-5.45f,1.7f}, // actual ox-side stall post/rail
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
            {  7.3f,  8.7f,10.2f,10.7f,2.2f},  // closed worker-house door
            {  6.4f, 10.2f,-2.4f, 0.4f,2.2f},   // milk collection platform
            {-14.3f,-13.8f,10.4f,11.0f,4.3f},   // billboard supports
            { -6.2f, -5.7f,10.4f,11.0f,4.3f}
        }};
        for (size_t index = 0; index < obstacles.size(); ++index)
        {
            if (index == 25) // Actual rotated door leaf is tested below.
                continue;
            const Box& box = obstacles[index];
            // Position is the viewer's eye, so include an approximate body
            // height below it when testing low troughs, cattle, and rails.
            if (position.y <= box.height + 1.8f &&
                position.x + radius > box.minX && position.x - radius < box.maxX &&
                position.z + radius > box.minZ && position.z - radius < box.maxZ)
                return false;
        }

        // Dynamic pickup bundles block actors while they remain on the lawn.
        if (position.y <= 2.8f)
        {
            for (int index=0; index<FarmInteraction::HayCount; ++index)
            {
                if ((hayAvailableMask & (1u<<index)) == 0u)
                    continue;
                const glm::vec3 hay=FarmInteraction::hayPosition(index);
                if (position.x+radius>hay.x-0.47f && position.x-radius<hay.x+0.47f &&
                    position.z+radius>hay.z-0.36f && position.z-radius<hay.z+0.36f)
                    return false;
            }
        }

        const array<glm::vec2, 8> trees = {{
            {-15.8f,-14.5f}, {-16.4f,7.2f}, {-16.6f,13.0f}, {15.8f,-15.4f},
            {16.0f,6.0f}, {13.6f,12.5f}, {-2.0f,-16.0f}, {5.5f,-16.1f}
        }};
        if (position.y < 7.6f)
        {
            for (const glm::vec2& tree : trees)
                if (glm::length(glm::vec2(position.x, position.z) - tree) < 0.72f)
                    return false;
            for (int index = 0; index < 11; ++index)
            {
                const float z = -69.0f + index * 12.0f;
                const float offset = static_cast<float>(index % 3) * 2.4f;
                if (glm::length(glm::vec2(position.x, position.z) -
                                glm::vec2(-24.5f - offset, z)) < 0.72f ||
                    glm::length(glm::vec2(position.x, position.z) -
                                glm::vec2(24.5f + offset, z + 4.0f)) < 0.72f)
                    return false;
            }
        }

        // The perimeter is a thin collision plane with only the entrance gap.
        if (position.y < 5.3f)
        {
            if ((abs(position.x - 18.0f) < radius ||
                 abs(position.x + 18.0f) < radius) &&
                position.z > -18.0f && position.z < 15.8f)
                return false;
            if (abs(position.z + 18.0f) < radius && abs(position.x) < 18.0f)
                return false;
            if (abs(position.z - 15.5f) < radius && abs(position.x) >= 3.72f)
                return false;
        }

        // Entrance masonry pillars.
        if (position.y < 6.9f)
            for (float pillarX : {-4.3f, 4.3f})
                if (abs(position.x - pillarX) < 0.82f &&
                    abs(position.z - 15.5f) < 0.82f)
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

        if(position.y < 4.85f)
        {
            const float angle=glm::radians(workerDoorAngle);
            const glm::vec2 hinge(7.2f,10.48f);
            const glm::vec2 end=hinge+glm::vec2(1.6f*cos(angle),-1.6f*sin(angle));
            if(distanceToSegment(glm::vec2(position.x,position.z),hinge,end)<radius+.06f)return false;
        }

        if (position.y < 5.1f)
        {
            const float angle = glm::radians(gateAngleDegrees);
            const glm::vec2 leftStart(-3.82f, 15.47f);
            const glm::vec2 rightStart(3.82f, 15.47f);
            const glm::vec2 leftEnd = leftStart + glm::vec2(cos(angle) * 3.82f,
                                                            -sin(angle) * 3.82f);
            const glm::vec2 rightEnd = rightStart + glm::vec2(-cos(angle) * 3.82f,
                                                              -sin(angle) * 3.82f);
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
                const glm::vec2 start(hingeX, -11.05f);
                const glm::vec2 end = start + glm::vec2(cos(angle) * 4.0f,
                                                        -sin(angle) * 4.0f);
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
                glm::vec2(sin(angle) * 1.24f, cos(angle) * 1.24f);
            if (distanceToSegment(point, start, end) < radius + 0.08f)
                return false;
        }
        return true;
    }
};

#endif
