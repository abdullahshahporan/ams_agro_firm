#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "animation_system.h"
#include "cube_renderer.h"
#include "entity_renderer.h"
#include "farm_scene.h"
#include "lighting_system.h"
#include "primitive_renderer.h"
#include "shader.h"
#include "texture_manager.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

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
bool texturesEnabled = true;
AnimationSystem animationSystem;
LightingSystem lightingSystem;

int framebufferWidth = static_cast<int>(InitialWidth);
int framebufferHeight = static_cast<int>(InitialHeight);

void framebufferSizeCallback(GLFWwindow*, int width, int height)
{
    framebufferWidth = width;
    framebufferHeight = height;
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow*, int key, int, int action, int)
{
    // GLFW_PRESS fires once per physical press; holding G cannot repeatedly toggle.
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        gateShouldBeOpen = !gateShouldBeOpen;
        std::cout << "Gate: " << (gateShouldBeOpen ? "opening" : "closing") << '\n';
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
        std::cout << "Calf running: " << (animationSystem.calvesOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        animationSystem.toggleHeadMotion();
        std::cout << "Cow head/grazing motion: " << (animationSystem.headMotionOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        animationSystem.toggleWorkers();
        std::cout << "Worker movement: " << (animationSystem.workersOn() ? "ON" : "PAUSED") << '\n';
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        animationSystem.toggleFans();
        std::cout << "Shed fans: " << (animationSystem.fansOn() ? "ON" : "PAUSED") << '\n';
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        lightingSystem.toggleDirectional();
        std::cout << "Directional light: " << (lightingSystem.directionalEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        lightingSystem.togglePointLights();
        std::cout << "Point lights: " << (lightingSystem.pointLightsEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        lightingSystem.toggleSpotlight();
        std::cout << "Entrance spotlight: " << (lightingSystem.spotlightEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        lightingSystem.toggleDayNight();
        std::cout << "Time of day: " << (lightingSystem.nightMode() ? "NIGHT" : "DAY") << '\n';
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        lightingSystem.toggleAmbient();
        std::cout << "Ambient component: " << (lightingSystem.ambientEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        lightingSystem.toggleDiffuse();
        std::cout << "Diffuse component: " << (lightingSystem.diffuseEnabled() ? "ON" : "OFF") << '\n';
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        lightingSystem.toggleSpecular();
        std::cout << "Specular component: " << (lightingSystem.specularEnabled() ? "ON" : "OFF") << '\n';
    }
}

void mouseCallback(GLFWwindow*, double xPosition, double yPosition)
{
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
    camera.processMouseScroll(static_cast<float>(yOffset));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Down, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::Up, deltaTime);
}

void updateAnimations(float frameDeltaTime)
{
    const float targetAngle = gateShouldBeOpen ? 90.0f : 0.0f;
    const float maximumStep = GateSpeedDegreesPerSecond * frameDeltaTime;

    if (gateAngleDegrees < targetAngle)
        gateAngleDegrees = std::min(gateAngleDegrees + maximumStep, targetAngle);
    else if (gateAngleDegrees > targetAngle)
        gateAngleDegrees = std::max(gateAngleDegrees - maximumStep, targetAngle);

    animationSystem.update(frameDeltaTime);
}

void renderScene(const Shader& shader, const FarmScene& farmScene,
                 const EntityRenderer& entityRenderer)
{
    farmScene.render(shader, gateAngleDegrees, animationSystem.fanAngle(),
                     lightingSystem.pointFixtureEmission(),
                     lightingSystem.spotlightFixtureEmission());
    entityRenderer.render(shader, animationSystem);
}

void printControls()
{
    std::cout
        << "========================================\n"
        << "AMS AGRO FARM - MODULE 4\n"
        << "========================================\n"
        << "W/A/S/D : Move Camera\n"
        << "Mouse   : Look Around\n"
        << "Q/E     : Move Down/Up\n"
        << "Scroll  : Zoom\n"
        << "G       : Open/Close Farm Gate\n"
        << "T       : Toggle Textures\n"
        << "C       : Pause/Resume Adult Cows\n"
        << "R       : Pause/Resume Running Calves\n"
        << "H       : Pause/Resume Cow Head Motion\n"
        << "M       : Pause/Resume Workers\n"
        << "F       : Pause/Resume Shed Fans\n"
        << "1       : Directional Light ON/OFF\n"
        << "2       : Point Lights ON/OFF\n"
        << "3       : Entrance Spotlight ON/OFF\n"
        << "4       : Day/Night Toggle\n"
        << "5       : Ambient Component ON/OFF\n"
        << "6       : Diffuse Component ON/OFF\n"
        << "7       : Specular Component ON/OFF\n"
        << "ESC     : Exit\n"
        << "========================================\n"
        << "Phong Lighting + Multiple Lights + Day/Night\n"
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
        "AMS Agro Farm - Module 4",
        nullptr,
        nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create the GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
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
        FarmScene farmScene(cubeRenderer, textureManager.farm());
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
                textureShader.use();
                textureShader.setMat4("projection", projection);
                textureShader.setMat4("view", view);
                lightingSystem.setupShader(textureShader, camera.Position);
                renderScene(textureShader, farmScene, entityRenderer);
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
