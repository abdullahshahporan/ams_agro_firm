#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "animation_system.h"
#include "collision_system.h"
#include "cube_renderer.h"
#include "curved_renderer.h"
#include "entity_renderer.h"
#include "farm_scene.h"
#include "lighting_system.h"
#include "primitive_renderer.h"
#include "shader.h"
#include "texture_manager.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
constexpr unsigned int InitialWidth = 1200;
constexpr unsigned int InitialHeight = 800;
constexpr float GateSpeedDegreesPerSecond = 60.0f;

Camera camera(glm::vec3(0.0f, 3.0f, 25.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = InitialWidth * 0.5f;
float lastMouseY = InitialHeight * 0.5f;
bool firstMouseEvent = true;

bool gateShouldBeOpen = false;
float gateAngleDegrees = 0.0f;
bool stallGatesShouldBeOpen = false;
float stallGateAngleDegrees = 0.0f;
float calfShedDoorAngleDegrees = 0.0f;
bool texturesEnabled = true;
bool birdEyeMode = false;
bool fourViewMode = false;
bool mouseCaptured = true;
AnimationSystem animationSystem;
LightingSystem lightingSystem;

int framebufferWidth = static_cast<int>(InitialWidth);
int framebufferHeight = static_cast<int>(InitialHeight);

void printControls();

const char* onOff(bool enabled)
{
    return enabled ? "ON" : "OFF";
}

void updateWindowTitle(GLFWwindow* window)
{
    if (window == nullptr)
        return;

    std::string title = "AMS Agro Farm | ";
    title += lightingSystem.nightMode() ? "NIGHT" : "DAY";
    title += fourViewMode ? " | FOUR VIEW" : (birdEyeMode ? " | BIRD'S-EYE" : " | FREE VIEW");
    title += " | Shed ";
    title += onOff(lightingSystem.shedLightsEnabled());
    title += " | Fence ";
    title += onOff(lightingSystem.fenceLightsEnabled());
    title += " | Banner ";
    title += onOff(lightingSystem.bannerLightEnabled());
    title += " | Billboard ";
    title += onOff(lightingSystem.billboardLightEnabled());
    title += " | Spot ";
    title += onOff(lightingSystem.spotlightEnabled());
    glfwSetWindowTitle(window, title.c_str());
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    // GLFW_PRESS fires once per physical press; holding G cannot repeatedly toggle.
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        gateShouldBeOpen = !gateShouldBeOpen;
        std::cout << "Gate: " << (gateShouldBeOpen ? "opening" : "closing") << '\n';
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        stallGatesShouldBeOpen = !stallGatesShouldBeOpen;
        std::cout << "Cattle stall gates: "
                  << (stallGatesShouldBeOpen ? "opening" : "closing") << '\n';
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        texturesEnabled = !texturesEnabled;
        std::cout << "Textures: " << (texturesEnabled ? "ON" : "OFF") << '\n';
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        animationSystem.toggleAdultCows();
        std::cout << "Adult cow movement: " << (animationSystem.adultCowsOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        animationSystem.toggleCalves();
        std::cout << "Calf motion: " << (animationSystem.calvesOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        animationSystem.toggleHeadMotion();
        std::cout << "Cow head/grazing motion: " << (animationSystem.headMotionOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        if (lightingSystem.nightMode())
            std::cout << "Worker: night routine active; only the Home command is available.\n";
        else
        {
            animationSystem.commandWorker();
            std::cout << "Worker: " << animationSystem.workerStatus() << '\n';
        }
    }
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        animationSystem.sendWorkerHome();
        std::cout << "Worker: " << animationSystem.workerStatus() << '\n';
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (lightingSystem.nightMode())
            std::cout << "Mobile animals: night routine keeps every animal sheltered.\n";
        else
        {
            animationSystem.toggleAnimalShelter();
            std::cout << "Mobile animals: "
                      << (animationSystem.animalShelterRequested()
                          ? "returning/staying in shelters" : "released to daytime fields")
                      << '\n';
        }
    }
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
    {
        if (lightingSystem.nightMode())
            std::cout << "Calves: release is unavailable during the night routine.\n";
        else
        {
            animationSystem.toggleCalfShed();
            std::cout << "Calves: " << animationSystem.calfStatus() << '\n';
        }
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        if (lightingSystem.nightMode())
            std::cout << "Calves: feeding command is unavailable during the night routine.\n";
        else
        {
            animationSystem.sendCalvesToFeed();
            std::cout << "Calves: " << animationSystem.calfStatus() << '\n';
        }
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        animationSystem.toggleFans();
        std::cout << "Shed fans: " << (animationSystem.fansOn() ? "ON" : "PAUSED") << '\n';
    }

    if ((key == GLFW_KEY_1 || key == GLFW_KEY_KP_1) && action == GLFW_PRESS)
    {
        lightingSystem.toggleDirectional();
        std::cout << "Directional light: " << (lightingSystem.directionalEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_2 || key == GLFW_KEY_KP_2) && action == GLFW_PRESS)
    {
        lightingSystem.togglePointLights();
        std::cout << "Point lights: " << (lightingSystem.pointLightsEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_8 || key == GLFW_KEY_KP_8) && action == GLFW_PRESS)
    {
        lightingSystem.toggleShedLights();
        std::cout << "Shed/building lights: "
                  << (lightingSystem.shedLightsEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_9 || key == GLFW_KEY_KP_9) && action == GLFW_PRESS)
    {
        lightingSystem.toggleFenceLights();
        std::cout << "Fence lamps: "
                  << (lightingSystem.fenceLightsEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_0 || key == GLFW_KEY_KP_0) && action == GLFW_PRESS)
    {
        lightingSystem.toggleBannerLight();
        std::cout << "Entrance banner light: "
                  << (lightingSystem.bannerLightEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        lightingSystem.toggleBillboardLight();
        std::cout << "Owner billboard light: "
                  << (lightingSystem.billboardLightEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_3 || key == GLFW_KEY_KP_3) && action == GLFW_PRESS)
    {
        lightingSystem.toggleSpotlight();
        std::cout << "Entrance spotlight: " << (lightingSystem.spotlightEnabled() ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if ((key == GLFW_KEY_4 || key == GLFW_KEY_KP_4) && action == GLFW_PRESS)
    {
        lightingSystem.toggleDayNight();
        animationSystem.setNightMode(lightingSystem.nightMode());
        updateWindowTitle(window);
        std::cout << "Time of day: " << (lightingSystem.nightMode() ? "NIGHT" : "DAY")
                  << " | Shed: " << (lightingSystem.shedLightsEnabled() ? "ON" : "OFF")
                  << " | Fence: " << (lightingSystem.fenceLightsEnabled() ? "ON" : "OFF")
                  << " | Banner: " << (lightingSystem.bannerLightEnabled() ? "ON" : "OFF")
                  << " | Billboard: " << (lightingSystem.billboardLightEnabled() ? "ON" : "OFF")
                  << " | Spot: " << (lightingSystem.spotlightEnabled() ? "ON" : "OFF")
                  << '\n';
    }
    if ((key == GLFW_KEY_5 || key == GLFW_KEY_KP_5) && action == GLFW_PRESS)
    {
        lightingSystem.toggleAmbient();
        std::cout << "Ambient component: " << (lightingSystem.ambientEnabled() ? "ON" : "OFF") << '\n';
    }
    if ((key == GLFW_KEY_6 || key == GLFW_KEY_KP_6) && action == GLFW_PRESS)
    {
        lightingSystem.toggleDiffuse();
        std::cout << "Diffuse component: " << (lightingSystem.diffuseEnabled() ? "ON" : "OFF") << '\n';
    }
    if ((key == GLFW_KEY_7 || key == GLFW_KEY_KP_7) && action == GLFW_PRESS)
    {
        lightingSystem.toggleSpecular();
        std::cout << "Specular component: " << (lightingSystem.specularEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        birdEyeMode = !birdEyeMode;
        if (birdEyeMode)
            fourViewMode = false;
        firstMouseEvent = true;
        std::cout << "Bird's-eye view: " << (birdEyeMode ? "ON" : "OFF") << '\n';
        updateWindowTitle(window);
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        fourViewMode = !fourViewMode;
        if (fourViewMode)
            birdEyeMode = false;
        firstMouseEvent = true;
        std::cout << "Four-view mode: " << (fourViewMode ? "ON" : "OFF") << '\n';
        if (fourViewMode)
            std::cout << "Four-view layout: top-left Free | top-right Top | "
                         "bottom-left Front | bottom-right Side\n";
        updateWindowTitle(window);
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        mouseCaptured = !mouseCaptured;
        glfwSetInputMode(window, GLFW_CURSOR,
                         mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        firstMouseEvent = true;
        std::cout << "Mouse capture: " << (mouseCaptured ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        printControls();
    }
}

void mouseCallback(GLFWwindow*, double xPosition, double yPosition)
{
    if (!mouseCaptured || birdEyeMode || fourViewMode)
    {
        firstMouseEvent = true;
        return;
    }
    if (firstMouseEvent)
    {
        lastMouseX = static_cast<float>(xPosition);
        lastMouseY = static_cast<float>(yPosition);
        firstMouseEvent = false;
    }

    const float xOffset = static_cast<float>(xPosition) - lastMouseX;
    const float yOffset = lastMouseY - static_cast<float>(yPosition);
    lastMouseX = static_cast<float>(xPosition);
    lastMouseY = static_cast<float>(yPosition);

    camera.processMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow*, double, double yOffset)
{
    if (mouseCaptured && !birdEyeMode && !fourViewMode)
        camera.processMouseScroll(static_cast<float>(yOffset));
}

void tryCameraMovement(CameraMovement movement)
{
    const glm::vec3 previous = camera.Position;
    camera.processKeyboard(movement, deltaTime);
    const glm::vec3 requested = camera.Position;
    camera.Position = previous;

    // Sweep in short steps so a low-frame-rate movement cannot tunnel through
    // thin gates, fence rails, walls, or stall doors.
    const float distance = glm::length(requested - previous);
    const int steps = std::max(1, static_cast<int>(std::ceil(distance / 0.10f)));
    for (int step = 1; step <= steps; ++step)
    {
        const glm::vec3 candidate = glm::mix(previous, requested,
            static_cast<float>(step) / static_cast<float>(steps));
        if (!CollisionSystem::canOccupy(candidate, gateAngleDegrees,
                                        stallGateAngleDegrees,
                                        calfShedDoorAngleDegrees))
            break;
        camera.Position = candidate;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (birdEyeMode || fourViewMode)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Forward);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Backward);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Left);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Right);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Down);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        tryCameraMovement(CameraMovement::Up);
}

float moveTowardAngle(float current, float target, float maximumStep)
{
    if (current < target)
        return std::min(current + maximumStep, target);
    if (current > target)
        return std::max(current - maximumStep, target);
    return current;
}

void updateAnimations(float frameDeltaTime)
{
    const float targetAngle = gateShouldBeOpen ? 90.0f : 0.0f;
    const float maximumStep = GateSpeedDegreesPerSecond * frameDeltaTime;
    const float gateCandidate = moveTowardAngle(gateAngleDegrees, targetAngle,
                                                 maximumStep);
    if (CollisionSystem::canOccupy(camera.Position, gateCandidate,
                                   stallGateAngleDegrees,
                                   calfShedDoorAngleDegrees))
        gateAngleDegrees = gateCandidate;

    // Rear stall leaves swing forward along the stall dividers. A positive
    // angle sent them through the shed's brick back wall.
    const float stallTarget = stallGatesShouldBeOpen ? -88.0f : 0.0f;
    const float stallCandidate = moveTowardAngle(stallGateAngleDegrees,
                                                 stallTarget, maximumStep);
    if (CollisionSystem::canOccupy(camera.Position, gateAngleDegrees,
                                   stallCandidate,
                                   calfShedDoorAngleDegrees))
        stallGateAngleDegrees = stallCandidate;

    animationSystem.update(frameDeltaTime);

    const float calfDoorTarget = animationSystem.calfShedDoorNeeded() ? 92.0f : 0.0f;
    const float calfDoorStep = 180.0f * frameDeltaTime;
    const float calfDoorCandidate = moveTowardAngle(calfShedDoorAngleDegrees,
                                                     calfDoorTarget,
                                                     calfDoorStep);
    if (CollisionSystem::canOccupy(camera.Position, gateAngleDegrees,
                                   stallGateAngleDegrees,
                                   calfDoorCandidate))
        calfShedDoorAngleDegrees = calfDoorCandidate;
}

void renderScene(const Shader& shader, const FarmScene& farmScene,
                 const EntityRenderer& entityRenderer,
                 const CurvedRenderer& curvedRenderer,
                 const FarmTextures& textures)
{
    farmScene.render(shader, gateAngleDegrees, stallGateAngleDegrees,
                     calfShedDoorAngleDegrees,
                     animationSystem.fanAngle(),
                     lightingSystem.pointFixtureEmissions(),
                     lightingSystem.fenceFixtureEmission(),
                     lightingSystem.spotlightFixtureEmission());
    entityRenderer.render(shader, animationSystem);
    curvedRenderer.render(shader, textures);
}

glm::mat4 technicalProjection(float aspectRatio, float halfHeight)
{
    const float halfWidth = halfHeight * aspectRatio;
    return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 120.0f);
}

void renderOneView(
    const Shader& shader, const FarmScene& farmScene,
    const EntityRenderer& entityRenderer, const CurvedRenderer& curvedRenderer,
    const FarmTextures& textures, int x, int y, int width, int height,
    const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& viewPosition)
{
    if (width <= 0 || height <= 0)
        return;
    glViewport(x, y, width, height);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    lightingSystem.setupShader(shader, viewPosition);
    renderScene(shader, farmScene, entityRenderer, curvedRenderer, textures);
}

void drawFourViewSeparators(int width, int height, int verticalSplit,
                            int horizontalSplit)
{
    constexpr int thickness = 3;
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.78f, 0.57f, 0.20f, 1.0f);
    glScissor(std::max(0, verticalSplit - thickness / 2), 0,
              thickness, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glScissor(0, std::max(0, horizontalSplit - thickness / 2),
              width, thickness);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void printControls()
{
    std::cout
        << "========================================\n"
        << "AMS AGRO FARM - INTEGRATED SYSTEM\n"
        << "========================================\n"
        << "W/A/S/D : Move Camera\n"
        << "Mouse   : Look Around\n"
        << "Tab     : Capture/Release Mouse\n"
        << "F1      : Print Controls Again\n"
        << "Q/E     : Move Down/Up\n"
        << "Scroll  : Zoom\n"
        << "B       : Bird's-Eye View\n"
        << "V       : Four-Viewport Mode\n"
        << "G       : Open/Close Farm Gate\n"
        << "O       : Open/Close Cattle Stall Gates\n"
        << "T       : Toggle Textures\n"
        << "C       : Pause/Resume Adult Cows\n"
        << "R       : Pause/Resume All Calf Motion\n"
        << "H       : Pause/Resume Cow Head Motion\n"
        << "M       : Next Worker Task (Go/Feed/Home)\n"
        << "K       : Send Worker Home\n"
        << "L       : Recall/Release Mobile Animals\n"
        << "J       : Calves Leave/Return Ox-Side Shed\n"
        << "N       : Send Calves Through Ox-Side Door to Feed\n"
        << "F       : Pause/Resume Shed Fans\n"
        << "1/KP1   : Directional Light ON/OFF\n"
        << "2/KP2   : All Point-Light Groups ON/OFF\n"
        << "3/KP3   : Entrance Spotlight ON/OFF\n"
        << "4/KP4   : Day/Night Toggle\n"
        << "5/KP5   : Ambient Component ON/OFF\n"
        << "6/KP6   : Diffuse Component ON/OFF\n"
        << "7/KP7   : Specular Component ON/OFF\n"
        << "8/KP8   : Shed/Building Lights ON/OFF\n"
        << "9/KP9   : Fence Lamps ON/OFF\n"
        << "0/KP0   : Entrance Banner Light ON/OFF\n"
        << "P       : Owner Billboard Light ON/OFF\n"
        << "ESC     : Exit\n"
        << "========================================\n"
        << "Curves + Multiple Views + Integrated Farm\n"
        << "========================================\n";
}
}

int main()
{
    printControls();

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        InitialWidth,
        InitialHeight,
        "AMS Agro Farm",
        nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    updateWindowTitle(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glEnable(GL_DEPTH_TEST);

    int exitCode = 0;
    try
    {
        // These objects must be destroyed while the OpenGL context still exists.
        Shader textureShader("shaders/texture.vs", "shaders/texture.fs");
        TextureManager textureManager;
        CubeRenderer cubeRenderer;
        PrimitiveRenderer primitiveRenderer;
        CurvedRenderer curvedRenderer;
        FarmScene farmScene(cubeRenderer, primitiveRenderer, textureManager.farm());
        EntityRenderer entityRenderer(cubeRenderer, primitiveRenderer);

        if (textureManager.allFilesLoaded())
            std::cout << "All farm textures loaded successfully.\n";

        lastFrame = static_cast<float>(glfwGetTime());
        while (!glfwWindowShouldClose(window))
        {
            const float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = std::min(currentFrame - lastFrame, 0.10f);
            lastFrame = currentFrame;

            processInput(window);
            updateAnimations(deltaTime);

            const glm::vec3 skyColor = lightingSystem.clearColor();
            glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (framebufferWidth > 0 && framebufferHeight > 0)
            {
                const glm::mat4 projection = glm::perspective(
                    glm::radians(camera.Zoom),
                    static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight),
                    0.1f,
                    150.0f);
                const glm::mat4 view = camera.getViewMatrix();

                cubeRenderer.setTexturesEnabled(texturesEnabled);
                curvedRenderer.setTexturesEnabled(texturesEnabled);

                if (fourViewMode && framebufferWidth >= 2 && framebufferHeight >= 2)
                {
                    const int leftWidth = framebufferWidth / 2;
                    const int rightWidth = framebufferWidth - leftWidth;
                    const int bottomHeight = framebufferHeight / 2;
                    const int topHeight = framebufferHeight - bottomHeight;

                    const float freeAspect = static_cast<float>(leftWidth) / topHeight;
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), 0, bottomHeight, leftWidth, topHeight,
                                  camera.getViewMatrix(),
                                  glm::perspective(glm::radians(camera.Zoom), freeAspect, 0.1f, 150.0f),
                                  camera.Position);

                    const glm::vec3 topEye(0.0f, 42.0f, 0.01f);
                    const float topAspect = static_cast<float>(rightWidth) / topHeight;
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), leftWidth, bottomHeight, rightWidth, topHeight,
                                  glm::lookAt(topEye, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                  technicalProjection(topAspect, 19.5f), topEye);

                    const glm::vec3 frontEye(0.0f, 8.0f, 36.0f);
                    const float frontAspect = static_cast<float>(leftWidth) / bottomHeight;
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), 0, 0, leftWidth, bottomHeight,
                                  glm::lookAt(frontEye, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                                  technicalProjection(frontAspect, 12.5f), frontEye);

                    const glm::vec3 sideEye(36.0f, 8.0f, 0.0f);
                    const float sideAspect = static_cast<float>(rightWidth) / bottomHeight;
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), leftWidth, 0, rightWidth, bottomHeight,
                                  glm::lookAt(sideEye, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                                  technicalProjection(sideAspect, 12.5f), sideEye);

                    drawFourViewSeparators(framebufferWidth, framebufferHeight,
                                           leftWidth, bottomHeight);
                }
                else if (birdEyeMode)
                {
                    const float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;
                    const glm::vec3 birdEye(0.0f, 42.0f, 0.01f);
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), 0, 0, framebufferWidth, framebufferHeight,
                                  glm::lookAt(birdEye, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                  technicalProjection(aspect, 19.5f), birdEye);
                }
                else
                {
                    renderOneView(textureShader, farmScene, entityRenderer, curvedRenderer,
                                  textureManager.farm(), 0, 0, framebufferWidth, framebufferHeight,
                                  view, projection, camera.Position);
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "Application error: " << error.what() << '\n';
        exitCode = -1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
