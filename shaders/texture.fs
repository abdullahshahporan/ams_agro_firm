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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D texture1;
uniform vec3 objectTint;
uniform bool useTexture;
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

vec3 phongTerms(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor,
                vec3 lightDirection, vec3 normal, vec3 viewDirection,
                vec3 albedo)
{
    float diffuseFactor = max(dot(normal, lightDirection), 0.0);
    vec3 reflected = reflect(-lightDirection, normal);
    float specularFactor = pow(max(dot(viewDirection, reflected), 0.0),
                               material.shininess);
    vec3 ambient = ambientEnabled ? ambientColor * albedo : vec3(0.0);
    vec3 diffuse = diffuseEnabled ? diffuseColor * diffuseFactor * albedo : vec3(0.0);
    vec3 specular = specularEnabled
        ? specularColor * specularFactor * material.specularColor
        : vec3(0.0);
    return ambient + diffuse + specular;
}

vec3 calculateDirectional(vec3 normal, vec3 viewDirection, vec3 albedo)
{
    vec3 lightDirection = normalize(-dirLight.direction);
    return phongTerms(dirLight.ambient, dirLight.diffuse, dirLight.specular,
                      lightDirection, normal, viewDirection, albedo);
}

vec3 calculatePoint(PointLight light, vec3 normal, vec3 viewDirection, vec3 albedo)
{
    vec3 lightDirection = normalize(light.position - FragPos);
    float distanceToLight = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight
                         + light.quadratic * distanceToLight * distanceToLight);
    return phongTerms(light.ambient, light.diffuse, light.specular,
                      lightDirection, normal, viewDirection, albedo) * attenuation;
}

vec3 calculateSpot(vec3 normal, vec3 viewDirection, vec3 albedo)
{
    vec3 lightDirection = normalize(spotLight.position - FragPos);
    float theta = dot(lightDirection, normalize(-spotLight.direction));
    float epsilon = max(spotLight.cutOff - spotLight.outerCutOff, 0.0001);
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    float distanceToLight = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distanceToLight
                         + spotLight.quadratic * distanceToLight * distanceToLight);
    return phongTerms(spotLight.ambient, spotLight.diffuse, spotLight.specular,
                      lightDirection, normal, viewDirection, albedo)
           * attenuation * intensity;
}

void main()
{
    vec4 sampled = useTexture ? texture(texture1, TexCoord) : vec4(1.0);
    vec3 albedo = sampled.rgb * objectTint;
    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // A small environment term keeps the night readable without making it
    // look sunlit. It follows the global ambient-component switch.
    if (ambientEnabled)
        result += sceneAmbient * albedo;

    if (directionalLightEnabled)
        result += calculateDirectional(normal, viewDirection, albedo);
    if (pointLightsEnabled)
        for (int index = 0; index < NR_POINT_LIGHTS; ++index)
            result += calculatePoint(pointLights[index], normal, viewDirection, albedo);
    if (spotLightEnabled)
        result += calculateSpot(normal, viewDirection, albedo);

    result += material.emission * albedo;
    fragmentColor = vec4(result, sampled.a);
}
