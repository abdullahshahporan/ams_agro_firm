// Guided simulation of the real farm, not a separate model of the project.
// All chapter timing, camera choreography and demonstration commands live here.
#include "simulation.h"
#include "farm_scene.h"
#include "entity_renderer.h"
#include "curved_renderer.h"
#include "ray_trace_renderer.h"
#include "lighting_system.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace
{
constexpr float Tick = 1.0f / 60.0f;
struct Chapter { const char* name; const char* caption; double seconds; };
const array<Chapter, 12> Chapters = {{
    {"THE FARM AND ITS COUNTRYSIDE", "A FULL LANDSCAPE WITH FIELDS, TREES AND A ROUNDED DISTANT HORIZON", 12},
    {"ENTRANCE, SIGNS AND OPENING GATES", "THE TWO LEAVES ROTATE AROUND THEIR HINGES TO OPEN THE REAL ENTRANCE", 10},
    {"CATTLE SHED, COWS AND ROTATING FANS", "SCULPTED CATTLE, FEED TROUGHS, TETHERS, HINGED STALLS AND FAN MOTION", 12},
    {"CALVES, CHICKENS AND DUCKS", "CALVES LEAVE THROUGH THEIR SIDE DOOR; POULTRY FOLLOW ELLIPTICAL ROUTES", 14},
    {"BARN, WATER TOWER AND FARMER HOUSE", "A CLOSE LOOK AT STORAGE, FARM EQUIPMENT AND THE FARMER'S ROOM", 12},
    {"CURVES AND SURFACES", "BEZIER MILK CAN, B-SPLINE PIPE AND THE RULED CATTLE-SHED ROOF", 12},
    {"THE FARMER'S COMPLETE HAY DELIVERY", "LEAVE THE ROOM, LIFT HAY, CARRY IT TO THE COW, PLACE IT AND RETURN HOME", 40},
    {"GOURAUD SHADING", "THE SAME REAL SCENE NOW USES PER-VERTEX LIGHTING INSTEAD OF PHONG SHADING", 8},
    {"NIGHT AND MOVING LIGHTS", "POINT LIGHTS, EMISSIVE LAMPS, THE MOVING DRONE AND A SWEEPING SPOTLIGHT", 14},
    {"RAY-TRACED REFLECTIONS AND SHADOWS", "SEPARATE ANALYTIC PREVIEW: SIMPLIFIED OBJECTS, SHADOW RAYS AND ONE BOUNCE", 10},
    {"FOUR VIEWS OF ONE PROJECT", "PERSPECTIVE, TOP, FRONT AND SIDE VIEWS SHOW THE SAME ANIMATED SCENE", 10},
    {"THE COMPLETE AMS AGRO FARM", "FINAL OVERVIEW - LIVE FARM ANIMATIONS CONTINUE; PRESS R TO REPLAY", 10}
}};

double totalSeconds()
{
    double total = 0;
    for (const Chapter& chapter : Chapters) total += chapter.seconds;
    return total;
}
float eased(float value) { return glm::smoothstep(0.0f, 1.0f, glm::clamp(value, 0.0f, 1.0f)); }
float approach(float current, float target, float step)
{
    return current < target ? min(current + step, target) : max(current - step, target);
}
struct Pose { glm::vec3 eye, target; float fov{48.0f}; };
Pose mixPose(const Pose& a, const Pose& b, float t)
{
    const float u = eased(t);
    return {glm::mix(a.eye, b.eye, u), glm::mix(a.target, b.target, u), a.fov + (b.fov-a.fov)*u};
}

class Tour
{
public:
    AnimationSystem animals;
    LightingSystem lighting;
    size_t chapter{0};
    double time{0}, elapsed{0};
    float gate{0}, stall{0}, calfDoor{0};
    bool complete{false};
    bool workerStarted{false};
    bool sawPickup{false}, sawCarry{false}, sawFeed{false}, workerReturned{false};

    Tour() { enter(); }
    void enter()
    {
        cout << "Simulation " << chapter+1 << '/' << Chapters.size() << ": " << Chapters[chapter].name << '\n';
        if (chapter == 3) animals.toggleCalfShed();
        if (chapter == 8)
        {
            if (!lighting.nightMode()) lighting.toggleDayNight();
            animals.setNightMode(true);
            if (!lighting.movingLightEnabled()) lighting.toggleMovingLight();
            if (!lighting.spotlightSweep()) lighting.toggleSpotlightSweep();
        }
        if (chapter == 9)
        {
            if (lighting.nightMode()) lighting.toggleDayNight();
            animals.setNightMode(false);
        }
        if (chapter == 11) animals.toggleCalfShed();
    }
    void update(float dt)
    {
        if (!complete)
        {
            elapsed += dt;
            time += dt;
            if (time >= Chapters[chapter].seconds)
            {
                if (chapter+1 < Chapters.size())
                {
                    time -= Chapters[chapter].seconds;
                    ++chapter;
                    enter();
                }
                else
                {
                    complete = true;
                    time = Chapters[chapter].seconds;
                    elapsed = totalSeconds();
                }
            }
        }
        // Let the camera reach the doorway before the farmer starts moving.
        // Guard this event so returning home cannot start a second delivery.
        if (chapter == 6 && !workerStarted && time >= 1.25)
        {
            animals.commandWorker();
            workerStarted = true;
        }
        animals.update(dt);
        lighting.update(dt);
        gate = approach(gate, chapter >= 1 ? 90.0f : 0.0f, 60.0f*dt);
        stall = approach(stall, chapter == 2 ? -88.0f : 0.0f, 60.0f*dt);
        calfDoor = approach(calfDoor, animals.calfShedDoorNeeded() ? 92.0f : 0.0f, 180.0f*dt);
        const Worker& worker = animals.workers()[0];
        sawPickup |= worker.state == WorkerState::PickingUpHay;
        sawCarry |= worker.state == WorkerState::WalkingToCows && worker.carryingHay;
        sawFeed |= worker.state == WorkerState::Feeding;
        workerReturned |= worker.deliveredLoads > 0 && worker.state == WorkerState::HomeIdle;
    }
    Pose pose() const
    {
        const float u = static_cast<float>(time/Chapters[chapter].seconds);
        switch (chapter)
        {
        case 0:
        {
            const float angle = 0.55f + u * 0.95f;
            return {{sin(angle)*72.0f, 45.0f+u*5.0f, cos(angle)*72.0f}, {0,0,-2}, 48};
        }
        case 1: return mixPose({{-7,6,28},{0,2.5f,15.5f},48}, {{8,5.3f,25},{0,2.5f,15.5f},48},u);
        case 2: return mixPose({{-10,2.6f,-1},{-10,1.35f,-7.4f},48}, {{-3,3.0f,-3.5f},{-10,1.35f,-7.4f},48},u);
        case 3:
            if (time < 7)
            {
                const glm::vec3 calves = (animals.calves()[0].position + animals.calves()[1].position)*0.5f;
                return {calves+glm::vec3(4,3.3f,5), calves+glm::vec3(0,0.7f,0),48};
            }
            return mixPose({{2,3.6f,13},{6,0.6f,7},48}, {{5.5f,3,11.8f},{9.3f,0.8f,7.1f},48},(static_cast<float>(time)-7)/7);
        case 4:
            if (time < 4) return {{4,6,-4},{11,1.8f,-11},48};
            if (time < 8) return {{18,7,6},{13,3,0.5f},48};
            return {{3,4.8f,16},{8,1.5f,12},48};
        case 5:
            if (time < 6) return mixPose({{5,2.8f,2},{8.3f,1,-1},44}, {{6.5f,3.7f,6},{10,1.5f,1.5f},46},static_cast<float>(time)/6);
            return mixPose({{-3,8,0},{-10,4,-8},48}, {{-15,9,-1},{-10,4,-8},48},(static_cast<float>(time)-6)/6);
        case 6:
        {
            const Worker& worker = animals.workers()[0];
            if (worker.deliveredLoads > 0 && worker.state == WorkerState::HomeIdle)
                return {{-6,3.2f,-1},{-12,0.85f,-5.45f},48};
            // Watch from the open front doorway, not through the house's back
            // wall or the poultry shelter while he is leaving his room.
            if (worker.position.x > 6.0f && worker.position.z > 9.5f)
                return {{12.2f,3.4f,8.8f},worker.position+glm::vec3(0,1.15f,0),50};
            return {worker.position+glm::vec3(3.3f,3.4f,5.4f), worker.position+glm::vec3(0,1.3f,0),48};
        }
        case 7: return mixPose({{-10,2.6f,-1},{-10,1.35f,-7.4f},48}, {{-3,3,-3.5f},{-10,1.35f,-7.4f},48},u);
        case 8: return mixPose({{14,10,25},{0,1,-2},48}, {{-14,8,22},{0,1,-3},48},u);
        case 9: return mixPose({{19,6,8},{11,2,0},48}, {{17,5,5.8f},{12,3,0.5f},48},u);
        case 10: return {{34,27,40},{0,1,-2},48};
        default: return mixPose({{-40,35,55},{0,0,-2},48}, {{42,48,65},{0,0,-2},48},u);
        }
    }
};

struct Controls { bool paused{false}, restart{false}; float speed{1}; };
void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;
    auto& controls = *static_cast<Controls*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,true);
    if (key == GLFW_KEY_SPACE) controls.paused = !controls.paused;
    if (key == GLFW_KEY_R) { controls.restart=true; controls.paused=false; }
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) controls.speed=min(4.0f,controls.speed*2);
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) controls.speed=max(0.25f,controls.speed*0.5f);
}

// Small code-native bitmap letters: no external font, image or UI dependency.
array<unsigned char,7> glyph(char ch)
{
    switch (ch)
    {
    case 'A':return {14,17,17,31,17,17,17};case 'B':return {30,17,17,30,17,17,30};
    case 'C':return {14,17,16,16,16,17,14};case 'D':return {30,17,17,17,17,17,30};
    case 'E':return {31,16,16,30,16,16,31};case 'F':return {31,16,16,30,16,16,16};
    case 'G':return {14,17,16,23,17,17,15};case 'H':return {17,17,17,31,17,17,17};
    case 'I':return {14,4,4,4,4,4,14};case 'J':return {7,2,2,2,18,18,12};
    case 'K':return {17,18,20,24,20,18,17};case 'L':return {16,16,16,16,16,16,31};
    case 'M':return {17,27,21,21,17,17,17};case 'N':return {17,25,21,19,17,17,17};
    case 'O':return {14,17,17,17,17,17,14};case 'P':return {30,17,17,30,16,16,16};
    case 'Q':return {14,17,17,17,21,18,13};case 'R':return {30,17,17,30,20,18,17};
    case 'S':return {15,16,16,14,1,1,30};case 'T':return {31,4,4,4,4,4,4};
    case 'U':return {17,17,17,17,17,17,14};case 'V':return {17,17,17,17,17,10,4};
    case 'W':return {17,17,17,21,21,21,10};case 'X':return {17,17,10,4,10,17,17};
    case 'Y':return {17,17,10,4,4,4,4};case 'Z':return {31,1,2,4,8,16,31};
    case '0':return {14,17,19,21,25,17,14};case '1':return {4,12,4,4,4,4,14};
    case '2':return {14,17,1,2,4,8,31};case '3':return {30,1,1,14,1,1,30};
    case '4':return {2,6,10,18,31,2,2};case '5':return {31,16,16,30,1,1,30};
    case '6':return {14,16,16,30,17,17,14};case '7':return {31,1,2,4,8,8,8};
    case '8':return {14,17,17,14,17,17,14};case '9':return {14,17,17,15,1,1,14};
    case '.':return {0,0,0,0,0,12,12};case ':':return {0,12,12,0,12,12,0};
    case '-':return {0,0,0,31,0,0,0};case '+':return {0,4,4,31,4,4,0};
    case '/':return {1,1,2,4,8,16,16};case '\'':return {4,4,0,0,0,0,0};
    default:return {};
    }
}

class Overlay
{
    Shader shader_{"shaders/simulation.vs","shaders/simulation.fs"};
    unsigned int vao_{0},vbo_{0};
    vector<float> vertices_;
    void rectangle(float x,float y,float w,float h,const glm::vec4& color)
    {
        const float points[]={x,y,x+w,y,x+w,y+h,x,y,x+w,y+h,x,y+h};
        for(int i=0;i<12;i+=2) vertices_.insert(vertices_.end(),{points[i],points[i+1],color.r,color.g,color.b,color.a});
    }
    void text(string value,float x,float y,float pixel,const glm::vec4& color)
    {
        for(unsigned char ch:value)
        {
            const auto rows=glyph(static_cast<char>(toupper(ch)));
            for(int row=0;row<7;++row)for(int column=0;column<5;++column)
                if(rows[row] & (1<<(4-column))) rectangle(x+column*pixel,y+row*pixel,pixel,pixel,color);
            x+=6*pixel;
        }
    }
public:
    Overlay()
    {
        glGenVertexArrays(1,&vao_);glGenBuffers(1,&vbo_);glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,6*sizeof(float),nullptr);glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,6*sizeof(float),reinterpret_cast<void*>(2*sizeof(float)));glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }
    ~Overlay(){glDeleteBuffers(1,&vbo_);glDeleteVertexArrays(1,&vao_);}
    void render(const Tour& tour,const Controls& controls,int width,int height)
    {
        vertices_.clear();
        const float s=min(width/1200.0f,height/800.0f);
        const float margin=20*s,panelWidth=width-2*margin;
        const glm::vec4 white(.94f,.97f,.95f,1),gold(.96f,.76f,.30f,1),muted(.67f,.79f,.73f,1),panel(.015f,.04f,.03f,.83f);
        rectangle(margin,margin,panelWidth,100*s,panel);
        rectangle(margin,margin,4*s,100*s,gold);
        text("AMS AGRO FARM / GUIDED SIMULATION",margin+16*s,margin+12*s,1.5f*s,muted);
        text(to_string(tour.chapter+1)+"/"+to_string(Chapters.size())+"  "+Chapters[tour.chapter].name,margin+16*s,margin+36*s,2.1f*s,white);
        text(Chapters[tour.chapter].caption,margin+16*s,margin+72*s,1.25f*s,gold);
        const float bottom=height-margin-83*s;
        rectangle(margin,bottom,panelWidth,83*s,panel);
        string status="LIVE FARM / "+to_string(static_cast<int>(tour.elapsed))+" OF "+to_string(static_cast<int>(totalSeconds()))+" SECONDS";
        if(tour.chapter==6) status=string("FARMER: ")+tour.animals.workerStatus()+" / FED "+to_string(tour.animals.deliveredHayLoads());
        if(tour.complete) status="TOUR COMPLETE / LIVE ANIMATIONS CONTINUE / R TO REPLAY";
        if(controls.paused) status="PAUSED / SPACE TO CONTINUE";
        text(status,margin+16*s,bottom+12*s,1.35f*s,white);
        string speed=controls.speed<1?(controls.speed<.5f?"0.25":"0.5"):to_string(static_cast<int>(controls.speed));
        text("SPACE PAUSE / R REPLAY / +/- SPEED "+speed+"X / ESC EXIT",margin+16*s,bottom+37*s,1.35f*s,muted);
        rectangle(margin+16*s,bottom+64*s,panelWidth-32*s,4*s,glm::vec4(.18f,.27f,.22f,1));
        rectangle(margin+16*s,bottom+64*s,(panelWidth-32*s)*static_cast<float>(tour.elapsed/totalSeconds()),4*s,gold);
        shader_.use();shader_.setVec2("resolution",glm::vec2(width,height));
        glDisable(GL_DEPTH_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
        glBufferData(GL_ARRAY_BUFFER,vertices_.size()*sizeof(float),vertices_.data(),GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES,0,static_cast<int>(vertices_.size()/6));glBindVertexArray(0);
        glDisable(GL_BLEND);glEnable(GL_DEPTH_TEST);
    }
};

void rasterView(const Shader& shader,const Tour& tour,const FarmScene& farm,const EntityRenderer& entities,
                const CurvedRenderer& curves,const FarmTextures& textures,const Pose& pose,
                int x,int y,int width,int height,bool orthographic=false,float halfHeight=19.5f,
                glm::vec3 up=glm::vec3(0,1,0))
{
    if(width<=0||height<=0)return;
    const float aspect=static_cast<float>(width)/height;
    shader.use();glViewport(x,y,width,height);
    shader.setMat4("view",glm::lookAt(pose.eye,pose.target,up));
    shader.setMat4("projection",orthographic
        ?glm::ortho(-halfHeight*aspect,halfHeight*aspect,-halfHeight,halfHeight,.1f,120.0f)
        :glm::perspective(glm::radians(pose.fov),aspect,.1f,TerrainRenderer::FarPlane));
    tour.lighting.setupShader(shader,pose.eye);
    const Worker& worker=tour.animals.workers()[0];
    farm.render(shader,tour.gate,tour.stall,tour.calfDoor,tour.animals.fanAngle(),tour.lighting,
                tour.animals.workerDoorOpen(),!worker.carryingHay&&worker.state!=WorkerState::PickingUpHay,
                tour.animals.deliveredHayLoads());
    entities.render(shader,tour.animals);curves.render(shader,textures);
}

void render(const Tour& tour,const Pose& pose,int width,int height,const Shader& phong,const Shader& gouraud,
            const Shader& ray,const FarmScene& farm,const EntityRenderer& entities,const CurvedRenderer& curves,
            const RayTraceRenderer& tracer,const FarmTextures& textures,Overlay& overlay,const Controls& controls)
{
    if(width<=0||height<=0)return;
    const glm::vec3 sky=tour.lighting.clearColor();glClearColor(sky.r,sky.g,sky.b,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(tour.chapter==9)
    {
        ray.use();glViewport(0,0,width,height);glDisable(GL_DEPTH_TEST);
        tour.lighting.setupShader(ray,pose.eye);ray.setBool("nightMode",tour.lighting.nightMode());
        const Worker& worker=tour.animals.workers()[0];
        ray.setBool("workerVisible",worker.state!=WorkerState::HomeIdle);ray.setBool("workerCarryingHay",worker.carryingHay);
        ray.setVec3("workerPosition",worker.position);ray.setInt("deliveredHayLoads",tour.animals.deliveredHayLoads());
        ray.setFloat("workerFeedProgress",worker.state==WorkerState::Feeding?min(worker.taskTimer/1.35f,1.0f):0);
        tracer.render(ray,glm::lookAt(pose.eye,pose.target,glm::vec3(0,1,0)),
                      glm::perspective(glm::radians(pose.fov),static_cast<float>(width)/height,.1f,TerrainRenderer::FarPlane));
        glEnable(GL_DEPTH_TEST);
    }
    else if(tour.chapter==10&&width>=2&&height>=2)
    {
        const int left=width/2,bottom=height/2;
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,bottom,left,height-bottom);
        rasterView(phong,tour,farm,entities,curves,textures,{{0,42,.01f},{0,0,0},48},left,bottom,width-left,height-bottom,true,19.5f,{0,0,-1});
        rasterView(phong,tour,farm,entities,curves,textures,{{0,8,36},{0,2,0},48},0,0,left,bottom,true,12.5f);
        rasterView(phong,tour,farm,entities,curves,textures,{{36,8,0},{0,2,0},48},left,0,width-left,bottom,true,12.5f);
        glEnable(GL_SCISSOR_TEST);glClearColor(.8f,.62f,.27f,1);
        glScissor(left-1,0,3,height);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glScissor(0,bottom-1,width,3);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glDisable(GL_SCISSOR_TEST);
    }
    else rasterView(tour.chapter==7?gouraud:phong,tour,farm,entities,curves,textures,pose,0,0,width,height);
    glViewport(0,0,width,height);overlay.render(tour,controls,width,height);
}
}

int runSimulation(bool smokeTest)
{
    if(!glfwInit()){cerr<<"Simulation: GLFW initialization failed.\n";return 1;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    if(smokeTest)glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
    GLFWwindow* window=glfwCreateWindow(smokeTest?640:1200,smokeTest?400:800,"AMS Agro Farm - Guided Simulation",nullptr,nullptr);
    if(!window){glfwTerminate();return 2;}
    glfwMakeContextCurrent(window);glfwSwapInterval(smokeTest?0:1);
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {glfwDestroyWindow(window);glfwTerminate();return 3;}
    int result=0;
    try
    {
        Shader phong("shaders/texture.vs","shaders/texture.fs"),gouraud("shaders/gouraud.vs","shaders/gouraud.fs"),
               ray("shaders/raytrace.vs","shaders/raytrace.fs");
        TextureManager textures;CubeRenderer cubes;PrimitiveRenderer primitives;CurvedRenderer curves;
        RayTraceRenderer tracer;FarmScene farm(cubes,primitives,textures.farm());EntityRenderer entities(cubes,primitives);
        Overlay overlay;Tour tour;Controls controls;
        glfwSetWindowUserPointer(window,&controls);glfwSetKeyCallback(window,keyCallback);
        glEnable(GL_DEPTH_TEST);
        if(!textures.allFilesLoaded())throw runtime_error("Simulation: a farm texture could not be loaded.");
        cout<<"Guided tour: "<<totalSeconds()<<" seconds. Space: pause, R: replay, +/-: speed, Esc: exit.\n";
        Pose smoothed=tour.pose();double previous=glfwGetTime(),accumulator=0;
        size_t renderedChapter=Chapters.size();bool renderedMidpoint=false;
        WorkerState renderedWorker=WorkerState::HomeIdle;
        array<bool,12> visited{};
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            if(glfwWindowShouldClose(window))break;
            const double now=glfwGetTime();const float wallDt=static_cast<float>(min(now-previous,.1));previous=now;
            if(controls.restart){tour=Tour();smoothed=tour.pose();accumulator=0;controls.restart=false;}
            if(!controls.paused)accumulator+=smokeTest?Tick:wallDt*controls.speed;
            while(accumulator>=Tick){tour.update(Tick);accumulator-=Tick;}
            const Pose desired=tour.pose();
            const float blend=smokeTest?1.0f:1.0f-exp(-3.5f*(controls.paused?0:wallDt*controls.speed));
            smoothed.eye=glm::mix(smoothed.eye,desired.eye,blend);smoothed.target=glm::mix(smoothed.target,desired.target,blend);
            smoothed.fov+=blend*(desired.fov-smoothed.fov);
            if(renderedChapter!=tour.chapter)renderedMidpoint=false;
            const WorkerState state=tour.animals.workers()[0].state;
            const bool halfway=!renderedMidpoint&&tour.time>=Chapters[tour.chapter].seconds*.5;
            if(!smokeTest||renderedChapter!=tour.chapter||halfway||renderedWorker!=state||tour.complete)
            {
                int width,height;glfwGetFramebufferSize(window,&width,&height);
                render(tour,smoothed,width,height,phong,gouraud,ray,farm,entities,curves,tracer,textures.farm(),overlay,controls);
                if(smokeTest)
                {
                    glFinish();if(glGetError()!=GL_NO_ERROR)throw runtime_error("Guided simulation OpenGL error.");
                    visited[tour.chapter]=true;
                }
                glfwSwapBuffers(window);renderedChapter=tour.chapter;renderedWorker=state;
                if(halfway)renderedMidpoint=true;
            }
            if(smokeTest&&tour.complete)
            {
                if(!all_of(visited.begin(),visited.end(),[](bool value){return value;})||!tour.sawPickup||!tour.sawCarry||!tour.sawFeed||!tour.workerReturned)
                    throw runtime_error("Guided simulation missed a chapter or the complete farmer delivery flow.");
                cout<<"Simulation test passed: all 12 chapters, Phong/Gouraud/ray tracing/four views, "
                       "pickup, carry, feed and return home.\n";
                glfwSetWindowShouldClose(window,true);
            }
        }
    }
    catch(const exception& error){cerr<<error.what()<<'\n';result=4;}
    glfwDestroyWindow(window);glfwTerminate();return result;
}
