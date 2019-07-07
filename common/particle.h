#pragma once
#include <variant>
#include "../glm/glm.hpp"

namespace T {
    using namespace std;
    using namespace glm;

    enum class ParticleType {
        None,
        Sand,
        Iron
    };

    struct IParticle {
        virtual ParticleType get_type() = 0;
        float temperature = 0.f;
        vec2 position = vec2();
    };

    struct IPowder : IParticle {
        virtual float get_mass() = 0;
        vec2 velocity = vec2();
    };

    struct ISolid : IParticle {

    };

    struct Sand : IPowder {
        ParticleType get_type() { return ParticleType::Sand; }
        float get_mass() { return 1.f; }
    };

    struct Iron : ISolid {
        ParticleType get_type() { return ParticleType::Iron; }
    };

    using Particle = variant<Sand, Iron>;

    struct ParticleInfo {
        ParticleType type = ParticleType::None;
        vec2 position = vec2();
    };
}