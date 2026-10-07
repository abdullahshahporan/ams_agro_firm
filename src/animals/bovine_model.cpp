// Item: shared cow/calf anatomy. Animals face local +X; all dimensions
// are diameters because the reusable sphere has radius 0.5.
#include "entity_renderer.h"
#include "material.h"
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>

using namespace std;

EntityRenderer::~EntityRenderer()
{
    for (const BovineMesh* mesh : {&bovineHead_, &bovineBody_, &bullBody_})
    {
        glDeleteBuffers(1, &mesh->ebo);
        glDeleteBuffers(1, &mesh->vbo);
        glDeleteVertexArrays(1, &mesh->vao);
    }
}

EntityRenderer::BovineMesh EntityRenderer::createBovineHead()
{
    // Broader poll/cheeks and a shorter nasal bridge avoid a horse-like face.
    // X is shortened in the head transform; all facial details use that
    // same transform, so the eyes, ears, horns and nostrils remain attached.
    const vector<glm::vec4> rings = {
        {-0.28f, 0.08f, 0.0f, 0.0f},
        {-0.12f, 0.08f, 0.28f, 0.30f},
        {0.16f, 0.12f, 0.36f, 0.32f},
        {0.42f, 0.26f, 0.28f, 0.31f},
        {0.69f, 0.35f, 0.28f, 0.32f},
        {0.87f, 0.24f, 0.28f, 0.30f},
        {1.08f, 0.08f, 0.235f, 0.255f},
        {1.28f, -0.085f, 0.19f, 0.235f},
        {1.48f, -0.18f, 0.14f, 0.265f},
        {1.64f, -0.19f, 0.115f, 0.245f},
        {1.70f, -0.19f, 0.092f, 0.20f},
        {1.73f, -0.19f, 0.0f, 0.0f}
    };
    return createBovineSurface(rings);
}

EntityRenderer::BovineMesh EntityRenderer::createBovineBody(bool bull)
{
    // X, back height, belly height, half-width. Unlike a scaled sphere,
    // this profile has a nearly level back, a full rib cage, a gentle
    // abdominal sag and a tucked-up rear flank, all in one connected mesh.
    const array<glm::vec4, 11> outline = {{
        {-1.68f, 1.73f, 1.73f, 0.0f},
        {-1.48f, 2.11f, 1.24f, 0.37f},
        {-1.14f, 2.27f, 1.02f, 0.53f},
        {-0.80f, 2.28f, 0.91f, 0.61f},
        {-0.32f, 2.25f, 0.85f, 0.67f},
        {0.18f, 2.25f, 0.88f, 0.66f},
        {0.62f, 2.27f, 0.99f, 0.59f},
        {0.96f, 2.23f, 1.07f, 0.53f},
        {1.22f, 2.08f, 1.16f, 0.43f},
        {1.43f, 1.87f, 1.41f, 0.28f},
        {1.52f, 1.67f, 1.67f, 0.0f}
    }};
    vector<glm::vec4> rings;
    for (const glm::vec4& section : outline)
    {
        const float shoulder = section.w > 0.0f
            ? exp(-pow((section.x - 0.73f) / 0.40f, 2.0f)) : 0.0f;
        // The ox's hump is sculpted into the topline instead of sitting
        // on the back as a separate oval. Dairy cows retain a leaner back.
        const float top = section.y + (bull ? 0.28f * shoulder : 0.0f);
        const float bottom = section.z - (bull ? 0.035f * shoulder : 0.0f);
        rings.push_back(glm::vec4(section.x, (top + bottom) * 0.5f,
                                  (top - bottom) * 0.5f,
                                  section.w * (bull ? 1.035f : 1.0f)));
    }
    return createBovineSurface(rings);
}

EntityRenderer::BovineMesh EntityRenderer::createBovineSurface(
    const vector<glm::vec4>& rings)
{
    // Each ring stores X, centre Y, vertical radius and lateral radius.
    // Smooth profile interpolation + derivative normals serve both meshes.
    constexpr int subdivisions = 5;
    constexpr int sectors = 32;
    vector<glm::vec4> profile;
    for (int segment = 0; segment + 1 < static_cast<int>(rings.size()); ++segment)
    {
        const glm::vec4 p0 = rings[max(0, segment - 1)];
        const glm::vec4 p1 = rings[segment];
        const glm::vec4 p2 = rings[segment + 1];
        const glm::vec4 p3 = rings[min(segment + 2, static_cast<int>(rings.size()) - 1)];
        for (int sample = 0; sample < subdivisions; ++sample)
        {
            const float t = static_cast<float>(sample) / subdivisions;
            glm::vec4 ring = 0.5f * ((2.0f * p1) + (-p0 + p2) * t
                + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t
                + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
            ring.z = max(0.0f, ring.z);
            ring.w = max(0.0f, ring.w);
            profile.push_back(ring);
        }
    }
    profile.push_back(rings.back());

    vector<float> vertices;
    vector<unsigned int> indices;
    for (int row = 0; row < static_cast<int>(profile.size()); ++row)
    {
        const glm::vec4 p = profile[row];
        const glm::vec4 slope = profile[min(row + 1, static_cast<int>(profile.size()) - 1)]
                              - profile[max(0, row - 1)];
        for (int sector = 0; sector <= sectors; ++sector)
        {
            const float angle = glm::two_pi<float>() * sector / sectors;
            const float c = cos(angle), s = sin(angle);
            const glm::vec3 point(p.x, p.y + p.z * c, p.w * s);
            const glm::vec3 along(slope.x, slope.y + slope.z * c, slope.w * s);
            const glm::vec3 around(0.0f, -p.z * s, p.w * c);
            glm::vec3 normal = glm::cross(around, along);
            if (glm::length(normal) < 0.000001f)
                normal = glm::vec3(row == 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
            else
                normal = glm::normalize(normal);
            vertices.insert(vertices.end(), {point.x, point.y, point.z,
                normal.x, normal.y, normal.z,
                static_cast<float>(sector) / sectors,
                static_cast<float>(row) / (profile.size() - 1)});
        }
    }
    for (unsigned int row = 0; row + 1 < profile.size(); ++row)
        for (unsigned int sector = 0; sector < sectors; ++sector)
        {
            const unsigned int a = row * (sectors + 1) + sector;
            const unsigned int b = a + sectors + 1;
            indices.insert(indices.end(), {a, a + 1, b, a + 1, b + 1, b});
        }

    BovineMesh mesh;
    mesh.indexCount = static_cast<int>(indices.size());
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return mesh;
}

void EntityRenderer::drawBovineMesh(const BovineMesh& mesh, const Shader& shader,
                                     const glm::mat4& model, const glm::vec3& color) const
{
    shader.setMat4("model", model);
    shader.setVec3("objectTint", color);
    shader.setVec2("uvScale", glm::vec2(1.0f));
    shader.setBool("useTexture", false);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
}


void EntityRenderer::drawBovine(
    const Shader& shader, const glm::mat4& root,
    const glm::vec3& bodyColor, const glm::vec3& patchColor,
    float gaitAngle, float headDrop, float headTurn,
    float tailAngle, float bodyBob, float hornScale, bool juvenile,
    bool showUdder, bool ox) const
{
    const bool dairy = showUdder && !juvenile;
    const bool bull = ox && !juvenile;
    const float bodyY = juvenile ? 1.30f : 1.62f;
    const float hipY = juvenile ? 1.14f : 1.45f;
    const float foreX = juvenile ? 0.82f : 1.07f;
    const float rearX = juvenile ? -0.82f : -1.08f;
    const float hipZ = juvenile ? 0.34f : 0.43f;
    const glm::vec3 legColor = bull ? bodyColor * 0.86f : bodyColor * 0.96f;
    const glm::mat4 body = glm::translate(root, glm::vec3(0.0f, bodyBob, 0.0f));

    // The coat is a surface colour, not extra flattened spheres hovering
    // above the flanks. Always reset the uniform before drawing other items.
    auto skin = [&](const glm::mat4& parent, const glm::vec3& center,
                    const glm::vec3& size, const glm::vec3& base,
                    int coat = 0, float angle = 0.0f)
    {
        shader.setInt("cowCoatMode", coat);
        shader.setVec3("cowPatchColor", coat == 2 ? bodyColor : patchColor);
        primitives_.drawSphere(shader, parent, center, size, base, angle,
                               glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setInt("cowCoatMode", 0);
    };
    // Tapered curved horns and tail segments: orient a cone from base to tip.
    auto taper = [&](const glm::mat4& parent, const glm::vec3& start,
                     const glm::vec3& end, float diameter,
                     const glm::vec3& color)
    {
        const glm::vec3 span = end - start;
        const float length = glm::length(span);
        if (length < 0.0001f) return;
        const glm::vec3 direction = span / length;
        const glm::vec3 up(0.0f, 1.0f, 0.0f);
        const float cosine = glm::clamp(glm::dot(up, direction), -1.0f, 1.0f);
        glm::vec3 axis = glm::cross(up, direction);
        if (glm::length(axis) < 0.0001f) axis = glm::vec3(1.0f, 0.0f, 0.0f);
        primitives_.drawCone(shader, parent, (start + end) * 0.5f,
                             glm::vec3(diameter, length, diameter), color,
                             glm::degrees(acos(cosine)), glm::normalize(axis));
    };

    applyMaterial(shader, Materials::cow());
    const int coat = dairy ? 1 : (juvenile ? 3 : 0);
    glm::mat4 torso = body;
    if (juvenile)
        torso = glm::scale(torso, glm::vec3(0.79f, 0.81f, 0.79f));
    shader.setInt("cowCoatMode", coat);
    shader.setVec3("cowPatchColor", patchColor);
    drawBovineMesh(bull ? bullBody_ : bovineBody_, shader, torso, bodyColor);
    shader.setInt("cowCoatMode", 0);
    if (bull)
        skin(body, glm::vec3(1.20f, 1.20f, 0.0f),
             glm::vec3(0.66f, 0.44f, 0.16f), bodyColor * 0.91f, 0, -22.0f);

    drawBovineLeg(shader, body, glm::vec3(foreX, hipY, hipZ), gaitAngle, legColor, juvenile);
    drawBovineLeg(shader, body, glm::vec3(foreX, hipY, -hipZ), -gaitAngle, legColor, juvenile);
    drawBovineLeg(shader, body, glm::vec3(rearX, hipY, hipZ), -gaitAngle, legColor, juvenile);
    drawBovineLeg(shader, body, glm::vec3(rearX, hipY, -hipZ), gaitAngle, legColor, juvenile);

    glm::mat4 neck = glm::translate(body, glm::vec3(juvenile ? 1.10f : 1.28f,
                                                  juvenile ? 1.45f : 1.78f, 0.0f));
    neck = glm::rotate(neck, glm::radians(-headDrop), glm::vec3(0.0f, 0.0f, 1.0f));
    neck = glm::rotate(neck, glm::radians(headTurn), glm::vec3(0.0f, 1.0f, 0.0f));
    // Calves have a slightly shorter face, not an oversized adult head.
    neck = glm::scale(neck, juvenile ? glm::vec3(0.70f, 0.87f, 0.83f)
                                      : glm::vec3(0.84f, 1.0f, 1.0f));
    const glm::vec3 face = dairy ? patchColor : bodyColor * (bull ? 0.91f : 1.0f);
    const glm::vec3 muzzle = dairy ? glm::vec3(0.66f, 0.48f, 0.44f)
                                  : glm::vec3(0.27f, 0.20f, 0.16f);
    // One smoothly interpolated ring mesh joins neck, skull, bridge and nose.
    shader.setInt("cowCoatMode", dairy ? 2 : 4);
    shader.setVec3("cowPatchColor", bodyColor);
    shader.setVec3("cowMuzzleColor", muzzle);
    drawBovineMesh(bovineHead_, shader, neck, face);
    shader.setInt("cowCoatMode", 0);
    // Paired nostrils, subtle mouth line, and side-set eyes.
    for (float side : {-1.0f, 1.0f})
    {
        skin(neck, glm::vec3(1.718f, -0.155f, side * 0.125f),
             glm::vec3(0.030f, 0.050f, 0.075f), glm::vec3(0.06f, 0.045f, 0.038f));
        skin(neck, glm::vec3(0.94f, 0.33f, side * 0.247f),
             glm::vec3(0.12f, 0.09f, 0.038f), face * 0.72f);
        skin(neck, glm::vec3(0.953f, 0.333f, side * 0.265f),
             glm::vec3(0.060f, 0.051f, 0.025f), glm::vec3(0.025f, 0.021f, 0.017f));
        skin(neck, glm::vec3(0.967f, 0.348f, side * 0.277f),
             glm::vec3(0.014f, 0.012f, 0.006f), glm::vec3(0.76f));

        const float earAngle = side * (bull ? 22.0f : 8.0f);
        primitives_.drawSphere(shader, neck, glm::vec3(0.67f, 0.45f, side * 0.41f),
                               glm::vec3(0.27f, 0.11f, 0.40f), face, earAngle,
                               glm::vec3(1.0f, 0.0f, 0.0f));
        primitives_.drawSphere(shader, neck, glm::vec3(0.69f, 0.478f, side * 0.425f),
                               glm::vec3(0.18f, 0.028f, 0.26f),
                               glm::vec3(0.48f, 0.31f, 0.28f), earAngle,
                               glm::vec3(1.0f, 0.0f, 0.0f));
        if (!juvenile && hornScale > 0.0f)
        {
            const float height = (bull ? 0.24f : 0.12f) + hornScale * 0.26f;
            const glm::vec3 base(0.70f, 0.57f, side * 0.22f);
            const glm::vec3 bend(0.66f, 0.57f + height * 0.55f, side * 0.30f);
            const glm::vec3 tip(0.73f, 0.57f + height, side * 0.37f);
            taper(neck, base, bend + (bend - base) * 0.45f, bull ? 0.105f : 0.085f,
                  glm::vec3(0.72f, 0.65f, 0.48f));
            taper(neck, bend - glm::vec3(0.0f, 0.025f, 0.0f), tip, 0.054f,
                  glm::vec3(0.40f, 0.35f, 0.25f));
        }
    }
    cubes_.drawColored(shader, neck, glm::vec3(1.718f, -0.236f, 0.0f),
                       glm::vec3(0.017f, 0.012f, 0.31f), muzzle * 0.40f);

    glm::mat4 tail = glm::translate(body,
        glm::vec3(juvenile ? -1.24f : -1.61f, bodyY + 0.23f, 0.0f));
    tail = glm::rotate(tail, glm::radians(tailAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    const float tailLength = juvenile ? 0.72f : 1.04f;
    skin(tail, glm::vec3(-0.06f, -tailLength * 0.5f, 0.0f),
         glm::vec3(0.075f, tailLength, 0.075f), bodyColor, 0, -7.0f);
    skin(tail, glm::vec3(-0.12f, -tailLength, 0.0f),
         glm::vec3(0.12f, 0.27f, 0.13f), dairy ? patchColor : bodyColor * 0.38f);

    if (dairy)
    {
        skin(body, glm::vec3(-0.57f, 0.96f, 0.0f),
             glm::vec3(0.62f, 0.35f, 0.47f), glm::vec3(0.76f, 0.52f, 0.48f));
        for (float x : {-0.72f, -0.42f})
            for (float z : {-0.13f, 0.13f})
                skin(body, glm::vec3(x, 0.74f, z),
                     glm::vec3(0.068f, 0.17f, 0.067f), glm::vec3(0.66f, 0.43f, 0.40f));
    }
    shader.setInt("cowCoatMode", 0);
}

void EntityRenderer::drawBovineLeg(
    const Shader& shader, const glm::mat4& root, const glm::vec3& hip,
    float swing, const glm::vec3& legColor, bool juvenile) const
{
    const float upperLength = juvenile ? 0.51f : 0.68f;
    const float lowerLength = juvenile ? 0.50f : 0.63f;
    const float thickness = juvenile ? 0.17f : 0.23f;
    glm::mat4 upper = glm::translate(root, hip);
    upper = glm::rotate(upper, glm::radians(swing), glm::vec3(0.0f, 0.0f, 1.0f));
    primitives_.drawSphere(shader, upper, glm::vec3(0.0f, -upperLength * 0.44f, 0.0f),
                           glm::vec3(thickness * 1.35f, upperLength * 1.30f, thickness * 1.18f),
                           legColor);
    glm::mat4 knee = glm::translate(upper, glm::vec3(0.0f, -upperLength, 0.0f));
    knee = glm::rotate(knee, glm::radians(-swing * 0.36f), glm::vec3(0.0f, 0.0f, 1.0f));
    primitives_.drawSphere(shader, knee, glm::vec3(0.0f),
                           glm::vec3(thickness * 0.83f), legColor * 0.94f);
    primitives_.drawSphere(shader, knee, glm::vec3(0.0f, -lowerLength * 0.46f, 0.0f),
                           glm::vec3(thickness * 0.66f, lowerLength * 1.14f, thickness * 0.72f),
                           legColor * 0.85f);
    // Two flattened toe halves leave a readable cloven-hoof seam.
    for (float side : {-1.0f, 1.0f})
        cubes_.drawColored(shader, knee, glm::vec3(0.035f, -lowerLength - 0.025f,
                                                  side * thickness * 0.28f),
                           glm::vec3(thickness * 1.10f, 0.13f, thickness * 0.48f),
                           glm::vec3(0.12f, 0.105f, 0.088f));
}
