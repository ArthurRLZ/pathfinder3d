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

    // Recentraliza a órbita (usado quando o tamanho da grid muda, já que o
    // centro geométrico da grid muda junto). Não altera azimuth/elevation
    // nem cancela uma transição em andamento.
    void setOrbit(Vec3 newCenter, float newRadius);

    // Gira a câmera horizontalmente em torno do centro.
    void orbitHorizontal(float deltaDeg);

    // Inclina a câmera verticalmente em torno do centro (com limites, para
    // não deixar a câmera atravessar o chão nem virar de cabeça para baixo
    // durante a órbita livre, a vista top-down exata de 90° só é alcançada
    // via toggleTopView()). Não faz nada enquanto a vista top-down estiver
    // travada ou enquanto a transição entre vistas estiver em andamento.
    void orbitVertical(float deltaDeg);

    // Alterna entre a órbita livre e a vista de topo (elevation travada em
    // 90°, olhando reto para baixo). Não muda o ângulo na hora: define um
    // alvo (targetElevation) e liga a animação, quem realmente aproxima a
    // câmera do alvo, um pouco a cada chamada, é animationStep().
    void toggleTopView();
    bool isTopView() const { return topViewActive; }

    // Aproxima a elevation atual de targetElevation em até maxDeltaDeg graus.
    // Chamado repetidamente (via timer) enquanto isAnimating() for true, até
    // a câmera "chegar" no ângulo alvo.
    void animationStep(float maxDeltaDeg);
    bool isAnimating() const { return animating; }

    Vec3 GetTarget() const { return center; }

private:
    Vec3 center;
    float radius;
    float azimuth;
    float elevation;

    bool topViewActive = false;
    float savedElevation = 0.0f;

    float targetElevation = 0.0f;
    bool animating = false;

    static constexpr float kMinElevation = 5.0f;
    static constexpr float kMaxElevation = 85.0f;
    static constexpr float kTopViewElevation = 90.0f;

    void updateCameraVectors();
};