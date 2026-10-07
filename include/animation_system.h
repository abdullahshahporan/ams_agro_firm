#ifndef AMS_AGRO_FARM_ANIMATION_SYSTEM_H
#define AMS_AGRO_FARM_ANIMATION_SYSTEM_H

#include <array>
#include <glm/glm.hpp>

#include "animals/cow.h"
#include "animals/calf.h"
#include "animals/poultry.h"
#include "animals/farmer.h"

using namespace std;

class AnimationSystem
{
public:
    AnimationSystem();

    void update(float deltaTime);
    void setNightMode(bool nightMode);
    void toggleAnimalShelter();
    void toggleCalfShed();
    void sendCalvesToFeed();
    void commandWorker();
    void sendWorkerHome();
    void toggleManualWorker();
    bool interactWithWorker();
    void turnManualWorker(float angleDegrees);
    void moveManualWorker(const glm::vec3& position, float deltaTime);
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
    bool workerDoorOpen() const { return workers_[0].state != WorkerState::HomeIdle; }
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
    static void updateWorker(Worker& worker, float deltaTime);
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
};

#endif
