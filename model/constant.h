#pragma once
#include "../glm/glm.hpp"

namespace Simflow {
    using namespace glm;
    constexpr float K_GRAVITY = 10.0;
    constexpr float K_DT = 0.1f;

    constexpr float K_AIR_RESISTANCE = 0.2;
    constexpr int K_AIRFLOW_DOWNSAMPLE = 4;

    const int K_LIQUID_GRID_DOWNSAMPLE = 4;
    const int K_LIQUID_ITERATIONS = 5;
    const float K_LIQUID_RADIUS = 2.f;//16.f * K_LIQUID_SCALE; // kernel radius

    const float K_COLLISION_STEP_LENGTH = .5;
    const float K_COLLISION_RESTITUTION = 0.2;

    const float K_HEAT_DELTA = 5.0f;
    const int K_HEAT_ITERATIONS = 10;

    const float EPS = 1E-6;
}