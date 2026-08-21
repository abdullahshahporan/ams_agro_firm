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
};

enum class WorkerState
{
    Walking,
    Working
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

    void toggleAdultCows() { adultCowsOn_ = !adultCowsOn_; }
    void toggleCalves() { calvesOn_ = !calvesOn_; }
    void toggleHeadMotion() { headMotionOn_ = !headMotionOn_; }
    void toggleWorkers() { workersOn_ = !workersOn_; }
    void toggleFans() { fansOn_ = !fansOn_; }

    bool adultCowsOn() const { return adultCowsOn_; }
    bool calvesOn() const { return calvesOn_; }
    bool headMotionOn() const { return headMotionOn_; }
    bool workersOn() const { return workersOn_; }
    bool fansOn() const { return fansOn_; }
    float fanAngle() const { return fanAngle_; }

    const std::array<Cow, 4>& cows() const { return cows_; }
    const std::array<Calf, 2>& calves() const { return calves_; }
    const std::array<Worker, 2>& workers() const { return workers_; }

private:
    static void updateCowPatrol(Cow& cow, float deltaTime);
    static void updateCalfPath(Calf& calf, float deltaTime);
    static void updateWorker(Worker& worker, float deltaTime);
    static float yawForDirection(const glm::vec3& direction);

    std::array<Cow, 4> cows_;
    std::array<Calf, 2> calves_;
    std::array<Worker, 2> workers_;

    bool adultCowsOn_{true};
    bool calvesOn_{true};
    bool headMotionOn_{true};
    bool workersOn_{true};
    bool fansOn_{true};
    float fanAngle_{0.0f};
};

#endif
