#version 330 core

struct Material
{
    vec3 specularColor;
    float shininess;
    vec3 emission;
};

in vec2 TexCoord;
in vec3 VertexLighting;
in vec3 VertexSpecular;
in vec3 CoatPosition;
in vec3 FragPos;
out vec4 fragmentColor;

uniform sampler2D texture1;
uniform vec3 objectTint;
uniform bool useTexture;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform int cowCoatMode;
uniform vec3 cowPatchColor;
uniform vec3 cowMuzzleColor;

// Same surface markings as Phong; lighting still comes from the vertices.
vec3 bovineAlbedo(vec3 base)
{
    vec3 p = CoatPosition;
    if (cowCoatMode == 2 || cowCoatMode == 4)
    {
        float stripe = 1.0 - smoothstep(0.12, 0.23,
            abs(p.z + 0.035 * sin(p.y * 9.0)));
        vec3 face = cowCoatMode == 2
            ? mix(base, cowPatchColor, stripe * smoothstep(0.0, 0.35, p.x)) : base;
        return mix(face, cowMuzzleColor, smoothstep(2.86, 3.12, p.x));
    }
    if (cowCoatMode == 1 || cowCoatMode == 3)
    {
        // The sculpted torso uses real local dimensions, not a unit sphere.
        p = (p - vec3(0.0, 3.20, 0.0)) / vec3(3.20, 1.45, 1.35);
        float blotch = sin(6.0 * p.x + 0.6 * sin(9.0 * p.z))
                     + 0.65 * sin(8.0 * p.z - 1.4 * p.x)
                     + 0.40 * sin(11.0 * p.y + 3.0 * p.z)
                     + 0.15 * sin(25.0 * p.x + 7.0 * p.y);
        float threshold = cowCoatMode == 3 ? 0.65 : 0.20;
        return mix(base, cowPatchColor,
                   smoothstep(threshold, threshold + 0.06, blotch));
    }
    return base;
}

void main()
{
    vec4 sampled = useTexture ? texture(texture1, TexCoord) : vec4(1.0);
    vec3 albedo = bovineAlbedo(sampled.rgb * objectTint);
    vec3 result = albedo * VertexLighting + VertexSpecular
                  + material.emission * albedo;
    float haze = smoothstep(fogStart, fogEnd, length(viewPos - FragPos));
    result = mix(result, fogColor, haze);
    fragmentColor = vec4(result, sampled.a);
}
