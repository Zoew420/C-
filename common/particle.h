#pragma once
#include <variant>
#include "../glm/glm.hpp"

namespace T {
    using namespace std;
    using namespace glm;

    // ��������
    enum class ParticleType {
        None = 0,
        Iron = 1,
        Sand = 2,
        Water = 3
    };

    inline float particle_mass(ParticleType type) {
        static const float particle_mass_lut[] = { NAN, INFINITY, 0.1f, 0.5f };
        return particle_mass_lut[int(type)];
    }


    // ���ӵļ�����Ϣ����ViewModel���ظ�View
    struct ParticleInfo {
        ParticleType type = ParticleType::None;
        vec2 position = vec2();
    };


    struct Brush {
        vec2 center = vec2();
        float radius = 0;
        Brush() = default;
        Brush(vec2 center, float radius) : center(center), radius(radius) {};
    };

    struct HeatBrush : Brush {
        bool increase = true;
        HeatBrush() = default;
        HeatBrush(vec2 center, float radius, bool increase) : Brush(center, radius), increase(increase) {}
    };

    // �������ӵı�ˢ
    struct ParticleBrush : Brush {
        ParticleType type = ParticleType::None;
        ParticleBrush() = default;
        ParticleBrush(vec2 center, float radius, ParticleType type) : Brush(center, radius), type(type) {}
    };
}