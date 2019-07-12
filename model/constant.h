#pragma once
#include "../glm/glm.hpp"

namespace Simflow {
    using namespace glm;
    constexpr float K_GRAVITY = 10.0;
    constexpr float K_DT = 0.1f;

    constexpr float K_AIR_RESISTANCE = 0.2;
    constexpr int K_AIRFLOW_DOWNSAMPLE = 4;

    const int K_LIQUID_GRID_DOWNSAMPLE = 8;
    const int K_LIQUID_ITERATIONS = 5;
    const float K_LIQUID_RADIUS = 7.f;//16.f * K_LIQUID_SCALE; // kernel radius

    const float K_COLLISION_STEP_LENGTH = .5;
    const float K_COLLISION_RESTITUTION = 0.2;


    const float K_HEAT_DELTA = 5.0f;
    const int K_HEAT_ITERATIONS = 10;



    const float EPS = 1E-6;


    //const float M_PI = 3.14159;

    //const float REST_DENS = 2; // rest density
    //const float GAS_CONST = 10.f; // const for equation of state
    //const float HSQ = K_LIQUID_RADIUS * K_LIQUID_RADIUS; // radius^2 for optimization
    //const float VISC = 0.1; // viscosity constant

    //// smoothing kernels defined in M¨¹ller and their gradients
    //const float POLY6 = 315.f / (65.f * M_PI * pow(K_LIQUID_RADIUS, 9.f));
    //const float SPIKY_GRAD = -45.f / (M_PI * pow(K_LIQUID_RADIUS, 6.f));
    //const float VISC_LAP = 45.f / (M_PI * pow(K_LIQUID_RADIUS, 6.f));
}