#version 330 core
in vec2 ScreenPosition;
out vec4 fragmentColor;
uniform sampler2D sceneColor;
uniform sampler2D sceneDepth;
uniform sampler2D reflectedColor;
uniform mat4 inverseViewProjection;
uniform mat4 viewProjection;
uniform mat4 reflectionViewProjection;
uniform mat4 view;
uniform vec3 cameraPosition;
uniform float time;
uniform float wetness;

vec3 worldAt(vec2 uv,float depth)
{
    vec4 world=inverseViewProjection*vec4(uv*2.0-1.0,depth*2.0-1.0,1.0);
    return world.xyz/world.w;
}
bool traceReflection(vec3 origin,vec3 direction,out vec3 reflected)
{
    // March a real reflected ray against the current camera depth image.
    // The planar capture supplies off-screen information if no hit is visible.
    for(int step=0;step<48;++step)
    {
        float distance=.12+float(step)*.18+float(step*step)*.018;
        vec3 sampleWorld=origin+direction*distance;
        vec4 clip=viewProjection*vec4(sampleWorld,1);
        if(clip.w<=0.0)return false;
        vec2 uv=clip.xy/clip.w*.5+.5;
        if(any(lessThan(uv,vec2(.003)))||any(greaterThan(uv,vec2(.997))))return false;
        float depth=texture(sceneDepth,uv).r;
        if(depth>=.99999)continue;
        vec3 surface=worldAt(uv,depth);
        float rayDepth=-(view*vec4(sampleWorld,1)).z;
        float surfaceDepth=-(view*vec4(surface,1)).z;
        float gap=rayDepth-surfaceDepth;
        if(surface.y>.12&&gap>.005&&gap<.14+distance*.025)
        {
            reflected=texture(sceneColor,uv).rgb;
            return true;
        }
    }
    return false;
}
void main()
{
    vec2 uv=ScreenPosition*.5+.5;
    vec3 original=texture(sceneColor,uv).rgb;
    float depth=texture(sceneDepth,uv).r;
    if(depth>=.99999||wetness<=0.0){fragmentColor=vec4(original,1);return;}
    vec3 world=worldAt(uv,depth);
    bool exterior=abs(world.x)<3.98&&world.z>16.0&&world.z<128.0;
    bool interior=abs(world.x)<2.39&&world.z>-1.9&&world.z<16.0;
    // Restrict the effect to the actual road surface, preserving the colours
    // and lighting of every building, animal, tree and UI element unchanged.
    vec3 surfaceNormal=normalize(cross(dFdx(world),dFdy(world)));
    if((!exterior&&!interior)||world.y<-.11||world.y>.095||abs(surfaceNormal.y)<.90)
    {fragmentColor=vec4(original,1);return;}
    vec2 ripple=vec2(sin(world.z*11.0+time*3.0),cos(world.x*15.0-time*2.4))*.006;
    vec3 normal=normalize(vec3(ripple.x,1,ripple.y));
    vec3 incoming=normalize(world-cameraPosition);
    vec3 direction=reflect(incoming,normal);
    vec4 planar=reflectionViewProjection*vec4(world,1);
    vec2 reflectedUv=planar.xy/planar.w*.5+.5+ripple*.17;
    vec3 reflection=texture(reflectedColor,clamp(reflectedUv,vec2(.002),vec2(.998))).rgb;
    vec3 screenHit;
    if(traceReflection(world+normal*.06,direction,screenHit))reflection=mix(reflection,screenHit,.55);
    float fresnel=pow(1.0-max(dot(-incoming,normal),0.0),4.0);
    float puddle=.80+.20*sin(world.x*1.8+sin(world.z*.9));
    float strength=wetness*(.48+.30*fresnel)*puddle;
    // No additional tone mapping: keep precisely the existing raster colours.
    vec3 result=mix(original*(1.0-wetness*.12),reflection,strength);
    fragmentColor=vec4(result,1);
}
