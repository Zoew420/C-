#include <iostream>
#include "imgui/glew/GL/glew.h"
#include "../common/particle.h"
#define PI 3.14159265358979323846

using namespace Simflow;

inline void DrawParticle(float x, float y, ParticleType type) {
    // 画圆有点降FPS，暂时改成方形
    float radius = 1;
    switch (type)
    {
        case Simflow::ParticleType::Iron:
            glColor4f(1, 1, 1, 0);
            break;
        case Simflow::ParticleType::Sand:
            glColor4f(1, 1, 0, 0);
            break;
        case Simflow::ParticleType::Water:
            glColor4f(0, 1, 1, 0);
            radius = 2;
            break;
        default:
            break;
    }

    glRectf(x - radius, y - radius, x + radius, y + radius);
}