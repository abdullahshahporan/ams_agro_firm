#include "lighting_system.h"
#include "shared/rain_geometry.h"
#include "rain_renderer.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdexcept>
#include <limits>

using namespace std;
void require(bool ok,const char* message){if(!ok)throw runtime_error(message);}
glm::vec3 uniform(const Shader& shader,const char* name){
    glm::vec3 value;const GLint location=glGetUniformLocation(shader.ID,name);
    require(location>=0,"Missing lighting uniform");glGetUniformfv(shader.ID,location,&value.x);return value;
}
glm::vec3 groundLight(const Shader& shader){
    const float diffuse=max(-uniform(shader,"dirLight.direction").y,0.0f);
    return uniform(shader,"sceneAmbient")+uniform(shader,"dirLight.ambient")+uniform(shader,"dirLight.diffuse")*diffuse;
}
int main(){
    if(!glfwInit())return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
    GLFWwindow* window=glfwCreateWindow(32,32,"Lighting/weather regression",nullptr,nullptr);
    if(!window)return 2;glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))return 3;
    int status=0;
    try{
        Shader shader("shaders/texture.vs","shaders/texture.fs");shader.use();
        LightingSystem day;day.setupShader(shader,{0,4,20});const auto noon=groundLight(shader);
        LightingSystem afternoon;afternoon.setAfternoon(true);afternoon.setupShader(shader,{0,4,20});
        const auto evening=groundLight(shader);
        for(int c=0;c<3;++c)require(evening[c]/noon[c]>.75f&&evening[c]/noon[c]<.95f,"Afternoon fill should remain close to daylight");
        require(evening.r/noon.r>evening.g/noon.g,"Afternoon warmth lost");
        const auto sky=afternoon.clearColor();
        afternoon.setDuskProgress(0);afternoon.setupShader(shader,{0,4,20});
        require(glm::length(groundLight(shader)-evening)<1e-5f,"Dusk start jumps in brightness");
        afternoon.setDuskProgress(.5f);
        require(glm::length(afternoon.clearColor()-(sky+glm::vec3(.02f,.035f,.09f))*.5f)<1e-5f,"Dusk sky midpoint incorrect");
        afternoon.setDuskProgress(1);afternoon.setupShader(shader,{0,4,20});const auto duskEnd=groundLight(shader);
        LightingSystem night;night.toggleDayNight();night.setupShader(shader,{0,4,20});
        require(glm::length(groundLight(shader)-duskEnd)<1e-5f,"Dusk end jumps in brightness");

        vector<float> rain,replay,half,invalid;
        RainGeometry::append(rain,33,1200,800);RainGeometry::append(replay,33,1200,800);
        RainGeometry::append(half,33,600,400);
        RainGeometry::append(invalid,numeric_limits<double>::infinity(),1200,800);
        require(rain.size()==110*6*6&&rain==replay&&invalid.empty(),"Rain count/replay/input guard failed");
        require(abs((rain[6]-rain[0])-1.6f)<1e-4f&&abs((half[6]-half[0])-.8f)<1e-4f,"Rain width scaling failed");
        require(rain[5]==.62f&&abs((rain[13]-rain[1])-18.0f)<1e-4f,"Rain visibility settings incorrect");

        const glm::vec3 worker(4.6f,0,7.2f);
        for(float progress:{0.0f,.5f,1.0f}){
            const auto eye=worker+glm::mix(glm::vec3(-3.4f,2.8f,-4.2f),glm::vec3(-3,2.7f,-3.8f),progress);
            const auto view=glm::lookAt(eye,worker+glm::vec3(0,1.85f,0),glm::vec3(0,1,0));
            const auto projection=glm::perspective(glm::radians(46.f),1.5f,.1f,1800.f);
            for(const auto point:{glm::vec3(0,0,0),glm::vec3(0,3.24f,0),glm::vec3(-1.1f,2.76f,0),glm::vec3(1.1f,2.76f,0),glm::vec3(0,2.76f,-1.1f),glm::vec3(0,2.76f,1.1f)}){
                const auto clip=projection*view*glm::vec4(worker+point,1);const glm::vec3 ndc=glm::vec3(clip)/clip.w;
                require(abs(ndc.x)<.95f&&ndc.y>-.98f&&ndc.y<.70f,"Umbrella/boots overlap frame edge or upper HUD");
            }
        }
        RainRenderer worldRain;
        const auto rainView=glm::lookAt(glm::vec3(0,3,25),glm::vec3(0,0,18),glm::vec3(0,1,0));
        const auto rainProjection=glm::perspective(glm::radians(50.f),1.5f,.1f,1800.f);
        for(double time:{0.0,33.0,33.12,33.4}){
            worldRain.render(rainView,rainProjection,{0,3,25},time,worker,true);
            GLboolean depthWrite=GL_FALSE;glGetBooleanv(GL_DEPTH_WRITEMASK,&depthWrite);
            require(depthWrite&&glIsEnabled(GL_DEPTH_TEST)&&!glIsEnabled(GL_BLEND),"World rain leaked GL state");
        }
        require(glGetError()==GL_NO_ERROR,"Lighting/weather OpenGL error");
        cout<<"PASS: brighter warm afternoon, continuous dusk, deterministic visible rain, full umbrella/boots framing.\n";
        cout<<"Afternoon/day ground-light ratios: "<<evening.r/noon.r<<", "<<evening.g/noon.g<<", "<<evening.b/noon.b<<'\n';
    }catch(const exception& error){cerr<<error.what()<<'\n';status=4;}
    glfwDestroyWindow(window);glfwTerminate();return status;
}
