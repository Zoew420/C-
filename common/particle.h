#pragma once
#include <variant>
#include "../glm/glm.hpp"

namespace T {
    using namespace std;
    using namespace glm;

    // 粒子类型
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


    // 粒子的简略信息，由ViewModel返回给View
    struct ParticleInfo {
        ParticleType type = ParticleType::None;
        vec2 position = vec2();
    };


    struct Brush {
        enum class Type {
            Heat,
            Particle
        };
        virtual Type type() = 0;
    };

    struct HeatBrush : Brush {

        virtual Type type() { return Type::Heat; }
    };

    // 新增粒子的笔刷
    struct ParticleBrush : Brush {
        vec2 center;
        float radius;
        float heat;
        ParticleType type = ParticleType::None;
        ParticleBrush() = default;
        ParticleBrush(vec2 center, float radius, ParticleType type, float heat = 0.f) : center(center), radius(radius), type(type), heat(heat) {}
        virtual Type type() { return Type::Particle; }
    };
}