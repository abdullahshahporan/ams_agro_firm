#version 330 core

#define NR_POINT_LIGHTS 5
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
uniform float weatherWetness;

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

// Small sign glyphs are evaluated on the banner surface so the entrance
// remains identifiable in the full-screen road/reflection shot.
int signRow(int letter,int row)
{
    ivec4 top;ivec3 bottom;
    if(letter==0){top=ivec4(14,17,17,31);bottom=ivec3(17,17,17);} // A
    else if(letter==1){top=ivec4(17,27,21,21);bottom=ivec3(17,17,17);} // M
    else if(letter==2){top=ivec4(15,16,16,14);bottom=ivec3(1,1,30);} // S
    else if(letter==3){top=ivec4(14,17,16,23);bottom=ivec3(17,17,15);} // G
    else if(letter==4){top=ivec4(30,17,17,30);bottom=ivec3(20,18,17);} // R
    else if(letter==5){top=ivec4(14,17,17,17);bottom=ivec3(17,17,14);} // O
    else if(letter==6){top=ivec4(31,16,16,30);bottom=ivec3(16,16,16);} // F
    else return 0;
    return row<4?top[row]:bottom[row-4];
}

bool traceScene(vec3 rayOrigin, vec3 rayDirection,
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
            bool exteriorRoad=abs(point.x)<4.0&&point.z>16.0;
            bool farmRoad=abs(point.x)<2.4&&point.z>-2.0&&point.z<=16.0;
            if (exteriorRoad||farmRoad)
            {
                float grain=sin(point.x*83.0+point.z*61.0)*sin(point.z*97.0)*.035;
                color=vec3(.28,.22,.16)+grain;
            }
            else if (abs(point.x) > 23.5 && abs(point.x) < 67.0 &&
                     point.z > -75.0 && point.z < 60.0)
            {
                float cropRow = mod(floor(abs(point.x) / 2.25), 2.0);
                color = mix(vec3(0.40, 0.32, 0.19),
                            vec3(0.20, 0.43, 0.10), cropRow);
            }
            color=mix(color,color*vec3(0.54,0.62,0.69),weatherWetness*0.52);
            float wetReflection=(exteriorRoad||farmRoad)?0.72:0.12;
            vec3 groundNormal=normalize(vec3(weatherWetness*.006*sin(point.z*9.0),1.0,
                                             weatherWetness*.006*sin(point.x*13.0)));
            acceptHit(hit, groundT, rayOrigin, rayDirection,
                      groundNormal, color,
                      mix(8.0,96.0,weatherWetness),mix(0.03,wetReflection,weatherWetness));
        }
    }

    // The cow shed is open-sided just like the raster model: foundation,
    // roof, posts and a brick service wall instead of one solid block.
    testBox(hit,rayOrigin,rayDirection,vec3(-10.0,0.10,-8.0),
            vec3(4.10,0.10,3.50),vec3(0.42),24.0,0.10);
    testBox(hit,rayOrigin,rayDirection,vec3(-10.0,3.62,-8.0),
            vec3(4.35,0.18,3.82),vec3(0.22,0.25,0.27),72.0,0.25);
    for(int sideX=-1;sideX<=1;sideX+=2)
        for(int sideZ=-1;sideZ<=1;sideZ+=2)
            testBox(hit,rayOrigin,rayDirection,
                    vec3(-10.0+sideX*3.75,1.85,-8.0+sideZ*3.05),
                    vec3(0.15,1.85,0.15),vec3(0.075,0.080,0.078),48.0,0.12);
    testBox(hit,rayOrigin,rayDirection,vec3(-13.78,1.0,-8.0),
            vec3(0.16,0.90,2.80),vec3(0.46,0.18,0.10),16.0,0.04);
    // Two concrete drinking/feed troughs make the cattle area readable.
    testBox(hit,rayOrigin,rayDirection,vec3(-11.8,0.48,-4.65),
            vec3(1.85,0.48,0.55),vec3(0.48,0.49,0.46),20.0,0.08);
    testBox(hit,rayOrigin,rayDirection,vec3(-7.6,0.48,-4.65),
            vec3(1.85,0.48,0.55),vec3(0.48,0.49,0.46),20.0,0.08);

    // Main farm buildings, placed at the same coordinates as the raster scene.
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

    // Closed entrance leaves and lit front fence match the end-of-day scene.
    for(int side=-1;side<=1;side+=2)
    {
        for(int rail=0;rail<3;++rail)
            testBox(hit,rayOrigin,rayDirection,vec3(side*1.91,.35+float(rail)*.85,15.47),
                    vec3(1.90,.055,.065),vec3(.045,.055,.058),72.0,.25);
        for(int bar=0;bar<7;++bar)
            testBox(hit,rayOrigin,rayDirection,vec3(side*(.15+float(bar)*.59),1.22,15.47),
                    vec3(.035,1.15,.055),vec3(.045,.055,.058),72.0,.25);
        for(int rail=0;rail<2;++rail)
            testBox(hit,rayOrigin,rayDirection,vec3(side*11.2,.65+float(rail)*.60,15.5),
                    vec3(6.3,.055,.07),vec3(.35,.20,.075),16.0,.05);
        for(int post=0;post<5;++post)
        {
            vec3 centre=vec3(side*(5.8+float(post)*2.8),1.0,15.5);
            testBox(hit,rayOrigin,rayDirection,centre,vec3(.08,1.0,.08),vec3(.32,.18,.07),12.0,.03);
            float oldT=hit.t;
            testSphere(hit,rayOrigin,rayDirection,centre+vec3(0,1.05,0),.14,
                       vec3(1.0,.78,.34),64.0,.0);
            if(hit.t<oldT&&nightMode)hit.emission=vec3(3.8,2.6,1.0);
        }
    }
    for(int light=0;light<NR_POINT_LIGHTS;++light)
    {
        float oldT=hit.t;
        testSphere(hit,rayOrigin,rayDirection,pointLights[light].position,.16,
                   vec3(1.0,.77,.32),64.0,0.0);
        if(hit.t<oldT)hit.emission=pointLights[light].diffuse*3.0;
    }

    // The metal water tank keeps the same rectangular proportions as the
    // raster scene and is reflective enough to demonstrate a secondary ray.
    testBox(hit, rayOrigin, rayDirection, vec3(13.0, 3.85, 0.5),
            vec3(2.0, 1.25, 1.70), vec3(0.25, 0.48, 0.61), 96.0, 0.62);
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

    // Procedural brick and banner wood remove the featureless-block look
    // without requiring the raster meshes to be duplicated in the ray pass.
    if(hit.t<FAR_DISTANCE)
    {
        vec3 p=hit.position;
        bool pillar=abs(abs(p.x)-4.3)<.58&&abs(p.z-15.5)<.58&&p.y<4.51;
        if(pillar)
        {
            float row=floor(p.y/.23);
            float brick=fract((abs(hit.normal.x)>.5?p.z:p.x)/.47+mod(row,2.0)*.5);
            bool mortar=fract(p.y/.23)<.08||brick<.045;
            hit.color=mortar?vec3(.36,.30,.24):vec3(.48,.18,.08)*(1.0+.07*sin(row*13.0));
        }
        if(abs(p.z-15.92)<.015&&abs(p.x)<4.85&&p.y>4.17&&p.y<5.27)
        {
            hit.color=vec3(.20,.085,.026)*(1.0+.15*sin(p.y*110.0+p.x*1.5));
            vec2 pixel=vec2((p.x+3.9)/.10,(5.07-p.y)/.10);
            int cell=int(floor(pixel.x/6.0));int column=int(mod(floor(pixel.x),6.0));
            int row=int(floor(pixel.y));
            const int letters[13]=int[13](0,1,2,7,0,3,4,5,7,6,0,4,1);
            if(cell>=0&&cell<13&&column<5&&row>=0&&row<7)
                if((signRow(letters[cell],row)&(1<<(4-column)))!=0)
                {
                    hit.color=vec3(.98,.74,.22);
                    hit.emission=nightMode?vec3(.48,.29,.055):vec3(0);
                }
        }
    }
    return hit.t < FAR_DISTANCE;
}

bool inShadow(vec3 origin, vec3 direction, float maximumDistance)
{
    Hit blocker;
    if (!traceScene(origin, direction, blocker))
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
    if (traceScene(viewPos, rayDirection, primaryHit))
    {
        color = shadeHit(primaryHit, rayDirection);
        if (primaryHit.reflectivity > 0.0)
        {
            vec3 reflectedDirection = reflect(rayDirection, primaryHit.normal);
            vec3 reflectedOrigin = primaryHit.position
                                   + primaryHit.normal * EPSILON * 3.0;
            Hit reflectedHit;
            vec3 reflectedColor = skyColor(reflectedDirection);
            if (traceScene(reflectedOrigin, reflectedDirection, reflectedHit))
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
