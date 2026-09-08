#include "view/Camera.h"

namespace {
    const float PI = 3.14159265f;
}

Camera::Camera(Vec3 centerIn, float radiusIn, float azimuthDeg, float elevationDeg)
    : center(centerIn), radius(radiusIn), azimuth(azimuthDeg), elevation(elevationDeg) {
    updateCameraVectors();
}

void Camera::orbitHorizontal(float deltaDeg) {
    azimuth += deltaDeg;
    updateCameraVectors();
}

void Camera::orbitVertical(float deltaDeg) {
    if (topViewActive) return; // travada na vista de topo

    elevation += deltaDeg;
    if (elevation > kMaxElevation) elevation = kMaxElevation;
    if (elevation < kMinElevation) elevation = kMinElevation;
    updateCameraVectors();
}

void Camera::toggleTopView() {
    if (topViewActive) {
        elevation = savedElevation;
        topViewActive = false;
    } else {
        savedElevation = elevation;
        elevation = kTopViewElevation;
        topViewActive = true;
    }
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    float azRad = azimuth * PI / 180.0f;
    float elRad = elevation * PI / 180.0f;

    float cosEl = std::cos(elRad);
    float sinEl = std::sin(elRad);
    float cosAz = std::cos(azRad);
    float sinAz = std::sin(azRad);

    Position.x = center.x + radius * cosEl * cosAz;
    Position.y = center.y + radius * sinEl;
    Position.z = center.z + radius * cosEl * sinAz;

    // Vetor "up" derivado analiticamente (tangente à esfera na direção de
    // elevação crescente), em vez do Up = cross(Right, Front) tradicional.
    // Essa fórmula não degenera quando elevation se aproxima de 90° (câmera
    // olhando reto para baixo) — o que é essencial para a vista top-down do
    // próximo passo, que de outra forma sofreria de gimbal lock aqui.
    Up = Vec3(-sinEl * cosAz, cosEl, -sinEl * sinAz);
}