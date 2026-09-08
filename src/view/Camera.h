#pragma once

#include <cmath>

struct Vec3 {
    float x, y, z;
    Vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

// Câmera orbital: sempre mira um ponto fixo (o centro da grid) e só pode
// girar ao redor dele, ajustando um ângulo horizontal (azimuth) e um
// ângulo vertical (elevation). Não há mais translação livre nem zoom.
class Camera {
public:
    Vec3 Position;
    Vec3 Up;

    // center: ponto que a câmera sempre mira (tipicamente o centro da grid).
    // radius: distância da câmera até o centro.
    // azimuthDeg / elevationDeg: ângulos iniciais, em graus.
    Camera(Vec3 center, float radius, float azimuthDeg, float elevationDeg);

    // Gira a câmera horizontalmente em torno do centro.
    void orbitHorizontal(float deltaDeg);

    // Inclina a câmera verticalmente em torno do centro (com limites, para
    // não deixar a câmera atravessar o chão nem virar de cabeça para baixo
    // durante a órbita livre — a vista top-down exata de 90° fica reservada
    // para o botão dedicado do próximo passo). Não faz nada enquanto a
    // vista top-down estiver travada (ver toggleTopView()).
    void orbitVertical(float deltaDeg);

    // Alterna entre a órbita livre e a vista de topo (elevation travada em
    // 90°, olhando reto para baixo). Ao travar, guarda o ângulo atual para
    // restaurar quando destravar.
    void toggleTopView();
    bool isTopView() const { return topViewActive; }

    Vec3 GetTarget() const { return center; }

private:
    Vec3 center;
    float radius;
    float azimuth;
    float elevation;

    bool topViewActive = false;
    float savedElevation = 0.0f;

    static constexpr float kMinElevation = 5.0f;
    static constexpr float kMaxElevation = 85.0f;
    static constexpr float kTopViewElevation = 90.0f;

    void updateCameraVectors();
};