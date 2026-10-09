// Guided simulation of the real farm, not a separate model of the project.
// All chapter timing, camera choreography and demonstration commands live here.
#include "simulation.h"
#include "farm_scene.h"
#include "entity_renderer.h"
#include "curved_renderer.h"
#include "ray_trace_renderer.h"
#include "collision_system.h"
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
    {"WHOLE FARM AERIAL", "A FOUR SECOND DRONE-STYLE ORBIT ESTABLISHES THE FARM AND ITS SURROUNDINGS", 4},
    {"BANNER AND GATE ARRIVAL", "THE CAMERA FEATURES THE NAME BANNER, DESCENDS TO THE GATE AND ENTERS ON THE ROAD", 8},
    {"TWO HAY DELIVERIES", "THE FARMER LEAVES HOME AND CARRIES TWO SEPARATE HAY BLOCKS TO THE ADULT COWS", 32},
    {"POULTRY RELEASE AND FEED", "THE COOP GATE OPENS; THE FARMER COLLECTS A FEED BOX THROUGH THE BARN DOORWAY", 25},
    {"HOE COLLECTION AND CALF RELEASE", "HE COLLECTS THE HOE, OPENS THE CALF GATE AND STANDS CLEAR OF THE DOORWAY", 25},
    {"WORKING BESIDE THE HAY", "THE FARMER USES THE HOE ON A CULTIVATED PATCH BESIDE THE HAY AREA", 15},
    {"AFTERNOON RETURN ROUND", "HE RETURNS THE HOE, THEN GUIDES THE CALVES AND POULTRY THROUGH THEIR REAL DOORS", 35},
    {"FINAL WATERING ROUND", "ONLY AFTER EVERY ANIMAL IS HOME, HE FILLS THE CATTLE TROUGH AND RETURNS THE HOSE", 28},
    {"SECURING THE FARM", "THE ANIMAL GATES AND MAIN GATE CLOSE BEFORE THE FARMER RETURNS TO HIS HOUSE", 10},
    {"GRAPHICS EFFECT STUDY", "FLAT, GOURAUD AND PHONG LIGHTING ARE COMPARED ON THE SAME SCULPTED CATTLE", 7},
    {"NIGHT AND RAY-TRACED RAIN", "ALL FARM LIGHTS GLOW BEFORE A WET RAY-TRACING ANALYSIS BEGINS", 7},
    {"AFTER THE RAIN", "THE COW SHED AND NAMEPLATE LIGHTS TURN OFF WHILE THE OTHER LIGHTS REMAIN ON", 4}
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
template<size_t N>
glm::vec3 pointOnPath(const array<glm::vec3,N>& points,float progress)
{
    const float scaled=glm::clamp(progress,0.0f,1.0f)*(N-1);
    const size_t index=min(static_cast<size_t>(scaled),N-2);
    return glm::mix(points[index],points[index+1],eased(scaled-index));
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
    bool sawPickup{false}, sawCarry{false}, sawFeed{false}, workerReturned{false};
    bool waterValveTurned{false}, waterHoseTaken{false}, waterHosePlaced{false};
    bool sawWaterFill{false}, hoseRetrieved{false}, hoseReturned{false};
    bool poultryReleased{false}, feedBagTaken{false}, poultryFed{false};
    bool firstHayPicked{false}, secondHayPicked{false}, calvesReleased{false};
    bool hoeTaken{false}, hoeWorkStarted{false}, hoeReturned{false};
    bool calvesRecalled{false}, poultryRecalled{false};
    bool animalsReturned{false}, workerSentHome{false};
    bool waterBeganAfterAnimalsReturned{false}, feedGoneBeforeReturn{false};
    bool completedLightShow{false};
    int lightStage{-1};
    glm::vec3 waterStart{0},poultryStart{0},hayStart{0},calfGateStart{0};
    glm::vec3 hoeStart{0},herdingStart{0},homeStart{0};

    Tour() { animals.preparePoultrySheltered(); enter(); }
    void enter()
    {
        cout << "Simulation " << chapter+1 << '/' << Chapters.size() << ": " << Chapters[chapter].name << '\n';
        if(chapter==2)
        {
            if(!animals.manualWorkerControl())animals.toggleManualWorker();
            hayStart=animals.workers()[0].position;
        }
        if(chapter==3)poultryStart=animals.workers()[0].position;
        if(chapter==4)calfGateStart=animals.workers()[0].position;
        if(chapter==5)hoeStart=animals.workers()[0].position;
        if(chapter==6)
        {
            lighting.setAfternoon(true);
            herdingStart=animals.workers()[0].position;
        }
        if(chapter==7)waterStart=animals.workers()[0].position;
        if(chapter==8)homeStart=animals.workers()[0].position;
        if(chapter==10)
        {
            animals.sendWorkerHome();
            lighting.setAfternoon(false);
            if(!lighting.nightMode())lighting.toggleDayNight();
            animals.setNightMode(true);
            configureNightLights(true);
        }
        if(chapter==11)
        {
            configureNightLights(false);
            completedLightShow=true;
        }
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
        if(chapter==2)
        {
            const array<glm::vec3,6> homeToHay={{hayStart,{8.0f,0,10.25f},{4.6f,0,9.75f},
                {1.0f,0,7.25f},{-2.45f,0,6.35f},FarmInteraction::hayApproachPosition(0)}};
            const array<glm::vec3,5> hayToCowsOne={{FarmInteraction::hayApproachPosition(0),{-2.0f,0,.3f},
                {-3.0f,0,-3.0f},{-7.0f,0,-3.75f},{-12.0f,0,-3.8f}}};
            const array<glm::vec3,5> cowsToHayTwo={{{-12.0f,0,-3.8f},{-7.0f,0,-3.75f},
                {-3.0f,0,-3.0f},{-2.0f,0,.3f},FarmInteraction::hayApproachPosition(1)}};
            const array<glm::vec3,5> hayToCowsTwo={{FarmInteraction::hayApproachPosition(1),{-2.0f,0,.3f},
                {-3.0f,0,-3.0f},{-7.0f,0,-3.75f},{-12.0f,0,-3.8f}}};
            if(time>=.4&&time<6.0)moveDemonstrationWorker(pointOnPath(homeToHay,static_cast<float>((time-.4)/5.6)),dt);
            if(time>=6.1&&!firstHayPicked)firstHayPicked=animals.interactWithWorker();
            if(time>=7.2&&time<12.0&&firstHayPicked)
                moveDemonstrationWorker(pointOnPath(hayToCowsOne,static_cast<float>((time-7.2)/4.8)),dt);
            if(time>=12.2&&animals.deliveredHayLoads()==0&&animals.workers()[0].carryingHay)animals.interactWithWorker();
            if(time>=14.2&&time<19.5&&animals.deliveredHayLoads()>=1)
                moveDemonstrationWorker(pointOnPath(cowsToHayTwo,static_cast<float>((time-14.2)/5.3)),dt);
            if(time>=19.6&&!secondHayPicked)secondHayPicked=animals.interactWithWorker();
            if(time>=20.7&&time<25.7&&secondHayPicked)
                moveDemonstrationWorker(pointOnPath(hayToCowsTwo,static_cast<float>((time-20.7)/5.0)),dt);
            if(time>=25.9&&animals.deliveredHayLoads()==1&&animals.workers()[0].carryingHay)animals.interactWithWorker();
        }
        if(chapter==3)
        {
            const array<glm::vec3,6> toCoop={{poultryStart,{-7.0f,0,-3.7f},{-2.0f,0,0.0f},
                {2.5f,0,3.5f},{5.8f,0,4.5f},{7.25f,0,4.45f}}};
            const array<glm::vec3,7> toFeedStore={{{7.25f,0,4.45f},{6.2f,0,0.5f},
                {6.7f,0,-4.8f},{9.2f,0,-6.8f},{11.0f,0,-7.45f},
                {11.0f,0,-9.25f},FarmInteraction::FeedStorePosition}};
            const array<glm::vec3,7> toPoultry={{FarmInteraction::FeedStorePosition,{11.0f,0,-9.25f},
                {11.0f,0,-7.45f},{8.5f,0,-5.2f},{6.5f,0,-1.0f},
                {6.0f,0,3.6f},FarmInteraction::PoultryFeedPosition}};
            if(time>=.3&&time<5.2)moveDemonstrationWorker(pointOnPath(toCoop,static_cast<float>((time-.3)/4.9)),dt);
            if(time>=5.25&&!poultryReleased){animals.sendPoultryOut();poultryReleased=true;}
            if(time>=6.2&&time<13.4)moveDemonstrationWorker(pointOnPath(toFeedStore,static_cast<float>((time-6.2)/7.2)),dt);
            if(time>=13.6&&!feedBagTaken)feedBagTaken=animals.pickupFeedBag();
            if(time>=14.2&&time<22.2&&feedBagTaken)
                moveDemonstrationWorker(pointOnPath(toPoultry,static_cast<float>((time-14.2)/8.0)),dt);
            if(time>=22.4&&!poultryFed)poultryFed=animals.scatterPoultryFeed();
        }
        if(chapter==4)
        {
            const array<glm::vec3,7> toHoe={{calfGateStart,{6.2f,0,3.5f},{6.6f,0,-4.8f},
                {9.2f,0,-6.8f},{11.0f,0,-7.45f},{11.0f,0,-9.25f},FarmInteraction::HoeStorePosition}};
            const array<glm::vec3,8> toCalfSide={{FarmInteraction::HoeStorePosition,{11.0f,0,-9.25f},
                {11.0f,0,-7.45f},{7.0f,0,-5.0f},{2.0f,0,-4.5f},{-1.5f,0,-5.5f},
                {-3.6f,0,-8.1f},{-4.30f,0,-11.25f}}};
            if(time>=.3&&time<8.0)moveDemonstrationWorker(pointOnPath(toHoe,static_cast<float>((time-.3)/7.7)),dt);
            if(time>=8.2&&!hoeTaken)hoeTaken=animals.pickupHoe();
            if(time>=8.8&&time<18.5&&hoeTaken)
                moveDemonstrationWorker(pointOnPath(toCalfSide,static_cast<float>((time-8.8)/9.7)),dt);
            if(time>=18.7&&!calvesReleased){animals.toggleCalfShed();calvesReleased=true;}
        }
        if(chapter==5)
        {
            const array<glm::vec3,6> toWork={{hoeStart,{-3.6f,0,-7.0f},{-2.5f,0,-3.0f},
                {-2.1f,0,1.5f},{-2.0f,0,4.5f},FarmInteraction::HoeWorkPosition}};
            if(time>=.3&&time<7.5)moveDemonstrationWorker(pointOnPath(toWork,static_cast<float>((time-.3)/7.2)),dt);
            if(time>=7.7&&!hoeWorkStarted)hoeWorkStarted=animals.beginHoeWork();
        }
        if(chapter==6)
        {
            const array<glm::vec3,7> backToHoe={{herdingStart,{1.0f,0,3.0f},{5.5f,0,-1.0f},
                {7.0f,0,-5.0f},{11.0f,0,-7.45f},{11.0f,0,-9.25f},FarmInteraction::HoeStorePosition}};
            const array<glm::vec3,8> toCalves={{FarmInteraction::HoeStorePosition,{11.0f,0,-9.25f},
                {11.0f,0,-7.45f},{7.0f,0,-5.0f},{2.0f,0,-4.5f},{-1.5f,0,-5.5f},
                {-3.6f,0,-8.1f},{-4.30f,0,-11.25f}}};
            const array<glm::vec3,7> toBirds={{{-4.30f,0,-11.25f},{-2.5f,0,-6.0f},
                {0.0f,0,-2.0f},{2.5f,0,2.0f},{4.8f,0,4.2f},{6.3f,0,4.45f},{7.25f,0,4.45f}}};
            if(time>=.2&&time<7.4)moveDemonstrationWorker(pointOnPath(backToHoe,static_cast<float>((time-.2)/7.2)),dt);
            if(time>=7.6&&!hoeReturned)hoeReturned=animals.returnHoe();
            if(time>=8.0&&time<16.0)moveDemonstrationWorker(pointOnPath(toCalves,static_cast<float>((time-8.0)/8.0)),dt);
            if(time>=16.2&&!calvesRecalled){animals.sendCalvesHome();calvesRecalled=true;}
            if(time>=17.0&&time<23.0)moveDemonstrationWorker(pointOnPath(toBirds,static_cast<float>((time-17.0)/6.0)),dt);
            if(time>=23.2&&!poultryRecalled)
            {
                feedGoneBeforeReturn=animals.poultryFeedLevel()<=0.001f;
                animals.sendPoultryHome();poultryRecalled=true;
            }
        }
        if(chapter==7)
        {
            const array<glm::vec3,4> toTap={{waterStart,{7.0f,0,4.6f},{7.0f,0,4.0f},{7.10f,0,3.85f}}};
            const array<glm::vec3,6> toTrough={{{7.10f,0,3.85f},{4.5f,0,5.0f},{1.0f,0,4.0f},
                {-2.5f,0,1.0f},{-3.0f,0,-3.75f},FarmInteraction::WaterTroughInteraction}};
            const array<glm::vec3,6> backToTap={{FarmInteraction::WaterTroughInteraction,
                {-3.0f,0,-3.75f},{-2.5f,0,1.0f},{1.0f,0,4.0f},{4.5f,0,5.0f},{7.10f,0,3.85f}}};
            if(time>=.4&&time<3.4)moveDemonstrationWorker(pointOnPath(toTap,static_cast<float>((time-.4)/3.0)),dt);
            if(time>=3.6&&!waterValveTurned)
            {
                waterValveTurned=animals.toggleWaterValve();
                if(waterValveTurned)waterBeganAfterAnimalsReturned=animalsReturned;
            }
            if(time>=4.2&&!waterHoseTaken)waterHoseTaken=animals.interactWithWorker()&&animals.workerHoldingHose();
            if(time>=4.5&&time<12.3&&waterHoseTaken)
                moveDemonstrationWorker(pointOnPath(toTrough,static_cast<float>((time-4.5)/7.8)),dt);
            if(time>=12.5&&!waterHosePlaced&&animals.workerHoldingHose())
                waterHosePlaced=animals.interactWithWorker()&&animals.hosePlacedAtTrough();
            if(time>=21.0&&!hoseRetrieved&&animals.hosePlacedAtTrough())
                hoseRetrieved=animals.interactWithWorker()&&animals.workerHoldingHose();
            if(time>=21.3&&time<26.9&&hoseRetrieved)
                moveDemonstrationWorker(pointOnPath(backToTap,static_cast<float>((time-21.3)/5.6)),dt);
            if(time>=27.0&&!hoseReturned&&animals.workerHoldingHose())
                hoseReturned=animals.interactWithWorker()&&!animals.workerHoldingHose();
            if(time>=27.2&&animals.waterValveOpen())animals.toggleWaterValve();
        }
        if(chapter==8)
        {
            const array<glm::vec3,7> toHome={{homeStart,{7.0f,0,4.8f},{7.6f,0,5.0f},
                {6.5f,0,7.2f},{6.0f,0,9.6f},{8.0f,0,10.35f},{8.0f,.22f,12.70f}}};
            if(time>=.3&&time<8.8)moveDemonstrationWorker(pointOnPath(toHome,static_cast<float>((time-.3)/8.5)),dt);
            if(time>=9.0&&!workerSentHome){animals.sendWorkerHome();workerSentHome=true;}
        }
        animals.update(dt);
        lighting.update(dt);
        gate = approach(gate, chapter >= 1 && chapter < 8 ? 90.0f : 0.0f, 60.0f*dt);
        stall = approach(stall, 0.0f, 60.0f*dt);
        calfDoor = approach(calfDoor, animals.calfShedDoorNeeded() ? 92.0f : 0.0f, 180.0f*dt);
        const Worker& worker = animals.workers()[0];
        sawPickup |= worker.state == WorkerState::PickingUpHay;
        sawCarry |= worker.carryingHay;
        sawFeed |= worker.state == WorkerState::Feeding;
        workerReturned |= chapter>=8&&worker.deliveredLoads>=2&&worker.state==WorkerState::HomeIdle;
        sawWaterFill |= animals.waterTroughFill()>.55f;
        if(chapter>=6&&calvesRecalled&&poultryRecalled)
        {
            const bool calvesHome=all_of(animals.calves().begin(),animals.calves().end(),
                [](const Calf& calf){return calf.routeState==Calf::RouteState::Sheltered;});
            const bool birdsHome=all_of(animals.birds().begin(),animals.birds().end(),
                [](const Bird& bird){return !bird.mobile||bird.routeState==Calf::RouteState::Sheltered;});
            animalsReturned|=calvesHome&&birdsHome;
        }
    }
    Pose pose() const
    {
        const float u = static_cast<float>(time/Chapters[chapter].seconds);
        switch (chapter)
        {
        case 0:
        {
            const float angle=.20f+u*1.35f;
            return {{sin(angle)*30.0f,22.5f,cos(angle)*30.0f},{0,0,-1.5f},54};
        }
        case 1:
            if(time<2.4)return mixPose({{-7.5f,5.8f,25.5f},{0,4.7f,15.5f},38},
                                       {{0,5.0f,24.0f},{0,4.7f,15.5f},34},static_cast<float>(time/2.4));
            if(time<4.0)return mixPose({{0,4.7f,24.0f},{0,2.4f,15.5f},42},
                                       {{0,3.7f,19.8f},{0,1.8f,14.0f},45},static_cast<float>((time-2.4)/1.6));
            return mixPose({{0,3.7f,19.8f},{0,1.8f,13.5f},45},
                           {{0,3.2f,8.0f},{0,1.2f,-1.0f},49},static_cast<float>((time-4.0)/4.0));
        case 2:
        {
            const Worker& worker=animals.workers()[0];
            if(time<7.2)return {{-1.0f,6.2f,8.4f},worker.position+glm::vec3(0,1.0f,0),48};
            if(time<14.0)return {worker.position+glm::vec3(4.2f,3.2f,4.8f),worker.position+glm::vec3(0,1.1f,0),46};
            if(time<20.7)return {{-.8f,5.8f,8.0f},worker.position+glm::vec3(0,1.0f,0),47};
            return {worker.position+glm::vec3(4.0f,3.1f,4.6f),worker.position+glm::vec3(0,1.1f,0),46};
        }
        case 3:
        {
            const Worker& worker=animals.workers()[0];
            if(time<6.2)return {{4.3f,4.1f,10.7f},{9.4f,.7f,6.0f},47};
            if(time<10.8)return {{3.0f,5.0f,-1.0f},worker.position+glm::vec3(0,1,0),50};
            if(time<14.2)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1.0f,0),44};
            if(time<22.2)return {{2.7f,5.3f,1.2f},worker.position+glm::vec3(0,1.0f,0),50};
            return {{2.2f,3.3f,10.0f},FarmInteraction::PoultryFeedPosition+glm::vec3(0,.55f,0),44};
        }
        case 4:
        {
            const Worker& worker=animals.workers()[0];
            if(time<6.5)return {{3.0f,5.2f,1.0f},worker.position+glm::vec3(0,1,0),50};
            if(time<10.0)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1,0),44};
            if(time<18.2)return {{0.3f,5.3f,-3.2f},worker.position+glm::vec3(0,1,0),48};
            const glm::vec3 calves=(animals.calves()[0].position+animals.calves()[1].position)*.5f;
            return {{-1.3f,3.6f,-13.6f},glm::mix(calves,glm::vec3(-5.6f,.8f,-9.65f),.35f),44};
        }
        case 5:
        {
            const Worker& worker=animals.workers()[0];
            return {{3.0f,4.4f,10.0f},glm::mix(worker.position,FarmInteraction::HoeWorkPosition,.45f)+glm::vec3(0,.75f,0),46};
        }
        case 6:
        {
            const Worker& worker=animals.workers()[0];
            if(time<7.8)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1,0),44};
            const glm::vec3 calves=(animals.calves()[0].position+animals.calves()[1].position)*.5f;
            glm::vec3 birds(0);
            int mobile=0;
            for(const Bird& bird:animals.birds())if(bird.mobile){birds+=bird.position;++mobile;}
            if(mobile>0)birds/=static_cast<float>(mobile);
            if(time<22.5)return {{-1.2f,4.0f,-13.8f},glm::mix(worker.position,calves,.55f)+glm::vec3(0,.7f,0),45};
            return {{4.4f,4.1f,1.7f},glm::mix(worker.position,birds,.62f)+glm::vec3(0,.55f,0),46};
        }
        case 7:
        {
            const Worker& worker=animals.workers()[0];
            if(time<4.5)return {{11.2f,3.4f,6.2f},worker.position+glm::vec3(0,1.0f,0),47};
            if(time<12.5)return {worker.position+glm::vec3(4.0f,3.2f,4.7f),worker.position+glm::vec3(0,1.0f,0),47};
            if(time<21.2)return {{-3.0f,3.8f,-.8f},{-9.5f,.9f,-5.5f},44};
            return {worker.position+glm::vec3(4.0f,3.3f,4.8f),worker.position+glm::vec3(0,1.0f,0),47};
        }
        case 8:
        {
            const Worker& worker=animals.workers()[0];
            if(time<3.0)return {{4.2f,4.0f,9.0f},{8.5f,.8f,5.7f},46};
            return {{3.7f,3.7f,8.5f},worker.position+glm::vec3(0,1.0f,0),45};
        }
        case 9:
            // Stable close framing makes the normal interpolation difference
            // readable across the three simultaneous comparison panels.
            return {{-18.2f,4.1f,-0.8f},{-10.2f,1.85f,-7.45f},38};
        case 10:
            return mixPose({{-8.0f,7.2f,31.5f},{0,1.5f,2.0f},55},
                           {{7.0f,6.5f,29.0f},{0,1.3f,0.0f},53},u);
        default:
            return mixPose({{-16.0f,8.5f,27.0f},{0,1.8f,8.0f},49},
                           {{12.0f,8.0f,27.0f},{0,1.8f,8.0f},49},u);
        }
    }
    bool rainActive() const { return chapter==10&&time>=1.6; }
    int cameraShot() const
    {
        if(chapter==1)return static_cast<int>(chapter)*10+(time<2.4?0:(time<4.0?1:2));
        if(chapter==2)return 20+(time<7.2?0:(time<14.0?1:(time<20.7?2:3)));
        if(chapter==3)return 30+(time<6.2?0:(time<10.8?1:(time<14.2?2:(time<22.2?3:4))));
        if(chapter==4)return 40+(time<6.5?0:(time<10.0?1:(time<18.2?2:3)));
        if(chapter==6)return 60+(time<7.8?0:(time<22.5?1:2));
        if(chapter==7)return 70+(time<4.5?0:(time<12.5?1:(time<21.2?2:3)));
        if(chapter==8)return 80+(time<3.0?0:1);
        return static_cast<int>(chapter)*10;
    }
private:
    void configureNightLights(bool allOn)
    {
        lightStage=allOn?0:1;
        const bool shed=allOn;
        const bool fence=true;
        const bool banner=allOn; // entrance nameplate
        const bool billboard=true;
        const bool spot=true;
        const bool directional=true;
        if(lighting.shedLightsEnabled()!=shed)lighting.toggleShedLights();
        if(lighting.fenceLightsEnabled()!=fence)lighting.toggleFenceLights();
        if(lighting.bannerLightEnabled()!=banner)lighting.toggleBannerLight();
        if(lighting.billboardLightEnabled()!=billboard)lighting.toggleBillboardLight();
        if(lighting.spotlightEnabled()!=spot)lighting.toggleSpotlight();
        if(lighting.directionalEnabled()!=directional)lighting.toggleDirectional();
        if(!lighting.spotlightSweep())lighting.toggleSpotlightSweep();
    }
    void moveDemonstrationWorker(const glm::vec3& next,float dt)
    {
        const Worker& worker=animals.workers()[0];
        const glm::vec2 difference(next.x-worker.position.x,next.z-worker.position.z);
        if(glm::length(difference)>.0001f)
        {
            const float target=glm::degrees(atan2(-difference.y,difference.x));
            float turn=fmod(target-worker.yaw,360.0f);
            if(turn>180)turn-=360;if(turn<-180)turn+=360;
            animals.turnManualWorker(turn);
        }
        animals.moveManualWorker(next,dt);
    }
};

bool cameraPositionClear(const Tour& tour,const glm::vec3& position)
{
    return CollisionSystem::canOccupy(position,tour.gate,tour.stall,tour.calfDoor,
        tour.animals.workerDoorAngle(),tour.animals.hayAvailableMask());
}

Pose collisionSafePose(const Tour& tour,Pose pose)
{
    if(cameraPositionClear(tour,pose.eye))return pose;
    const glm::vec3 offset=pose.eye-pose.target;
    for(float degrees:{45.0f,-45.0f,90.0f,-90.0f,135.0f,-135.0f,180.0f})
    {
        const float angle=glm::radians(degrees);
        Pose alternative=pose;
        alternative.eye=pose.target+glm::vec3(offset.x*cos(angle)-offset.z*sin(angle),
            offset.y,offset.x*sin(angle)+offset.z*cos(angle));
        if(cameraPositionClear(tour,alternative.eye))return alternative;
    }
    pose.eye.y=max(pose.eye.y,8.2f);
    return pose;
}

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
    void quadrilateral(const glm::vec2& a,const glm::vec2& b,const glm::vec2& c,
                       const glm::vec2& d,const glm::vec4& color)
    {
        for(const glm::vec2& point:{a,b,c,a,c,d})
            vertices_.insert(vertices_.end(),{point.x,point.y,color.r,color.g,color.b,color.a});
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

        // Deterministic screen-space rain keeps the finale smooth and makes
        // every replay frame-identical, which is useful when recording video.
        if(tour.rainActive())
        {
            for(int index=0;index<110;++index)
            {
                const float x=fmod(index*97.0f+static_cast<float>(tour.time)*61.0f,width+80.0f)-40.0f;
                const float y=fmod(index*53.0f+static_cast<float>(tour.time)*335.0f,height+90.0f)-45.0f;
                const float length=(13.0f+static_cast<float>(index%17))*s;
                const float widthPixels=(1.0f+static_cast<float>(index%3)*.45f)*s;
                quadrilateral({x,y},{x+widthPixels,y},{x-5.0f*s+widthPixels,y+length},
                              {x-5.0f*s,y+length},glm::vec4(.66f,.82f,1.0f,.42f));
            }
        }
        rectangle(margin,margin,panelWidth,100*s,panel);
        rectangle(margin,margin,4*s,100*s,gold);
        text("AMS AGRO FARM / GUIDED SIMULATION",margin+16*s,margin+12*s,1.5f*s,muted);
        text(to_string(tour.chapter+1)+"/"+to_string(Chapters.size())+"  "+Chapters[tour.chapter].name,margin+16*s,margin+36*s,2.1f*s,white);
        text(Chapters[tour.chapter].caption,margin+16*s,margin+72*s,1.25f*s,gold);

        const float technicalWidth=286*s;
        const float technicalX=width-margin-technicalWidth;
        rectangle(technicalX,margin,technicalWidth,100*s,glm::vec4(.018f,.055f,.048f,.95f));
        rectangle(technicalX,margin,3*s,100*s,muted);
        const string timeName=tour.lighting.nightMode()?"NIGHT 22:00":
            (tour.lighting.afternoonMode()?"AFTERNOON 16:30":"MORNING 10:30");
        string renderName="PHONG / TEXTURED";
        if(tour.chapter==9)renderName="FLAT / GOURAUD / PHONG";
        if(tour.rainActive())renderName="RASTER + RAY TRACING";
        string cameraName=tour.chapter==0?"AERIAL ORBIT":(tour.chapter==1?"BANNER / GATE DOLLY":
            (tour.chapter==9?"THREE PANEL":(tour.rainActive()?"WET ANALYSIS":"CINEMATIC FOLLOW")));
        text("TIME  "+timeName,technicalX+13*s,margin+10*s,1.05f*s,white);
        if(tour.rainActive())
        {
            text("PIPELINE  ANALYTIC GPU RAYS",technicalX+13*s,margin+29*s,.92f*s,gold);
            text("SHADOWS  HARD OCCLUSION",technicalX+13*s,margin+48*s,.92f*s,white);
            text("BOUNCES  1 REFLECTION",technicalX+13*s,margin+67*s,.92f*s,white);
            text("WET SURFACE  100 PERCENT",technicalX+13*s,margin+86*s,.82f*s,muted);
        }
        else
        {
            text("RENDER  "+renderName,technicalX+13*s,margin+29*s,1.05f*s,gold);
            text("CAMERA  "+cameraName,technicalX+13*s,margin+48*s,1.05f*s,muted);
            text("LIGHTS  SUN POINT SPOT",technicalX+13*s,margin+67*s,1.05f*s,white);
            text("TRANSFORMS  MODEL VIEW PROJECTION",technicalX+13*s,margin+86*s,.78f*s,muted);
        }

        if(tour.chapter==9)
        {
            const float third=static_cast<float>(width)/3.0f;
            for(int index=0;index<3;++index)
            {
                const float chipX=third*index+third*.5f-52*s;
                rectangle(chipX,margin+112*s,104*s,25*s,panel);
                text(index==0?"FLAT":(index==1?"GOURAUD":"PHONG"),
                     chipX+12*s,margin+119*s,1.25f*s,index==2?gold:white);
            }
        }
        if(tour.rainActive())
        {
            const float half=static_cast<float>(width)*.5f;
            rectangle(half-1.5f*s,margin+108*s,3*s,height-2*margin-195*s,
                      glm::vec4(.96f,.76f,.30f,.68f));
            rectangle(half*.5f-92*s,margin+112*s,184*s,25*s,panel);
            text("PHONG RASTER FARM",half*.5f-77*s,margin+119*s,1.05f*s,white);
            rectangle(half+half*.5f-106*s,margin+112*s,212*s,25*s,panel);
            text("RAY TRACED WET VIEW",half+half*.5f-91*s,margin+119*s,1.05f*s,gold);
        }
        const float bottom=height-margin-49*s;
        rectangle(margin,bottom,panelWidth,49*s,panel);
        string status="LIVE FARM / "+to_string(static_cast<int>(tour.elapsed))+" OF "+to_string(static_cast<int>(totalSeconds()))+" SECONDS";
        if(tour.chapter==2)status=string("HAY DELIVERIES  ")+to_string(tour.animals.deliveredHayLoads())+" / 2   "+tour.animals.workerStatus();
        if(tour.chapter==3)status=tour.poultryFed?"POULTRY RELEASED AND FED / GRAIN IS BEING CONSUMED":"COOP GATE OPEN / FARMER COLLECTING THE FEED BOX";
        if(tour.chapter==4)status=tour.calvesReleased?string("FARMER CLEAR OF DOORWAY / ")+tour.animals.calfStatus():"HOE COLLECTED / WALKING TO THE CALF SIDE GATE";
        if(tour.chapter==5)status=string("VISIBLE FARM WORK / ")+tour.animals.workerStatus();
        if(tour.chapter==6){
            if(!tour.calvesRecalled)status="FARMER WALKING TO BRING THE CALVES HOME";
            else if(!tour.poultryRecalled)status="CALVES RETURNING / FARMER WALKING TO THE POULTRY";
            else if(!tour.animalsReturned)status="FARMER GUIDING THE HENS AND DUCKS INTO THEIR COOP";
            else status="ALL CALVES HENS AND DUCKS SAFELY INSIDE / NO GRAIN REMAINS";
        }
        if(tour.chapter==7)status=tour.hoseReturned?"CATTLE WATERED / HOSE RETURNED / VALVE CLOSED":string("WATER  ")+tour.animals.waterStatus()+" / TROUGH "+to_string(static_cast<int>(tour.animals.waterTroughFill()*100))+" PERCENT";
        if(tour.chapter==8)status=tour.workerReturned?"ALL GATES SECURED / FARMER SAFELY INSIDE":"SECURING THE COOP AND MAIN GATE / RETURNING HOME";
        if(tour.chapter==9)status="ONE CAMERA / ONE MODEL / THREE DIFFERENT NORMAL INTERPOLATION METHODS";
        if(tour.chapter==10)status=tour.rainActive()?"RAY ANALYSIS / HARD SHADOWS / WET REFLECTION / ONE SECONDARY BOUNCE":"NIGHT BEGINS / ALL FARM LIGHTS ON";
        if(tour.chapter==11)status="RAIN CLEARED / COW SHED AND NAMEPLATE OFF / FENCE GATE BILLBOARD AND SPOTLIGHT ON";
        if(tour.complete) status="TOUR COMPLETE / LIVE ANIMATIONS CONTINUE / R TO REPLAY";
        if(controls.paused) status="PAUSED / SPACE TO CONTINUE";
        text(status,margin+16*s,bottom+12*s,1.35f*s,white);
        rectangle(margin+16*s,bottom+38*s,panelWidth-32*s,3*s,glm::vec4(.18f,.27f,.22f,1));
        rectangle(margin+16*s,bottom+38*s,(panelWidth-32*s)*static_cast<float>(tour.elapsed/totalSeconds()),3*s,gold);
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
                glm::vec3 up=glm::vec3(0,1,0),bool flatShading=false)
{
    if(width<=0||height<=0)return;
    const float aspect=static_cast<float>(width)/height;
    shader.use();glViewport(x,y,width,height);
    shader.setMat4("view",glm::lookAt(pose.eye,pose.target,up));
    shader.setMat4("projection",orthographic
        ?glm::ortho(-halfHeight*aspect,halfHeight*aspect,-halfHeight,halfHeight,.1f,120.0f)
        :glm::perspective(glm::radians(pose.fov),aspect,.1f,TerrainRenderer::FarPlane));
    tour.lighting.setupShader(shader,pose.eye);
    shader.setBool("flatShading",flatShading);
    const Worker& worker=tour.animals.workers()[0];
    farm.render(shader,tour.gate,tour.stall,tour.calfDoor,tour.animals.poultryDoorAngle(),
                tour.animals.fanAngle(),tour.lighting,
                tour.animals.workerDoorAngle(),tour.animals.hayAvailableMask(),
                tour.animals.deliveredHayLoads(),tour.animals.waterValveOpen(),
                tour.animals.waterValveAngle(),tour.animals.workerHoldingHose(),
                tour.animals.hosePlacedAtTrough(),tour.animals.hoseEndPosition(),
                tour.animals.waterTroughFill(),tour.animals.feedBagAvailable(),
                tour.animals.hoeAvailable());
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
        const int first=width/3,second=width/3,third=width-first-second;
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,0,first,height,
                   false,19.5f,glm::vec3(0,1,0),true);
        rasterView(gouraud,tour,farm,entities,curves,textures,pose,first,0,second,height);
        rasterView(phong,tour,farm,entities,curves,textures,pose,first+second,0,third,height);
    }
    else if(tour.rainActive())
    {
        // Keep the detailed raster farm visible while the analytic renderer
        // demonstrates hard shadow rays and a reflected wet surface beside it.
        // This comparison is easier to explain than replacing the whole farm
        // with the intentionally compact ray-traced geometry.
        const int rasterWidth=width/2;
        const int rayWidth=width-rasterWidth;
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,0,rasterWidth,height);
        const float aspect=static_cast<float>(rayWidth)/height;
        const glm::mat4 view=glm::lookAt(pose.eye,pose.target,glm::vec3(0,1,0));
        const glm::mat4 projection=glm::perspective(glm::radians(pose.fov),aspect,.1f,TerrainRenderer::FarPlane);
        glViewport(rasterWidth,0,rayWidth,height);glDisable(GL_DEPTH_TEST);ray.use();
        tour.lighting.setupShader(ray,pose.eye);
        ray.setBool("nightMode",true);
        ray.setBool("workerVisible",false);
        ray.setBool("workerCarryingHay",false);
        ray.setVec3("workerPosition",tour.animals.workers()[0].position);
        ray.setFloat("workerFeedProgress",0.0f);
        ray.setInt("deliveredHayLoads",tour.animals.deliveredHayLoads());
        ray.setFloat("weatherWetness",1.0f);
        tracer.render(ray,view,projection);glEnable(GL_DEPTH_TEST);
    }
    else
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,0,width,height);
    glViewport(0,0,width,height);overlay.render(tour,controls,width,height);
}
}

int runSimulation(bool smokeTest)
{
    if(!glfwInit()){cerr<<"Simulation: GLFW initialization failed.\n";return 1;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_SAMPLES,4);
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
        glEnable(GL_MULTISAMPLE);
        if(!textures.allFilesLoaded())throw runtime_error("Simulation: a farm texture could not be loaded.");
        cout<<"Guided tour: "<<totalSeconds()<<" seconds. Space: pause, R: replay, +/-: speed, Esc: exit.\n";
        Pose smoothed=collisionSafePose(tour,tour.pose());double previous=glfwGetTime(),accumulator=0;
        int activeCameraShot=tour.cameraShot();
        size_t renderedChapter=Chapters.size();bool renderedMidpoint=false;
        WorkerState renderedWorker=WorkerState::HomeIdle;
        array<bool,Chapters.size()> visited{};
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            if(glfwWindowShouldClose(window))break;
            const double now=glfwGetTime();const float wallDt=static_cast<float>(min(now-previous,.1));previous=now;
            if(controls.restart){tour=Tour();smoothed=collisionSafePose(tour,tour.pose());activeCameraShot=tour.cameraShot();accumulator=0;controls.restart=false;}
            if(!controls.paused)accumulator+=smokeTest?Tick:wallDt*controls.speed;
            while(accumulator>=Tick){tour.update(Tick);accumulator-=Tick;}
            const Pose desired=collisionSafePose(tour,tour.pose());
            const int requestedShot=tour.cameraShot();
            const float blend=smokeTest?1.0f:1.0f-exp(-3.5f*(controls.paused?0:wallDt*controls.speed));
            if(requestedShot!=activeCameraShot)
            {
                // A deliberate cinematic cut prevents interpolation from
                // carrying the camera through a roof or wall between setups.
                smoothed=desired;
                activeCameraShot=requestedShot;
            }
            else
            {
                const glm::vec3 requestedEye=glm::mix(smoothed.eye,desired.eye,blend);
                if(cameraPositionClear(tour,requestedEye))smoothed.eye=requestedEye;
                else
                {
                    glm::vec3 detour=smoothed.eye;
                    detour.y=approach(detour.y,8.2f,max(.08f,blend*2.8f));
                    if(cameraPositionClear(tour,detour))smoothed.eye=detour;
                }
                smoothed.target=glm::mix(smoothed.target,desired.target,blend);
                smoothed.fov+=blend*(desired.fov-smoothed.fov);
            }
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
                if(!all_of(visited.begin(),visited.end(),[](bool value){return value;})||
                   !tour.sawPickup||!tour.sawCarry||!tour.sawFeed||
                   !tour.waterValveTurned||!tour.waterHoseTaken||!tour.waterHosePlaced||
                   !tour.sawWaterFill||!tour.hoseRetrieved||!tour.hoseReturned||
                   !tour.poultryReleased||!tour.feedBagTaken||!tour.poultryFed||
                   tour.animals.deliveredHayLoads()!=2||!tour.calvesReleased||
                   !tour.hoeTaken||!tour.hoeWorkStarted||!tour.hoeReturned||
                   !tour.calvesRecalled||!tour.poultryRecalled||!tour.animalsReturned||
                   !tour.waterBeganAfterAnimalsReturned||!tour.feedGoneBeforeReturn||
                   !tour.workerReturned||!tour.completedLightShow)
                    throw runtime_error("200-second simulation missed a required workday or graphics-showcase action.");
                cout<<"Simulation test passed: exactly 200 seconds - sheltered poultry start, water and hose return, "
                       "feed collection, two hay loads, calf release, hoe work, all returns, three shading modes, "
                       "night lighting and the ray-traced rain finale.\n";
                glfwSetWindowShouldClose(window,true);
            }
        }
    }
    catch(const exception& error){cerr<<error.what()<<'\n';result=4;}
    glfwDestroyWindow(window);glfwTerminate();return result;
}
