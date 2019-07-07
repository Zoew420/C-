#pragma once
#include <variant>
#include "../glm/glm.hpp"

namespace T {
    using namespace std;
    using namespace glm;

    // 粒子类型
    enum class ParticleType {
        None,
        Sand,
        Iron
    };

    // 粒子基类
    struct IParticle {
        virtual ParticleType get_type() = 0;
        float temperature = 0.f;
        vec2 position = vec2();
    };

    // 粉尘粒子基类
    struct IPowder : IParticle {
        virtual float get_mass() = 0;
        vec2 velocity = vec2();
    };

    // 固体粒子基类
    struct ISolid : IParticle {

    };

    // 沙
    struct Sand : IPowder {
        ParticleType get_type() { return ParticleType::Sand; }
        float get_mass() { return 1.f; }
    };

    // 铁
    struct Iron : ISolid {
        ParticleType get_type() { return ParticleType::Iron; }
    };

    // 通用粒子类
    using Particle = variant<Sand, Iron>;

    // 粒子的简略信息，由ViewModel返回给View
    struct ParticleInfo {
        ParticleType type = ParticleType::None;
        vec2 position = vec2();
    };

    // 新增粒子的笔刷
    struct ParticleBrush {
        vec2 center;
        float radius;
        ParticleType type;
    };
}