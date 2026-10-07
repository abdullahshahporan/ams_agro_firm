#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 CoatPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uvScale;

void main()
{
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    FragPos = worldPosition.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord * uvScale;
    CoatPosition = aPosition * 2.0;
    gl_Position = projection * view * worldPosition;
}
