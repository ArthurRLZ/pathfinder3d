#pragma once

#include <cmath>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct Vec3 {
    float x, y, z;
    Vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

class Camera {
public:
    Vec3 Position;
    Vec3 Front;
    Vec3 Up;
    Vec3 Right;
    Vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    Vec3 GetTarget();

private:
    void updateCameraVectors();
};