#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "cube_renderer.h"
#include "farm_scene.h"
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
}

void printControls()
{
    std::cout
        << "========================================\n"
        << "AMS AGRO FARM - MODULE 2\n"
        << "========================================\n"
        << "W/A/S/D : Move Camera\n"
        << "Mouse   : Look Around\n"
        << "Q/E     : Move Down/Up\n"
        << "Scroll  : Zoom\n"
        << "G       : Open/Close Farm Gate\n"
        << "T       : Toggle Textures\n"
        << "ESC     : Exit\n"
        << "========================================\n"
        << "Detailed Environment + Texture Mapping\n"
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
        "Interactive 3D Agro Farm - Module 2",
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
        FarmScene farmScene(cubeRenderer, textureManager.farm());

        if (textureManager.allFilesLoaded())
            std::cout << "All Module 2 textures loaded successfully.\n";

        lastFrame = static_cast<float>(glfwGetTime());
        while (!glfwWindowShouldClose(window))
        {
            const float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = std::min(currentFrame - lastFrame, 0.10f);
            lastFrame = currentFrame;

            processInput(window);
            updateAnimations(deltaTime);

            glClearColor(0.52f, 0.78f, 0.94f, 1.0f);
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
                farmScene.render(textureShader, gateAngleDegrees);
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
