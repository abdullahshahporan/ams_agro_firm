#ifndef AMS_AGRO_FARM_ANIMATION_SYSTEM_H
#define AMS_AGRO_FARM_ANIMATION_SYSTEM_H

#include <array>
#include <glm/glm.hpp>

#include "animals/cow.h"
#include "animals/calf.h"
#include "animals/poultry.h"
#include "animals/farmer.h"
#include "shared/farm_interaction.h"

using namespace std;

class AnimationSystem
{
public:
    AnimationSystem();

    void update(float deltaTime);
    void setNightMode(bool nightMode);
    void toggleAnimalShelter();
    void sendCalvesHome();
    void sendPoultryHome();
    void preparePoultrySheltered();
    void sendPoultryOut();
    void toggleCalfShed();
    void sendCalvesToFeed();
    void commandWorker();
    void sendWorkerHome();
    void toggleManualWorker();
    bool interactWithWorker();
    bool toggleWaterValve();
    bool scatterPoultryFeed();
    bool pickupFeedBag();
    bool returnFeedBox();
    void beginRainInspection();
    void setWorkerUmbrella(bool open) { workers_[0].holdingUmbrella=open; }
    bool pickupHoe();
    bool beginHoeWork();
    bool returnHoe();
    void stopWaterFlow() { waterValveOpen_=false; }
    void turnManualWorker(float angleDegrees);
    void moveManualWorker(const glm::vec3& position, float deltaTime);
    void toggleManualCalf();
    void selectNextCalf();
    void driveManualCalf(float forward, float turn, float deltaTime,
                         float gate, float stall, float calfDoor);
    bool manualCalfControl() const { return manualCalfControl_; }
    size_t selectedCalfIndex() const { return selectedCalf_; }
    bool restockHay();
    unsigned int hayAvailableMask() const;
    int hayRemaining() const;
    bool waterValveOpen() const { return waterValveOpen_; }
    float waterValveAngle() const { return waterValveAngle_; }
    bool workerHoldingHose() const { return workers_[0].holdingHose; }
    bool hosePlacedAtTrough() const { return hosePlacedAtTrough_; }
    const glm::vec3& hoseEndPosition() const { return hoseEndPosition_; }
    float waterTroughFill() const { return waterTroughFills_[activeWaterCow_]; }
    float waterTroughFill(int cow) const { return waterTroughFills_[cow==0?0:1]; }
    int visibleCowHayLoads() const { return visibleCowHayLoads_; }
    void consumeCowHay() { visibleCowHayLoads_=0; }
    float hayWorkProgress() const { return hayWorkProgress_; }
    const char* waterStatus() const;
    float poultryFeedLevel() const { return poultryFeedLevel_; }
    bool feedBagAvailable() const { return feedBagAvailable_; }
    bool hoeAvailable() const { return hoeAvailable_; }
    bool workerHoldingFeedBag() const { return workers_[0].holdingFeedBag; }
    bool workerHoldingHoe() const { return workers_[0].holdingHoe; }
    bool workerDigging() const { return workers_[0].digging; }
    float workerDoorAngle() const { return workerDoorAngle_; }
    float poultryDoorAngle() const { return poultryDoorAngle_; }
    void stopManualWorker() { workers_[0].manualWalking = false; }

    void toggleAdultCows() { adultCowsOn_ = !adultCowsOn_; }
    void toggleCalves() { calvesOn_ = !calvesOn_; }
    void toggleHeadMotion() { headMotionOn_ = !headMotionOn_; }
    void toggleFans() { fansOn_ = !fansOn_; }

    bool adultCowsOn() const { return adultCowsOn_; }
    bool calvesOn() const { return calvesOn_; }
    bool headMotionOn() const { return headMotionOn_; }
    bool workersOn() const { return workers_[0].state != WorkerState::HomeIdle; }
    bool manualWorkerControl() const { return manualWorkerControl_; }
    bool workerDoorOpen() const { return abs(workerDoorAngle_) >= 85.0f; }
    int deliveredHayLoads() const { return workers_[0].deliveredLoads; }
    bool fansOn() const { return fansOn_; }
    float fanAngle() const { return fanAngle_; }
    bool nightMode() const { return nightMode_; }
    bool animalShelterRequested() const { return shelterRequested_ || nightMode_; }
    bool calfShedDoorNeeded() const;
    const char* calfStatus() const;
    const char* workerStatus() const;

    const array<Cow, 2>& cows() const { return cows_; }
    const array<Calf, 2>& calves() const { return calves_; }
    const array<Worker, 1>& workers() const { return workers_; }
    const array<Bird, 7>& birds() const { return birds_; }

private:
    // Each item owns its initialization and update code in src/animals/.
    void initializeCows();
    void initializeCalves();
    void initializeWorkers();
    void initializeBirds();
    void updateCows(float deltaTime);
    void updateCalves(float deltaTime);
    void updateBirds(float deltaTime);
    void updateWorkers(float deltaTime);
    void updateFans(float deltaTime);

    static void updateCowPatrol(Cow& cow, float deltaTime);
    static void updateCalfPath(Calf& calf, float deltaTime);
    static void updateCalfNavigation(Calf& calf, float deltaTime,
                                     CalfDestination destination);
    void updateWorker(Worker& worker, float deltaTime);
    bool beginHayPickup(Worker& worker, int index);
    void returnUndeliveredHay(Worker& worker);
    void resumeCalf(Calf& calf);
    static void updateBirdPath(Bird& bird, float deltaTime);
    static void updateBirdNavigation(Bird& bird, float deltaTime, bool shelterRequested);
    static bool moveToward(glm::vec3& position, float& yaw, const glm::vec3& target,
                           float speed, float deltaTime);
    static float yawForDirection(const glm::vec3& direction);

    array<Cow, 2> cows_;
    array<Calf, 2> calves_;
    array<Worker, 1> workers_;
    array<Bird, 7> birds_;

    bool adultCowsOn_{true};
    bool calvesOn_{true};
    bool headMotionOn_{true};
    bool fansOn_{true};
    bool nightMode_{false};
    bool shelterRequested_{false};
    bool manualWorkerControl_{false};
    CalfDestination calfDestination_{CalfDestination::Home};
    float fanAngle_{0.0f};
    float workerDoorAngle_{0.0f};
    float poultryDoorAngle_{0.0f};
    array<bool,FarmInteraction::HayCount> hayAvailable_{{true,true,true}};
    bool manualCalfControl_{false};
    size_t selectedCalf_{0};
    bool waterValveOpen_{false};
    float waterValveAngle_{0.0f};
    bool hosePlacedAtTrough_{false};
    glm::vec3 hoseEndPosition_{FarmInteraction::HoseRest};
    array<float,2> waterTroughFills_{{0.0f,0.0f}};
    int activeWaterCow_{1};
    int visibleCowHayLoads_{0};
    float hayWorkProgress_{0.0f};
    float poultryFeedLevel_{0.0f};
    bool feedBagAvailable_{true};
    bool hoeAvailable_{true};
};

#endif
