#version 330 core

#define NR_POINT_LIGHTS 5

struct Material
{
    vec3 specularColor;
    float shininess;
    vec3 emission;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 VertexLighting;
out vec3 VertexSpecular;
out vec3 CoatPosition;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uvScale;
uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform bool directionalLightEnabled;
uniform bool pointLightsEnabled;
uniform bool spotLightEnabled;
uniform bool ambientEnabled;
uniform bool diffuseEnabled;
uniform bool specularEnabled;
uniform vec3 sceneAmbient;

void addPhongTerms(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor,
                   vec3 lightDirection, vec3 normal, vec3 viewDirection,
                   float attenuation, inout vec3 lighting, inout vec3 specular)
{
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 reflected = reflect(-lightDirection, normal);
    float specularFactor = diffuseFactor > 0.0
        ? pow(max(dot(viewDirection, reflected), 0.0), material.shininess) : 0.0;
    if (ambientEnabled)
        lighting += ambientColor * attenuation;
    if (diffuseEnabled)
        lighting += diffuseColor * diffuseFactor * attenuation;
    if (specularEnabled)
        specular += specularColor * specularFactor
                    * material.specularColor * attenuation;
}

void main()
{
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    vec3 normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 viewDirection = normalize(viewPos - worldPosition.xyz);
    vec3 lighting = ambientEnabled ? sceneAmbient : vec3(0.0);
    vec3 specular = vec3(0.0);

    if (directionalLightEnabled)
    {
        addPhongTerms(dirLight.ambient, dirLight.diffuse, dirLight.specular,
                      normalize(-dirLight.direction), normal, viewDirection,
                      1.0, lighting, specular);
    }

    if (pointLightsEnabled)
    {
        for (int index = 0; index < NR_POINT_LIGHTS; ++index)
        {
            vec3 toLight = pointLights[index].position - worldPosition.xyz;
            float distanceToLight = length(toLight);
            float attenuation = 1.0 /
                (pointLights[index].constant
                 + pointLights[index].linear * distanceToLight
                 + pointLights[index].quadratic * distanceToLight * distanceToLight);
            addPhongTerms(pointLights[index].ambient, pointLights[index].diffuse,
                          pointLights[index].specular, normalize(toLight), normal,
                          viewDirection, attenuation, lighting, specular);
        }
    }

    if (spotLightEnabled)
    {
        vec3 toLight = spotLight.position - worldPosition.xyz;
        float distanceToLight = length(toLight);
        vec3 lightDirection = normalize(toLight);
        float theta = dot(lightDirection, normalize(-spotLight.direction));
        float epsilon = max(spotLight.cutOff - spotLight.outerCutOff, 0.0001);
        float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
        float attenuation = intensity /
            (spotLight.constant + spotLight.linear * distanceToLight
             + spotLight.quadratic * distanceToLight * distanceToLight);
        addPhongTerms(spotLight.ambient, spotLight.diffuse, spotLight.specular,
                      lightDirection, normal, viewDirection, attenuation,
                      lighting, specular);
    }

    TexCoord = aTexCoord * uvScale;
    CoatPosition = aPosition * 2.0;
    FragPos = worldPosition.xyz;
    VertexLighting = lighting;
    VertexSpecular = specular;
    gl_Position = projection * view * worldPosition;
}
