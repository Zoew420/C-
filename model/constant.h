#pragma once
#include "../glm/glm.hpp"

namespace T {
    using namespace glm;
    const int K_AIRFLOW_DOWNSAMPLE = 4;
    const int K_LIQUID_DOWNSAMPLE = 8;

    const float K_COLLISION_STEP_LENGTH = .5;
    const float EPS = 1E-6;
    const float K_AIR_RESISTANCE = 0.2;

    const float K_GRAVITY = 10.0;
    const float K_DT = 0.1f;
    const float K_RESTITUTION = 0.2;

    const int K_LIQUID_ITERATIONS = 5;

    const float H = 7.f;//16.f * K_LIQUID_SCALE; // kernel radius
    inline float kernel_fn(float dist) {
        return 20 * pow(H - dist, 2);
        //return 2000.f * glm::clamp(-dist + 1, 0.f, 1.f);
    }

    const float M_PI = 3.14159;

    const float REST_DENS = 2; // rest density
    const float GAS_CONST = 10.f; // const for equation of state
    const float HSQ = H * H; // radius^2 for optimization
    const float VISC = 0.1; // viscosity constant

    // smoothing kernels defined in M¨¹ller and their gradients
    const float POLY6 = 315.f / (65.f * M_PI * pow(H, 9.f));
    const float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
    const float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));
}