#include "view/Camera.h"
#include <cmath>

const float PI = 3.14159265f;

Vec3 crossProduct(Vec3 a, Vec3 b) {
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) return v;
    return Vec3(v.x / length, v.y / length, v.z / length);
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(Vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.2f) {
    
    Position = Vec3(posX, posY, posZ);
    WorldUp = Vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    
    updateCameraVectors();
}

Vec3 Camera::GetTarget() {
    return Vec3(Position.x + Front.x, Position.y + Front.y, Position.z + Front.z);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    
    if (direction == CameraMovement::FORWARD) {
        Position.x += Front.x * velocity;
        Position.y += Front.y * velocity;
        Position.z += Front.z * velocity;
    }
    if (direction == CameraMovement::BACKWARD) {
        Position.x -= Front.x * velocity;
        Position.y -= Front.y * velocity;
        Position.z -= Front.z * velocity;
    }
    if (direction == CameraMovement::LEFT) {
        Position.x -= Right.x * velocity;
        Position.y -= Right.y * velocity;
        Position.z -= Right.z * velocity;
    }
    if (direction == CameraMovement::RIGHT) {
        Position.x += Right.x * velocity;
        Position.y += Right.y * velocity;
        Position.z += Right.z * velocity;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;
    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    Vec3 front;
    front.x = std::cos(Yaw * PI / 180.0f) * std::cos(Pitch * PI / 180.0f);
    front.y = std::sin(Pitch * PI / 180.0f);
    front.z = std::sin(Yaw * PI / 180.0f) * std::cos(Pitch * PI / 180.0f);
    Front = normalize(front);
    Right = normalize(crossProduct(Front, WorldUp));
    Up = normalize(crossProduct(Right, Front));
}