#ifndef AMS_AGRO_FARM_ANIMATION_SYSTEM_H
#define AMS_AGRO_FARM_ANIMATION_SYSTEM_H

#include <array>
#include <glm/glm.hpp>

enum class CowState
{
    Standing,
    Walking,
    Grazing,
    Feeding
};

enum class CattleRole
{
    MilchCow,
    Ox
};

struct Cow
{
    glm::vec3 position;
    float yaw;
    float scale;
    CowState state;
    float speed;
    float animationTime;
    float headAnimationTime;
    float phase;
    glm::vec3 pathA;
    glm::vec3 pathB;
    bool targetingB;
    glm::vec3 bodyColor;
    glm::vec3 patchColor;
    float hornScale;
    bool tied;
    glm::vec3 tetherAnchor;
    CattleRole role;
};

struct Calf
{
    glm::vec3 position;
    float yaw;
    float scale;
    float speed;
    float animationTime;
    float pathAngle;
    glm::vec3 pathCenter;
    glm::vec2 pathRadius;
    float direction;
    glm::vec3 bodyColor;
    glm::vec3 patchColor;
    glm::vec3 shelterPosition;
    glm::vec3 feedingPosition;
    enum class RouteState
    {
        Roaming,
        GoingToShelter,
        Sheltered,
        GoingToField,
        GoingToFeed,
        Feeding
    } routeState;
};

enum class CalfDestination
{
    Home,
    Field,
    FeedingArea
};

enum class BirdSpecies
{
    Chicken,
    Duck
};

struct Bird
{
    BirdSpecies species;
    glm::vec3 position;
    glm::vec3 shelterPosition;
    glm::vec3 pathCenter;
    glm::vec2 pathRadius;
    float pathAngle;
    float direction;
    float speed;
    float scale;
    float yaw;
    float animationTime;
    bool mobile;
    bool juvenile;
    glm::vec3 color;
    Calf::RouteState routeState;
};

enum class WorkerState
{
    HomeIdle,
    WalkingToCows,
    ReadyAtCows,
    Feeding,
    WalkingHome
};

struct Worker
{
    glm::vec3 position;
    float yaw;
    WorkerState state;
    float speed;
    float animationTime;
    float phase;
    glm::vec3 pathA;
    glm::vec3 pathB;
    bool targetingB;
    glm::vec3 shirtColor;
};

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

    void toggleAdultCows() { adultCowsOn_ = !adultCowsOn_; }
    void toggleCalves() { calvesOn_ = !calvesOn_; }
    void toggleHeadMotion() { headMotionOn_ = !headMotionOn_; }
    void toggleFans() { fansOn_ = !fansOn_; }

    bool adultCowsOn() const { return adultCowsOn_; }
    bool calvesOn() const { return calvesOn_; }
    bool headMotionOn() const { return headMotionOn_; }
    bool workersOn() const { return workers_[0].state != WorkerState::HomeIdle; }
    bool fansOn() const { return fansOn_; }
    float fanAngle() const { return fanAngle_; }
    bool nightMode() const { return nightMode_; }
    bool animalShelterRequested() const { return shelterRequested_ || nightMode_; }
    bool calfShedDoorNeeded() const;
    const char* calfStatus() const;
    const char* workerStatus() const;

    const std::array<Cow, 2>& cows() const { return cows_; }
    const std::array<Calf, 2>& calves() const { return calves_; }
    const std::array<Worker, 1>& workers() const { return workers_; }
    const std::array<Bird, 7>& birds() const { return birds_; }

private:
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

    std::array<Cow, 2> cows_;
    std::array<Calf, 2> calves_;
    std::array<Worker, 1> workers_;
    std::array<Bird, 7> birds_;

    bool adultCowsOn_{true};
    bool calvesOn_{true};
    bool headMotionOn_{true};
    bool fansOn_{true};
    bool nightMode_{false};
    bool shelterRequested_{false};
    CalfDestination calfDestination_{CalfDestination::Home};
    float fanAngle_{0.0f};
};

#endif
