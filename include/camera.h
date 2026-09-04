#ifndef AMS_AGRO_FARM_CAMERA_H
#define AMS_AGRO_FARM_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

enum class CameraMovement
{
    Forward,
    Backward,
    Left,
    Right,
    Down,
    Up
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front{0.0f, 0.0f, -1.0f};
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed{7.0f};
    float MouseSensitivity{0.10f};
    float Zoom{45.0f};

    explicit Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f)
        : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch)
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        const float velocity = MovementSpeed * deltaTime;

        if (direction == CameraMovement::Forward)
            Position += Front * velocity;
        if (direction == CameraMovement::Backward)
            Position -= Front * velocity;
        if (direction == CameraMovement::Left)
            Position -= Right * velocity;
        if (direction == CameraMovement::Right)
            Position += Right * velocity;
        if (direction == CameraMovement::Down)
            Position -= WorldUp * velocity;
        if (direction == CameraMovement::Up)
            Position += WorldUp * velocity;
    }

    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
    {
        Yaw += xOffset * MouseSensitivity;
        Pitch += yOffset * MouseSensitivity;

        if (constrainPitch)
            Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

        updateCameraVectors();
    }

    void processMouseScroll(float yOffset)
    {
        Zoom = glm::clamp(Zoom - yOffset, 20.0f, 70.0f);
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        front.y = std::sin(glm::radians(Pitch));
        front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif
