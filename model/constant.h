#pragma once
#include "../glm/glm.hpp"

namespace T {
    using namespace glm;

    const float K_COLLISION_STEP_LENGTH = .5;
    const float K_AIR_RESISTANCE = .2;
    const float K_GRAVITY = 50.0;
    const float K_DT = 0.1;
    const int K_AIRFLOW_DOWNSAMPLE = 4;

    const float M_PI = 3.14159;

    // "Particle-Based Fluid Simulation for Interactive Applications"
    // solver parameters
    const float REST_DENS = 1.42; // rest density
    const float GAS_CONST = 2000.f; // const for equation of state
    const float H = 2.f;//16.f * K_LIQUID_SCALE; // kernel radius
    const float HSQ = H * H; // radius^2 for optimization
    const float VISC = 1; // viscosity constant

    // smoothing kernels defined in M¨¹ller and their gradients
    const float POLY6 = 315.f / (65.f * M_PI * pow(H, 9.f));
    const float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
    const float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));
}