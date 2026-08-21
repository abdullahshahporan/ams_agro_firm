#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 fragmentColor;

uniform sampler2D texture1;
uniform vec3 objectTint;
uniform bool useTexture;

void main()
{
    vec4 surfaceColor = useTexture
        ? texture(texture1, TexCoord) * vec4(objectTint, 1.0)
        : vec4(objectTint, 1.0);

    // Constant display brightness only; full illumination remains a Module 4 task.
    fragmentColor = vec4(surfaceColor.rgb * 0.96, surfaceColor.a);
}
