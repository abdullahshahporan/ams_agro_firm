#version 330 core

#define NR_POINT_LIGHTS 6
#define FAR_DISTANCE 10000.0
#define EPSILON 0.002

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

struct Hit
{
    float t;
    vec3 position;
    vec3 normal;
    vec3 color;
    float shininess;
    float reflectivity;
    vec3 emission;
};

in vec2 ScreenPosition;
out vec4 fragmentColor;

uniform mat4 inverseViewProjection;
uniform vec3 viewPos;
uniform float fogStart;
uniform float fogEnd;
uniform bool nightMode;
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
uniform bool workerVisible;
uniform bool workerCarryingHay;
uniform vec3 workerPosition;
uniform float workerFeedProgress;
uniform int deliveredHayLoads;

void setMiss(out Hit hit)
{
    hit.t = FAR_DISTANCE;
    hit.position = vec3(0.0);
    hit.normal = vec3(0.0, 1.0, 0.0);
    hit.color = vec3(0.0);
    hit.shininess = 1.0;
    hit.reflectivity = 0.0;
    hit.emission = vec3(0.0);
}

bool sphereHit(vec3 rayOrigin, vec3 rayDirection, vec3 centre, float radius,
               out float t, out vec3 normal)
{
    vec3 offset = rayOrigin - centre;
    float halfB = dot(offset, rayDirection);
    float c = dot(offset, offset) - radius * radius;
    float discriminant = halfB * halfB - c;
    if (discriminant < 0.0)
        return false;
    float root = sqrt(discriminant);
    t = -halfB - root;
    if (t <= EPSILON)
        t = -halfB + root;
    if (t <= EPSILON)
        return false;
    normal = normalize(rayOrigin + t * rayDirection - centre);
    return true;
}

bool boxHit(vec3 rayOrigin, vec3 rayDirection, vec3 minimumPoint,
            vec3 maximumPoint, out float t, out vec3 normal)
{
    vec3 safeDirection = rayDirection;
    safeDirection.x = abs(safeDirection.x) < 0.000001
        ? (safeDirection.x < 0.0 ? -0.000001 : 0.000001) : safeDirection.x;
    safeDirection.y = abs(safeDirection.y) < 0.000001
        ? (safeDirection.y < 0.0 ? -0.000001 : 0.000001) : safeDirection.y;
    safeDirection.z = abs(safeDirection.z) < 0.000001
        ? (safeDirection.z < 0.0 ? -0.000001 : 0.000001) : safeDirection.z;
    vec3 inverseDirection = 1.0 / safeDirection;
    vec3 t0 = (minimumPoint - rayOrigin) * inverseDirection;
    vec3 t1 = (maximumPoint - rayOrigin) * inverseDirection;
    vec3 nearValues = min(t0, t1);
    vec3 farValues = max(t0, t1);
    float nearT = max(max(nearValues.x, nearValues.y), nearValues.z);
    float farT = min(min(farValues.x, farValues.y), farValues.z);
    if (farT < max(nearT, EPSILON))
        return false;
    t = nearT > EPSILON ? nearT : farT;
    vec3 point = rayOrigin + t * rayDirection;
    vec3 centre = (minimumPoint + maximumPoint) * 0.5;
    vec3 halfSize = (maximumPoint - minimumPoint) * 0.5;
    vec3 local = (point - centre) / halfSize;
    vec3 absoluteLocal = abs(local);
    if (absoluteLocal.x > absoluteLocal.y && absoluteLocal.x > absoluteLocal.z)
        normal = vec3(sign(local.x), 0.0, 0.0);
    else if (absoluteLocal.y > absoluteLocal.z)
        normal = vec3(0.0, sign(local.y), 0.0);
    else
        normal = vec3(0.0, 0.0, sign(local.z));
    return true;
}

void acceptHit(inout Hit closest, float t, vec3 rayOrigin, vec3 rayDirection,
               vec3 normal, vec3 color, float shininess, float reflectivity)
{
    if (t > EPSILON && t < closest.t)
    {
        closest.t = t;
        closest.position = rayOrigin + t * rayDirection;
        closest.normal = normal;
        closest.color = color;
        closest.shininess = shininess;
        closest.reflectivity = reflectivity;
        closest.emission = vec3(0.0);
    }
}

void testBox(inout Hit hit, vec3 rayOrigin, vec3 rayDirection,
             vec3 centre, vec3 halfSize, vec3 color,
             float shininess, float reflectivity)
{
    float t;
    vec3 normal;
    if (boxHit(rayOrigin, rayDirection, centre - halfSize, centre + halfSize,
               t, normal))
        acceptHit(hit, t, rayOrigin, rayDirection, normal, color,
                  shininess, reflectivity);
}

void testSphere(inout Hit hit, vec3 rayOrigin, vec3 rayDirection,
                vec3 centre, float radius, vec3 color,
                float shininess, float reflectivity)
{
    float t;
    vec3 normal;
    if (sphereHit(rayOrigin, rayDirection, centre, radius, t, normal))
        acceptHit(hit, t, rayOrigin, rayDirection, normal, color,
                  shininess, reflectivity);
}

bool traceScene(vec3 rayOrigin, vec3 rayDirection, bool includeLightFixture,
                out Hit hit)
{
    setMiss(hit);

    if (rayDirection.y < -0.00001)
    {
        float groundT = (-0.15 - rayOrigin.y) / rayDirection.y;
        if (groundT > EPSILON)
        {
            vec3 point = rayOrigin + groundT * rayDirection;
            vec3 color = vec3(0.19, 0.43, 0.10);
            if (abs(point.x) < 3.5 || (point.z > 6.0 && point.z < 12.0))
                color = vec3(0.12, 0.13, 0.14);
            else if (abs(point.x) > 23.5 && abs(point.x) < 67.0 &&
                     point.z > -75.0 && point.z < 60.0)
            {
                float cropRow = mod(floor(abs(point.x) / 2.25), 2.0);
                color = mix(vec3(0.40, 0.32, 0.19),
                            vec3(0.20, 0.43, 0.10), cropRow);
            }
            float grid = mod(floor(point.x) + floor(point.z), 2.0);
            color *= mix(0.92, 1.05, grid);
            acceptHit(hit, groundT, rayOrigin, rayDirection,
                      vec3(0.0, 1.0, 0.0), color, 8.0, 0.03);
        }
    }

    // Main farm buildings, placed at the same main coordinates as the raster scene.
    testBox(hit, rayOrigin, rayDirection, vec3(-10.0, 1.65, -8.0),
            vec3(4.25, 1.65, 3.62), vec3(0.46, 0.18, 0.10), 16.0, 0.04);
    testBox(hit, rayOrigin, rayDirection, vec3(11.0, 1.85, -11.0),
            vec3(3.40, 1.85, 2.90), vec3(0.50, 0.20, 0.10), 18.0, 0.05);
    testBox(hit, rayOrigin, rayDirection, vec3(10.0, 1.35, 7.3),
            vec3(3.00, 1.35, 2.30), vec3(0.76, 0.55, 0.22), 12.0, 0.03);
    testBox(hit, rayOrigin, rayDirection, vec3(8.0, 1.70, 12.0),
            vec3(2.55, 1.70, 2.00), vec3(0.70, 0.34, 0.16), 14.0, 0.04);

    // Entrance pillars and banner.
    testBox(hit, rayOrigin, rayDirection, vec3(-4.3, 2.25, 15.5),
            vec3(0.55, 2.25, 0.55), vec3(0.46, 0.16, 0.08), 12.0, 0.03);
    testBox(hit, rayOrigin, rayDirection, vec3(4.3, 2.25, 15.5),
            vec3(0.55, 2.25, 0.55), vec3(0.46, 0.16, 0.08), 12.0, 0.03);
    testBox(hit, rayOrigin, rayDirection, vec3(0.0, 4.72, 15.5),
            vec3(4.85, 0.55, 0.42), vec3(0.30, 0.10, 0.04), 18.0, 0.05);

    // The metal water tank is deliberately reflective to demonstrate the
    // secondary ray bounce in a useful object already present in the farm.
    testSphere(hit, rayOrigin, rayDirection, vec3(13.0, 3.85, 0.5),
               1.32, vec3(0.25, 0.48, 0.61), 96.0, 0.68);
    testBox(hit, rayOrigin, rayDirection, vec3(13.0, 0.45, 0.5),
            vec3(1.65, 0.45, 1.65), vec3(0.40, 0.42, 0.43), 30.0, 0.12);

    if (workerVisible)
    {
        testBox(hit, rayOrigin, rayDirection,
                workerPosition + vec3(0.0, 1.22, 0.0),
                vec3(0.30, 0.55, 0.21), vec3(0.12, 0.42, 0.72), 15.0, 0.02);
        testSphere(hit, rayOrigin, rayDirection,
                   workerPosition + vec3(0.0, 1.80, 0.0),
                   0.27, vec3(0.58, 0.34, 0.20), 12.0, 0.0);
        if (workerCarryingHay)
        {
            vec3 hayPosition = mix(workerPosition + vec3(0.0, 2.55, 0.0),
                                   vec3(-12.0, 0.72, -5.45),
                                   workerFeedProgress);
            testBox(hit, rayOrigin, rayDirection,
                    hayPosition,
                    vec3(0.47, 0.18, 0.36), vec3(0.79, 0.64, 0.28),
                    5.0, 0.0);
        }
    }
    if (deliveredHayLoads > 0)
        testBox(hit, rayOrigin, rayDirection, vec3(-12.0, 0.62, -5.45),
                vec3(1.1, 0.13, 0.34), vec3(0.69, 0.55, 0.20), 5.0, 0.0);

    // Trees: trunks plus round canopies.
    testBox(hit, rayOrigin, rayDirection, vec3(-15.0, 1.20, 11.0),
            vec3(0.28, 1.20, 0.28), vec3(0.30, 0.15, 0.05), 8.0, 0.0);
    testSphere(hit, rayOrigin, rayDirection, vec3(-15.0, 3.15, 11.0),
               1.55, vec3(0.10, 0.42, 0.08), 10.0, 0.02);
    testBox(hit, rayOrigin, rayDirection, vec3(15.0, 1.20, 10.5),
            vec3(0.28, 1.20, 0.28), vec3(0.30, 0.15, 0.05), 8.0, 0.0);
    testSphere(hit, rayOrigin, rayDirection, vec3(15.0, 3.15, 10.5),
               1.55, vec3(0.10, 0.42, 0.08), 10.0, 0.02);

    if (includeLightFixture)
    {
        float previousT = hit.t;
        testSphere(hit, rayOrigin, rayDirection, pointLights[5].position,
                   0.14, vec3(0.30, 0.75, 1.0), 32.0, 0.0);
        if (hit.t < previousT && length(pointLights[5].diffuse) > 0.0)
            hit.emission = vec3(0.30, 1.2, 2.0);
    }
    return hit.t < FAR_DISTANCE;
}

bool inShadow(vec3 origin, vec3 direction, float maximumDistance)
{
    Hit blocker;
    if (!traceScene(origin, direction, false, blocker))
        return false;
    return blocker.t < maximumDistance;
}

vec3 skyColor(vec3 direction)
{
    float horizon = clamp(direction.y * 0.5 + 0.5, 0.0, 1.0);
    if (nightMode)
        return mix(vec3(0.008, 0.012, 0.035), vec3(0.025, 0.055, 0.13), horizon);
    return mix(vec3(0.72, 0.82, 0.88), vec3(0.28, 0.62, 0.91), horizon);
}

void addLight(in Hit hit, vec3 viewDirection, vec3 lightDirection,
              vec3 diffuseColor, vec3 specularColor, float attenuation,
              inout vec3 result)
{
    float diffuseFactor = max(dot(hit.normal, lightDirection), 0.0);
    if (diffuseEnabled)
        result += hit.color * diffuseColor * diffuseFactor * attenuation;
    if (specularEnabled && diffuseFactor > 0.0)
    {
        vec3 reflected = reflect(-lightDirection, hit.normal);
        float specularFactor = pow(max(dot(viewDirection, reflected), 0.0),
                                   hit.shininess);
        result += specularColor * specularFactor * attenuation;
    }
}

vec3 shadeHit(in Hit hit, vec3 incomingDirection)
{
    vec3 viewDirection = normalize(-incomingDirection);
    vec3 result = hit.emission + (ambientEnabled ? sceneAmbient * hit.color : vec3(0.0));

    if (directionalLightEnabled)
    {
        vec3 direction = normalize(-dirLight.direction);
        float visibility = inShadow(hit.position + hit.normal * EPSILON * 3.0,
                                    direction, FAR_DISTANCE) ? 0.0 : 1.0;
        if (ambientEnabled)
            result += dirLight.ambient * hit.color;
        addLight(hit, viewDirection, direction, dirLight.diffuse,
                 dirLight.specular, visibility, result);
    }

    if (pointLightsEnabled)
    {
        for (int index = 0; index < NR_POINT_LIGHTS; ++index)
        {
            vec3 toLight = pointLights[index].position - hit.position;
            float distanceToLight = length(toLight);
            vec3 direction = toLight / max(distanceToLight, 0.0001);
            if (!inShadow(hit.position + hit.normal * EPSILON * 3.0,
                          direction, distanceToLight))
            {
                float attenuation = 1.0 /
                    (pointLights[index].constant
                     + pointLights[index].linear * distanceToLight
                     + pointLights[index].quadratic * distanceToLight * distanceToLight);
                if (ambientEnabled)
                    result += pointLights[index].ambient * hit.color * attenuation;
                addLight(hit, viewDirection, direction, pointLights[index].diffuse,
                         pointLights[index].specular, attenuation, result);
            }
        }
    }

    if (spotLightEnabled)
    {
        vec3 toLight = spotLight.position - hit.position;
        float distanceToLight = length(toLight);
        vec3 direction = toLight / max(distanceToLight, 0.0001);
        float theta = dot(direction, normalize(-spotLight.direction));
        float cone = clamp((theta - spotLight.outerCutOff)
                           / max(spotLight.cutOff - spotLight.outerCutOff, 0.0001),
                           0.0, 1.0);
        if (cone > 0.0 && !inShadow(hit.position + hit.normal * EPSILON * 3.0,
                                   direction, distanceToLight))
        {
            float attenuation = cone /
                (spotLight.constant + spotLight.linear * distanceToLight
                 + spotLight.quadratic * distanceToLight * distanceToLight);
            addLight(hit, viewDirection, direction, spotLight.diffuse,
                     spotLight.specular, attenuation, result);
        }
    }

    return result;
}

void main()
{
    vec4 nearPoint = inverseViewProjection * vec4(ScreenPosition, -1.0, 1.0);
    vec4 farPoint = inverseViewProjection * vec4(ScreenPosition, 1.0, 1.0);
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;
    vec3 rayDirection = normalize(farPoint.xyz - nearPoint.xyz);

    Hit primaryHit;
    vec3 color;
    if (traceScene(viewPos, rayDirection, true, primaryHit))
    {
        color = shadeHit(primaryHit, rayDirection);
        if (primaryHit.reflectivity > 0.0)
        {
            vec3 reflectedDirection = reflect(rayDirection, primaryHit.normal);
            vec3 reflectedOrigin = primaryHit.position
                                   + primaryHit.normal * EPSILON * 3.0;
            Hit reflectedHit;
            vec3 reflectedColor = skyColor(reflectedDirection);
            if (traceScene(reflectedOrigin, reflectedDirection, true, reflectedHit))
                reflectedColor = shadeHit(reflectedHit, reflectedDirection);
            color = mix(color, reflectedColor, primaryHit.reflectivity);
        }
        color = mix(color, skyColor(rayDirection),
                    smoothstep(fogStart, fogEnd, primaryHit.t));
    }
    else
    {
        color = skyColor(rayDirection);
    }

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));
    fragmentColor = vec4(color, 1.0);
}
