// Guided simulation of the real farm, not a separate model of the project.
// All chapter timing, camera choreography and demonstration commands live here.
#include "simulation.h"
#include "farm_scene.h"
#include "entity_renderer.h"
#include "curved_renderer.h"
#include "wet_reflection_renderer.h"
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
constexpr double NightRainStart=18.0;
constexpr double DuskTransitionEnd=8.0;
constexpr double RainInspectionStart=27.0;
constexpr double RainInspectionArrive=31.3;
struct Chapter { const char* name; const char* caption; double seconds; };
const array<Chapter, 12> Chapters = {{
    {"MORNING AERIAL", "AN OBLIQUE AERIAL VIEW ESTABLISHES THE WHOLE FARM AND THE SURROUNDING FIELDS", 4},
    {"BANNER AND GATE ARRIVAL", "THE BILLBOARD AND NAME BANNER INTRODUCE THE FARM BEFORE THE CAMERA ENTERS", 10},
    {"TWO HAY DELIVERIES", "THE FARMER LEAVES HOME AND CARRIES TWO SEPARATE HAY BLOCKS TO THE ADULT COWS", 30},
    {"POULTRY RELEASE AND FEED", "THE COOP GATE OPENS; THE FARMER COLLECTS A FEED BOX THROUGH THE BARN DOORWAY", 25},
    {"HOE COLLECTION AND CALF RELEASE", "HE RETURNS THE EMPTY FEED BOX, COLLECTS THE HOE AND RELEASES THE CALVES", 23},
    {"LOWERING THE HAY PILE", "THE FARMER HOOKS A TOP BALE WITH HIS HOE AND LOWERS IT INTO A GROUND PILE", 13},
    {"DAYLIGHT VESSEL SHADING", "THE BEZIER MILK VESSEL SHOWS FLAT, GOURAUD AND PHONG HIGHLIGHTS IN DAYLIGHT", 6},
    {"AFTERNOON RETURN ROUND", "HE RETURNS THE HOE, THEN GUIDES THE CALVES AND POULTRY THROUGH THEIR REAL DOORS", 35},
    {"FINAL WATERING ROUND", "BOTH COWS RECEIVE FRESH WATER AFTER THE MORNING HAY HAS BEEN EATEN", 24},
    {"SECURING THE FARM", "THE ANIMAL GATES AND MAIN GATE CLOSE BEFORE THE FARMER RETURNS TO HIS HOUSE", 10},
    {"DUSK NIGHT AERIAL AND RAIN", "AFTERNOON FADES TO NIGHT AS FARM LIGHTS COME ON, FOLLOWED BY RAIN AND INSPECTION", 40},
    {"AFTER THE RAIN", "THE FARMER RETURNS HOME; SHED AND NAMEPLATE LIGHTS TURN OFF WHILE THE OTHERS STAY ON", 20}
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
    bool secondWaterPlaced{false}, firstWaterRetrieved{false};
    bool poultryReleased{false}, feedBagTaken{false}, poultryFed{false}, feedBoxReturned{false};
    bool inspectionStarted{false}, umbrellaShown{false};
    glm::vec3 inspectionHome{0};
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
        if(chapter==7)
        {
            lighting.setAfternoon(true);
            animals.consumeCowHay();
            herdingStart=animals.workers()[0].position;
        }
        if(chapter==8)waterStart=animals.workers()[0].position;
        if(chapter==9)homeStart=animals.workers()[0].position;
        if(chapter==10)
        {
            animals.sendWorkerHome();
            lighting.setAfternoon(true);
            lighting.setDuskProgress(0.0f);
            configureNightLights(0);
        }
        if(chapter==11)
        {
            animals.setWorkerUmbrella(false);
            homeStart=animals.workers()[0].position;
            workerSentHome=false;
            configureNightLights(0);
        }
    }
    void update(float dt)
    {
        // Walking is renewed only by a movement command in this tick. It
        // must stop while the farmer lifts, scatters, hooks a bale or waits.
        animals.stopManualWorker();
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
                {-3.0f,0,-3.0f},{-6.0f,0,-3.75f},{-8.0f,0,-3.8f}}};
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
                {2.5f,0,3.5f},{5.8f,0,4.5f},{12.45f,0,5.0f}}};
            const array<glm::vec3,7> toFeedStore={{{12.45f,0,5.0f},{7.0f,0,4.5f},
                {6.0f,0,-4.8f},{9.2f,0,-6.8f},{11.0f,0,-7.45f},
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
            if(time>=23.4&&poultryFed)
            {
                const array<glm::vec3,7> returnBox={{FarmInteraction::PoultryFeedPosition,{6.0f,0,3.6f},
                    {6.5f,0,-1.0f},{8.5f,0,-5.2f},{11.0f,0,-7.45f},{11.0f,0,-9.25f},FarmInteraction::FeedStorePosition}};
                moveDemonstrationWorker(pointOnPath(returnBox,static_cast<float>((time-23.4)/7.5)),dt);
            }
        }
        if(chapter==4)
        {
            const array<glm::vec3,7> returnBox={{FarmInteraction::PoultryFeedPosition,{6.0f,0,3.6f},
                {6.5f,0,-1.0f},{8.5f,0,-5.2f},{11.0f,0,-7.45f},{11.0f,0,-9.25f},FarmInteraction::FeedStorePosition}};
            const array<glm::vec3,2> toHoe={{FarmInteraction::FeedStorePosition,FarmInteraction::HoeStorePosition}};
            const array<glm::vec3,8> toCalfSide={{FarmInteraction::HoeStorePosition,{11.0f,0,-9.25f},
                {11.0f,0,-7.45f},{7.0f,0,-5.0f},{2.0f,0,-4.5f},{-1.5f,0,-5.5f},
                {-3.6f,0,-8.1f},{-4.30f,0,-11.25f}}};
            if(time<5.9)moveDemonstrationWorker(pointOnPath(returnBox,static_cast<float>((time+1.6)/7.5)),dt);
            if(time>=6.0&&!feedBoxReturned)feedBoxReturned=animals.returnFeedBox();
            if(time>=6.0&&time<6.8&&feedBoxReturned)moveDemonstrationWorker(pointOnPath(toHoe,static_cast<float>((time-6.0)/.8)),dt);
            if(time>=6.9&&!hoeTaken&&feedBoxReturned)hoeTaken=animals.pickupHoe();
            if(time>=7.2&&time<17.0&&hoeTaken)
                moveDemonstrationWorker(pointOnPath(toCalfSide,static_cast<float>((time-7.2)/9.8)),dt);
            if(time>=17.0)
            {
                const Worker& worker=animals.workers()[0];
                const glm::vec3 toCalves=glm::vec3(-6.75f,0,-9.4f)-worker.position;
                const float facing=glm::degrees(atan2(-toCalves.z,toCalves.x));
                animals.turnManualWorker(FarmInteraction::turnToward(worker.yaw,facing,dt)-worker.yaw);
            }
            if(time>=17.7&&!calvesReleased){animals.toggleCalfShed();calvesReleased=true;}
        }
        if(chapter==5)
        {
            const array<glm::vec3,6> toWork={{hoeStart,{-3.6f,0,-7.0f},{-2.5f,0,-3.0f},
                {-2.1f,0,.5f},{-3.2f,0,1.0f},FarmInteraction::HoeWorkPosition}};
            if(time>=.3&&time<7.5)moveDemonstrationWorker(pointOnPath(toWork,static_cast<float>((time-.3)/7.2)),dt);
            if(time>=7.7&&!hoeWorkStarted)hoeWorkStarted=animals.beginHoeWork();
            if(hoeWorkStarted&&time<12.3)
            {
                // Follow the lowering bale along the clear east edge of the
                // stack; this short step has a visible task and tool contact.
                const float pull=glm::smoothstep(.12f,.85f,animals.hayWorkProgress());
                glm::vec3 working=FarmInteraction::HoeWorkPosition;
                working.z+=3.35f*pull;
                animals.moveManualWorker(working,dt);
                animals.stopManualWorker();
                animals.turnManualWorker(180.0f-animals.workers()[0].yaw);
            }
        }
        if(chapter==7)
        {
            const double workTime=time-2.0; // two seconds for the dusk aerial
            const array<glm::vec3,7> backToHoe={{herdingStart,{1.0f,0,3.0f},{5.5f,0,-1.0f},
                {7.0f,0,-5.0f},{11.0f,0,-7.45f},{11.0f,0,-9.25f},FarmInteraction::HoeStorePosition}};
            const array<glm::vec3,8> toCalves={{FarmInteraction::HoeStorePosition,{11.0f,0,-9.25f},
                {11.0f,0,-7.45f},{7.0f,0,-5.0f},{2.0f,0,-4.5f},{-1.5f,0,-5.5f},
                {-3.6f,0,-8.1f},{-4.30f,0,-11.25f}}};
            const array<glm::vec3,7> toBirds={{{-4.30f,0,-11.25f},{-2.5f,0,-6.0f},
                {0.0f,0,-2.0f},{2.5f,0,2.0f},{4.8f,0,4.2f},{6.3f,0,4.45f},{12.45f,0,5.0f}}};
            if(workTime>=.2&&workTime<7.4)moveDemonstrationWorker(pointOnPath(backToHoe,static_cast<float>((workTime-.2)/7.2)),dt);
            if(workTime>=7.6&&!hoeReturned)hoeReturned=animals.returnHoe();
            if(workTime>=8.0&&workTime<16.0)moveDemonstrationWorker(pointOnPath(toCalves,static_cast<float>((workTime-8.0)/8.0)),dt);
            if(workTime>=16.2&&!calvesRecalled){animals.sendCalvesHome();calvesRecalled=true;}
            if(workTime>=17.0&&workTime<23.0)moveDemonstrationWorker(pointOnPath(toBirds,static_cast<float>((workTime-17.0)/6.0)),dt);
            if(workTime>=23.2&&!poultryRecalled)
            {
                feedGoneBeforeReturn=animals.poultryFeedLevel()<=0.001f;
                animals.sendPoultryHome();poultryRecalled=true;
            }
        }
        if(chapter==8)
        {
            const array<glm::vec3,4> toTap={{waterStart,{7.0f,0,4.6f},{7.0f,0,4.0f},{7.10f,0,3.85f}}};
            const array<glm::vec3,6> toTrough={{{7.10f,0,3.85f},{4.5f,0,5.0f},{1.0f,0,4.0f},
                {-2.5f,0,1.0f},{-3.0f,0,-3.75f},FarmInteraction::cowTroughApproach(0)}};
            const array<glm::vec3,2> betweenTroughs={{FarmInteraction::cowTroughApproach(0),
                FarmInteraction::cowTroughApproach(1)}};
            const array<glm::vec3,6> backToTap={{FarmInteraction::WaterTroughInteraction,
                {-3.0f,0,-3.75f},{-2.5f,0,1.0f},{1.0f,0,4.0f},{4.5f,0,5.0f},{7.10f,0,3.85f}}};
            if(time>=.4&&time<3.0)moveDemonstrationWorker(pointOnPath(toTap,static_cast<float>((time-.4)/2.6)),dt);
            if(time>=3.1&&!waterValveTurned&&animalsReturned)
            {
                waterValveTurned=animals.toggleWaterValve();
                if(waterValveTurned)waterBeganAfterAnimalsReturned=animalsReturned;
            }
            if(time>=3.5&&!waterHoseTaken&&waterValveTurned)waterHoseTaken=animals.interactWithWorker()&&animals.workerHoldingHose();
            if(time>=3.8&&time<10.3&&waterHoseTaken)
                moveDemonstrationWorker(pointOnPath(toTrough,static_cast<float>((time-3.8)/6.5)),dt);
            if(time>=10.5&&!waterHosePlaced&&animals.workerHoldingHose())
                waterHosePlaced=animals.interactWithWorker()&&animals.hosePlacedAtTrough();
            if(time>=12.8&&!firstWaterRetrieved&&animals.hosePlacedAtTrough())
                firstWaterRetrieved=animals.interactWithWorker()&&animals.workerHoldingHose();
            if(time>=13.0&&time<15.2&&firstWaterRetrieved)
                moveDemonstrationWorker(pointOnPath(betweenTroughs,static_cast<float>((time-13.0)/2.2)),dt);
            if(time>=15.4&&!secondWaterPlaced&&animals.workerHoldingHose())
                secondWaterPlaced=animals.interactWithWorker()&&animals.hosePlacedAtTrough();
            if(time>=17.7&&!hoseRetrieved&&secondWaterPlaced&&animals.hosePlacedAtTrough())
                hoseRetrieved=animals.interactWithWorker()&&animals.workerHoldingHose();
            if(time>=18.0&&time<22.7&&hoseRetrieved)
                moveDemonstrationWorker(pointOnPath(backToTap,static_cast<float>((time-18.0)/4.7)),dt);
            if(time>=22.8&&!hoseReturned&&animals.workerHoldingHose())
                hoseReturned=animals.interactWithWorker()&&!animals.workerHoldingHose();
            if(time>=23.0&&animals.waterValveOpen())animals.toggleWaterValve();
        }
        if(chapter==9)
        {
            const array<glm::vec3,7> toHome={{homeStart,{7.0f,0,4.8f},{7.6f,0,5.0f},
                {6.5f,0,7.2f},{6.0f,0,9.6f},{8.0f,0,10.35f},{8.0f,.22f,12.70f}}};
            if(time>=.3&&time<8.8)moveDemonstrationWorker(pointOnPath(toHome,static_cast<float>((time-.3)/8.5)),dt);
            if(time>=9.0&&!workerSentHome){animals.sendWorkerHome();workerSentHome=true;}
        }
        if(chapter==10)
        {
            lighting.setDuskProgress(eased(static_cast<float>(time/DuskTransitionEnd)));
            if(time>=DuskTransitionEnd&&!lighting.nightMode())
            {
                lighting.toggleDayNight();
                animals.setNightMode(true);
            }
        }
        if(chapter==10&&time>=RainInspectionStart)
        {
            if(!inspectionStarted)
            {
                inspectionHome=animals.workers()[0].position;
                animals.beginRainInspection();inspectionStarted=true;
            }
            const array<glm::vec3,4> inspection={{inspectionHome,{8.0f,0,10.10f},
                {4.6f,0,9.8f},{4.6f,0,7.2f}}};
            if(time<RainInspectionArrive)moveDemonstrationWorker(pointOnPath(inspection,
                static_cast<float>((time-RainInspectionStart)/(RainInspectionArrive-RainInspectionStart))),dt);
            animals.setWorkerUmbrella(animals.workers()[0].position.x<5.6f&&animals.workers()[0].position.z<9.6f);
            umbrellaShown|=animals.workers()[0].holdingUmbrella;
            if(time>=RainInspectionArrive)animals.turnManualWorker(135.0f-animals.workers()[0].yaw);
        }
        if(chapter==11)
        {
            const array<glm::vec3,5> inspectionReturn={{homeStart,{4.6f,0,9.8f},{8.0f,0,9.8f},
                {8.0f,0,10.35f},{8.0f,.22f,12.7f}}};
            if(time<7.0)moveDemonstrationWorker(pointOnPath(inspectionReturn,static_cast<float>(time/7.0)),dt);
            if(time>=7.1&&!workerSentHome){animals.sendWorkerHome();workerSentHome=true;}
            const int requestedStage=time<8.0?0:(time<13.0?1:2);
            if(lightStage!=requestedStage)configureNightLights(requestedStage);
            completedLightShow|=requestedStage==2;
        }
        animals.update(dt);
        lighting.update(dt);
        gate = approach(gate, chapter >= 1 && chapter < 9 ? 90.0f : 0.0f, 60.0f*dt);
        stall = approach(stall, 0.0f, 60.0f*dt);
        calfDoor = approach(calfDoor, animals.calfShedDoorNeeded() ? 92.0f : 0.0f, 180.0f*dt);
        const Worker& worker = animals.workers()[0];
        sawPickup |= worker.state == WorkerState::PickingUpHay;
        sawCarry |= worker.carryingHay;
        sawFeed |= worker.state == WorkerState::Feeding;
        workerReturned |= chapter>=9&&worker.deliveredLoads>=2&&worker.state==WorkerState::HomeIdle;
        sawWaterFill |= animals.waterTroughFill(0)>.99f&&animals.waterTroughFill(1)>.99f;
        if(chapter>=7&&calvesRecalled&&poultryRecalled)
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
            // Oblique three-quarter aerial, matching the reference footage.
            return mixPose({{28.0f,23.0f,33.0f},{0,0,-1.5f},53},
                           {{21.0f,21.0f,32.0f},{0,0,-1.5f},53},u);
        }
        case 1:
            if(time<2.4)return mixPose({{-11.5f,4.6f,22.0f},{-10,3.5f,11.6f},43},
                                       {{-7.5f,4.0f,19.7f},{-10,3.5f,11.6f},41},static_cast<float>(time/2.4));
            if(time<4.4)return mixPose({{0,4.8f,24.0f},{0,4.7f,15.5f},38},
                                       {{0,3.7f,19.8f},{0,2.4f,15.5f},45},static_cast<float>((time-2.4)/2.0));
            return mixPose({{0,3.7f,19.8f},{0,1.8f,13.5f},45},
                           {{0,3.2f,8.0f},{0,1.2f,-1.0f},49},static_cast<float>((time-4.4)/5.6));
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
            if(time<6.2)
            {
                // Travel beside the farmer, then ease into the clear frontal
                // gate view as he arrives. This avoids the tank and tree line.
                const float arrival=glm::smoothstep(3.0f,5.2f,static_cast<float>(time));
                return {glm::mix(worker.position+glm::vec3(4.6f,3.4f,.8f),glm::vec3(6.0f,3.1f,1.2f),arrival),
                        glm::mix(worker.position+glm::vec3(0,1.1f,0),glm::vec3(10.3f,.65f,6.2f),arrival),52};
            }
            if(time<10.8)return {{3.0f,5.0f,-1.0f},worker.position+glm::vec3(0,1,0),50};
            if(time<14.2)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1.0f,0),44};
            if(time<22.2)return {{2.7f,5.3f,1.2f},worker.position+glm::vec3(0,1.0f,0),50};
            if(time<23.4)return {{2.2f,3.3f,10.0f},FarmInteraction::PoultryFeedPosition+glm::vec3(0,.55f,0),44};
            return {{2.7f,5.3f,1.2f},worker.position+glm::vec3(0,1,0),50};
        }
        case 4:
        {
            const Worker& worker=animals.workers()[0];
            if(time<3.5)return {{3.0f,5.2f,1.0f},worker.position+glm::vec3(0,1,0),50};
            if(time<7.2)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1,0),44};
            if(time<16.8)return {{0.3f,5.3f,-3.2f},worker.position+glm::vec3(0,1,0),48};
            const glm::vec3 calves=(animals.calves()[0].position+animals.calves()[1].position)*.5f;
            return {{-1.3f,3.6f,-13.6f},glm::mix(calves,glm::vec3(-5.6f,.8f,-9.65f),.35f),44};
        }
        case 5:
        {
            const Worker& worker=animals.workers()[0];
            if(time<7.7)return {{.8f,4.1f,6.9f},worker.position+glm::vec3(0,1,0),47};
            return {{-.5f,3.5f,7.3f},{-5.0f,1.1f,3.4f},43};
        }
        case 7:
        {
            const Worker& worker=animals.workers()[0];
            if(time<2.0)return mixPose({{26,22,31},{0,1,-1},53},{{23,21,29},{0,1,-1},53},static_cast<float>(time/2.0));
            if(time<9.8)return {{11.0f,2.8f,-5.4f},worker.position+glm::vec3(0,1,0),44};
            const glm::vec3 calves=(animals.calves()[0].position+animals.calves()[1].position)*.5f;
            glm::vec3 birds(0);
            int mobile=0;
            for(const Bird& bird:animals.birds())if(bird.mobile){birds+=bird.position;++mobile;}
            if(mobile>0)birds/=static_cast<float>(mobile);
            if(time<24.5)return {{-1.2f,4.0f,-13.8f},glm::mix(worker.position,calves,.55f)+glm::vec3(0,.7f,0),45};
            return {{6.0f,3.1f,1.2f},{10.3f,.65f,6.2f},52};
        }
        case 8:
        {
            const Worker& worker=animals.workers()[0];
            if(time<3.8)return {{4.3f,3.6f,6.8f},glm::mix(worker.position,FarmInteraction::ValvePosition,.35f)+glm::vec3(0,.75f,0),52};
            if(time<10.5)return {worker.position+glm::vec3(4.0f,3.2f,4.7f),worker.position+glm::vec3(0,1.0f,0),47};
            if(time<18.0)return {{-10.0f,4.2f,0.0f},{-10.0f,1.15f,-5.75f},62};
            return {worker.position+glm::vec3(4.0f,3.3f,4.8f),worker.position+glm::vec3(0,1.0f,0),47};
        }
        case 9:
        {
            const Worker& worker=animals.workers()[0];
            if(time<3.0)return {{4.2f,4.0f,9.0f},{8.5f,.8f,5.7f},46};
            return {{3.7f,3.7f,8.5f},worker.position+glm::vec3(0,1.0f,0),45};
        }
        case 6:
            // The smoothly curved metal vessel makes specular interpolation
            // easier to compare than the distant animal silhouettes.
            return {{5.2f,3.0f,3.0f},{8.3f,1.20f,-1.0f},38};
        case 10:
            if(time<DuskTransitionEnd)return mixPose({{28,21,32},{0,1,-1},52},{{18,19,31},{0,1,-1},52},static_cast<float>(time/DuskTransitionEnd));
            if(time<13.0)return mixPose({{-25,18,24},{0,1,-1},52},{{-19,17,26},{0,1,-1},52},static_cast<float>((time-DuskTransitionEnd)/5.0));
            if(time<NightRainStart)return mixPose({{16,15,28},{0,1,-1},52},{{8,14,27},{0,1,-1},52},static_cast<float>((time-13.0)/5.0));
            if(time<RainInspectionStart)return mixPose({{-1.3f,2.1f,28.0f},{0,2.0f,15.5f},51},
                                                       {{1.4f,1.8f,23.5f},{0,2.3f,15.5f},49},
                                                       static_cast<float>((time-NightRainStart)/(RainInspectionStart-NightRainStart)));
            if(time<32.0)
            {
                const Worker& worker=animals.workers()[0];
                return {{1.0f,3.0f,8.0f},worker.position+glm::vec3(0,1.4f,0),54};
            }
            return mixPose({{.7f,2.5f,13.0f},{4.6f,1.8f,7.2f},54},
                           {{-.7f,2.7f,11.0f},{4.6f,1.8f,7.2f},54},static_cast<float>((time-32.0)/8.0));
        default:
            if(time<8.0)return mixPose({{-18,18,28},{0,1,-1},52},{{-10,17,28},{0,1,-1},52},static_cast<float>(time/8.0));
            if(time<13.0)return {{-1.0f,4.0f,-.8f},{-10,1.8f,-8},49};
            if(time<17.0)return {{0,5.3f,24},{0,4.7f,15.5f},43};
            return mixPose({{18,19,29},{0,1,-1},52},{{23,20,29},{0,1,-1},52},static_cast<float>((time-17.0)/3.0));
        }
    }
    bool rainActive() const { return chapter==10&&time>=NightRainStart; }
    int cameraShot() const
    {
        if(chapter==1)return static_cast<int>(chapter)*10+(time<2.4?0:(time<4.4?1:2));
        if(chapter==2)return 20+(time<7.2?0:(time<14.0?1:(time<20.7?2:3)));
        if(chapter==3)return 30+(time<6.2?0:(time<10.8?1:(time<14.2?2:(time<22.2?3:(time<23.4?4:5)))));
        if(chapter==4)return 40+(time<3.5?0:(time<7.2?1:(time<16.8?2:3)));
        if(chapter==7)return 70+(time<2.0?0:(time<9.8?1:(time<24.5?2:3)));
        if(chapter==8)return 80+(time<3.8?0:(time<10.5?1:(time<18.0?2:3)));
        if(chapter==9)return 90+(time<3.0?0:1);
        if(chapter==5)return 50+(time<7.7?0:1);
        if(chapter==10)return 100+(time<DuskTransitionEnd?0:(time<13.0?1:(time<NightRainStart?2:(time<RainInspectionStart?3:(time<32.0?4:5)))));
        if(chapter==11)return 110+(time<8.0?0:(time<13.0?1:(time<17.0?2:3)));
        return static_cast<int>(chapter)*10;
    }
private:
    void configureNightLights(int stage)
    {
        lightStage=stage;
        const bool shed=stage==0;
        const bool fence=true;
        const bool banner=stage<2; // entrance nameplate
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
        glm::vec3 clearNext=next;
        // Keep a body-sized clearance around roaming calves. The farmer
        // bends his route around them instead of walking through a paused calf.
        for(const Calf& calf:animals.calves())
        {
            glm::vec2 away(clearNext.x-calf.position.x,clearNext.z-calf.position.z);
            const float distance=glm::length(away);
            if(distance<1.08f)
            {
                if(distance<.001f)away=glm::vec2(0,1);else away/=distance;
                clearNext.x=calf.position.x+away.x*1.08f;
                clearNext.z=calf.position.z+away.y*1.08f;
            }
        }
        for(const Bird& bird:animals.birds())if(bird.mobile)
        {
            glm::vec2 away(clearNext.x-bird.position.x,clearNext.z-bird.position.z);
            const float distance=glm::length(away);
            if(distance<.75f)
            {
                if(distance<.001f)away=glm::vec2(-1,0);else away/=distance;
                clearNext.x=bird.position.x+away.x*.75f;
                clearNext.z=bird.position.z+away.y*.75f;
            }
        }
        const glm::vec2 difference(clearNext.x-worker.position.x,clearNext.z-worker.position.z);
        if(glm::length(difference)>.0001f)
        {
            const float target=glm::degrees(atan2(-difference.y,difference.x));
            float turn=fmod(target-worker.yaw,360.0f);
            if(turn>180)turn-=360;if(turn<-180)turn+=360;
            animals.turnManualWorker(turn);
        }
        animals.moveManualWorker(clearNext,dt);
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
        text(string("AMS AGRO FARM")+
             (controls.paused?" / PAUSED":""),margin+16*s,margin+12*s,1.5f*s,muted);
        text(to_string(tour.chapter+1)+"/"+to_string(Chapters.size())+"  "+Chapters[tour.chapter].name,margin+16*s,margin+36*s,2.1f*s,white);
        text(Chapters[tour.chapter].caption,margin+16*s,margin+72*s,1.25f*s,gold);

        const float technicalWidth=286*s;
        const float technicalX=width-margin-technicalWidth;
        rectangle(technicalX,margin,technicalWidth,100*s,glm::vec4(.018f,.055f,.048f,.95f));
        rectangle(technicalX,margin,3*s,100*s,muted);
        const string timeName=tour.lighting.nightMode()?"NIGHT 22:00":
            (tour.chapter==10?"DUSK 18:30":(tour.chapter==6?"DAYLIGHT 12:30":
             (tour.lighting.afternoonMode()?"AFTERNOON 16:30":"MORNING 10:30")));
        string renderName="PHONG / TEXTURED";
        if(tour.chapter==6)renderName="FLAT / GOURAUD / PHONG";
        if(tour.rainActive())renderName="RAY TRACED ROAD";
        string cameraName=tour.chapter==0?"MORNING AERIAL":(tour.chapter==1?"BANNER / GATE DOLLY":
            (tour.chapter==6?"THREE PANEL":(tour.rainActive()?"WET ANALYSIS":"CINEMATIC FOLLOW")));
        if(tour.chapter==7&&tour.time<2.0)cameraName="EVENING AERIAL";
        if(tour.chapter==10&&tour.time<NightRainStart)cameraName="ALL LIGHTS AERIAL";
        if(tour.chapter==10&&tour.time<DuskTransitionEnd)cameraName="AFTERNOON TO NIGHT AERIAL";
        if(tour.chapter==10&&tour.time>=RainInspectionStart)cameraName="UMBRELLA INSPECTION";
        if(tour.chapter==3&&tour.time<5.2)cameraName="FARMER TO POULTRY GATE";
        text("TIME  "+timeName,technicalX+13*s,margin+10*s,1.05f*s,white);
        if(tour.rainActive())
        {
            text("PIPELINE  HYBRID SCREEN RAYS",technicalX+13*s,margin+29*s,.92f*s,gold);
            text("REFLECTION  ON / 48 RAY STEPS",technicalX+13*s,margin+48*s,.92f*s,white);
            text("COLOUR  ORIGINAL FARM LIGHTS",technicalX+13*s,margin+67*s,.92f*s,white);
            text("FALLBACK  TEXTURED SCENE CAPTURE",technicalX+13*s,margin+86*s,.82f*s,muted);
        }
        else
        {
            text("RENDER  "+renderName,technicalX+13*s,margin+29*s,1.05f*s,gold);
            text("CAMERA  "+cameraName,technicalX+13*s,margin+48*s,1.05f*s,muted);
            text("LIGHTS  SUN POINT SPOT",technicalX+13*s,margin+67*s,1.05f*s,white);
            text("TRANSFORMS  MODEL VIEW PROJECTION",technicalX+13*s,margin+86*s,.78f*s,muted);
        }

        if(tour.chapter==6)
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
            rectangle(margin,margin+112*s,268*s,25*s,panel);
            text("WET ROAD / RAY TRACED REFLECTIONS",margin+12*s,margin+119*s,1.12f*s,gold);
        }
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
                glm::vec3 up=glm::vec3(0,1,0),bool flatShading=false,bool reflectionCapture=false)
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
    shader.setBool("reflectionCapture",reflectionCapture);
    const Worker& worker=tour.animals.workers()[0];
    farm.render(shader,tour.gate,tour.stall,tour.calfDoor,tour.animals.poultryDoorAngle(),
                tour.animals.fanAngle(),tour.lighting,
                tour.animals.workerDoorAngle(),tour.animals.hayAvailableMask(),
                tour.animals.visibleCowHayLoads(),tour.animals.waterValveOpen(),
                tour.animals.waterValveAngle(),tour.animals.workerHoldingHose(),
                tour.animals.hosePlacedAtTrough(),tour.animals.hoseEndPosition(),
                tour.animals.waterTroughFill(0),tour.animals.feedBagAvailable(),
                tour.animals.hoeAvailable(),tour.animals.waterTroughFill(1),
                tour.animals.hayWorkProgress());
    entities.render(shader,tour.animals);curves.render(shader,textures);
}

void render(const Tour& tour,const Pose& pose,int width,int height,const Shader& phong,const Shader& gouraud,
            const FarmScene& farm,const EntityRenderer& entities,const CurvedRenderer& curves,
            const FarmTextures& textures,Overlay& overlay,const Controls& controls,
            WetReflectionRenderer& wet)
{
    if(width<=0||height<=0)return;
    const glm::vec3 sky=tour.lighting.clearColor();glClearColor(sky.r,sky.g,sky.b,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(tour.chapter==6)
    {
        const int first=width/3,second=width/3,third=width-first-second;
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,0,first,height,
                   false,19.5f,glm::vec3(0,1,0),true);
        rasterView(gouraud,tour,farm,entities,curves,textures,pose,first,0,second,height);
        rasterView(phong,tour,farm,entities,curves,textures,pose,first+second,0,third,height);
    }
    else if(tour.rainActive())
    {
        // Preserve the real farm textures/materials and practical lights.
        // Reflections trace depth-buffer rays with a full-scene planar fallback.
        wet.resize(width,height);
        const float aspect=static_cast<float>(width)/height;
        const glm::mat4 view=glm::lookAt(pose.eye,pose.target,glm::vec3(0,1,0));
        const glm::mat4 projection=glm::perspective(glm::radians(pose.fov),aspect,.1f,TerrainRenderer::FarPlane);
        Pose reflected=pose;
        constexpr float roadPlane=.04f;
        reflected.eye.y=2*roadPlane-pose.eye.y;
        reflected.target.y=2*roadPlane-pose.target.y;
        const glm::mat4 reflectedView=glm::lookAt(reflected.eye,reflected.target,glm::vec3(0,-1,0));
        glBindFramebuffer(GL_FRAMEBUFFER,wet.reflectionFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        rasterView(phong,tour,farm,entities,curves,textures,reflected,0,0,width,height,
                   false,19.5f,glm::vec3(0,-1,0),false,true);
        glBindFramebuffer(GL_FRAMEBUFFER,wet.sceneFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        rasterView(phong,tour,farm,entities,curves,textures,pose,0,0,width,height);
        wet.compose(view,projection,reflectedView,pose.eye,static_cast<float>(tour.elapsed),
                    eased(static_cast<float>(tour.time-NightRainStart)));
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
        Shader phong("shaders/texture.vs","shaders/texture.fs"),gouraud("shaders/gouraud.vs","shaders/gouraud.fs");
        TextureManager textures;CubeRenderer cubes;PrimitiveRenderer primitives;CurvedRenderer curves;
        FarmScene farm(cubes,primitives,textures.farm());EntityRenderer entities(cubes,primitives);
        WetReflectionRenderer wet;
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
                render(tour,smoothed,width,height,phong,gouraud,farm,entities,curves,textures.farm(),overlay,controls,wet);
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
                   !tour.firstWaterRetrieved||!tour.secondWaterPlaced||
                   tour.animals.hayWorkProgress()<.99f||tour.animals.visibleCowHayLoads()!=0||
                   !tour.poultryReleased||!tour.feedBagTaken||!tour.poultryFed||!tour.feedBoxReturned||
                   tour.animals.deliveredHayLoads()!=2||!tour.calvesReleased||
                   !tour.hoeTaken||!tour.hoeWorkStarted||!tour.hoeReturned||
                   !tour.calvesRecalled||!tour.poultryRecalled||!tour.animalsReturned||
                   !tour.waterBeganAfterAnimalsReturned||!tour.feedGoneBeforeReturn||
                   !tour.workerReturned||!tour.completedLightShow||!tour.umbrellaShown||
                   tour.animals.workers()[0].state!=WorkerState::HomeIdle)
                {
                    cerr<<"Verification: hay="<<tour.animals.deliveredHayLoads()
                        <<" water="<<tour.animals.waterTroughFill(0)<<','<<tour.animals.waterTroughFill(1)
                        <<" water-after-home="<<tour.waterBeganAfterAnimalsReturned
                        <<" animals-home="<<tour.animalsReturned
                        <<" hoe="<<tour.hoeTaken<<tour.hoeWorkStarted<<tour.hoeReturned
                        <<" hay-work="<<tour.animals.hayWorkProgress()
                        <<" poultry="<<tour.poultryReleased<<tour.feedBagTaken<<tour.poultryFed
                        <<" feed-gone="<<tour.feedGoneBeforeReturn
                        <<" farmer-home="<<tour.workerReturned<<'\n';
                    throw runtime_error("240-second simulation missed a required workday or graphics-showcase action.");
                }
                cout<<"Simulation test passed: exactly 240 seconds - sheltered poultry start, water and hose return, "
                       "feed collection, two hay loads, calf release, hoe work, all returns, three shading modes, "
                       "night lighting and the ray-traced rain finale.\n";
                glfwSetWindowShouldClose(window,true);
            }
        }
    }
    catch(const exception& error){cerr<<error.what()<<'\n';result=4;}
    glfwDestroyWindow(window);glfwTerminate();return result;
}
