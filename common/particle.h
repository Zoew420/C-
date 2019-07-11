#pragma once
#include <variant>
#include "../glm/glm.hpp"

namespace Simflow {
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
        static const float particle_mass_lut[] = { 1, 1E+8, 0.1f, 0.6f };
        return particle_mass_lut[int(type)];
    }

	inline float particle_diff(ParticleType type) {
		if (type == ParticleType::Iron) {
			return 2.2 / 100000;
		}
		else if (type == ParticleType::Sand) {
			return 9.02 / 10000000;
		}
		else if (type == ParticleType::Water) {
			return 2.78 / 10000000;
		}
	}

    // 粒子的简略信息，由ViewModel返回给View
    struct ParticleInfo {
        ParticleType type = ParticleType::None;
        vec2 position = vec2();
		float temperature = 25.00;
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

    // 新增粒子的笔刷
    struct ParticleBrush : Brush {
        ParticleType type = ParticleType::None;
        ParticleBrush() = default;
        ParticleBrush(vec2 center, float radius, ParticleType type) : Brush(center, radius), type(type){}
    };
}