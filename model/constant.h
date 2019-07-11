#pragma once
#include "../glm/glm.hpp"

namespace T {
    using namespace glm;
    const int K_AIRFLOW_DOWNSAMPLE = 4;
    const float K_COLLISION_STEP_LENGTH = .5;
    const float EPS = 1E-6;
    const float K_AIR_RESISTANCE = 0.0;

    const float K_GRAVITY = 50.0;
    const float K_DT = 0.01f;
    const float K_RESTITUTION = 0.0;

    const int K_LIQUID_ITERATIONS = 1;


    const float M_PI = 3.14159;

    const float REST_DENS = 2; // rest density
    const float GAS_CONST = 10.f; // const for equation of state
    const float H = 1.5f;//16.f * K_LIQUID_SCALE; // kernel radius
    const float HSQ = H * H; // radius^2 for optimization
    const float VISC = 0.1; // viscosity constant


    //constexpr float SCALE = 1;//0.65E+3;
    //constexpr float GSCALE = 1; //

    //// "Particle-Based Fluid Simulation for Interactive Applications"
    //// solver parameters
    //constexpr float K_GRAVITY = 12000 * 9.8f * SCALE * GSCALE;
    //const vec2 G = vec2(0, -12000 * 9.8f * SCALE * GSCALE); // external (gravitational) forces
    //constexpr float REST_DENS = 1000.f * GSCALE; // rest density
    //const float GAS_CONST = 2000; // const for equation of state
    //constexpr float H = 1.f;//16.f * K_LIQUID_SCALE; // kernel radius
    //constexpr float HSQ = H * H; // radius^2 for optimization
    //constexpr float MASS = 0.6; // assume all particles have the same mass
    //constexpr float VISC = 10; // viscosity constant
    //const float K_DT = 0.00008f / sqrt(GSCALE); // integration timestep
    //constexpr float M_PI = 3.14159;





    // smoothing kernels defined in M¨¹ller and their gradients
    const float POLY6 = 315.f / (65.f * M_PI * pow(H, 9.f));
    const float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
    const float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));
}