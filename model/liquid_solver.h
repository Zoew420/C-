#pragma once
#include "../glm/glm.hpp"
#include <iostream>
#include <vector>

namespace T {

    using namespace std;
    using namespace glm;

    // "Particle-Based Fluid Simulation for Interactive Applications"
    // solver parameters
    const static vec2 G(0.f, 12000 * -9.8f); // external (gravitational) forces
    const static float REST_DENS = 1000.f; // rest density
    const static float GAS_CONST = 2000.f; // const for equation of state
    const static float H = 16.f; // kernel radius
    const static float HSQ = H * H; // radius^2 for optimization
    const static float MASS = 65.f; // assume all particles have the same mass
    const static float VISC = 20.f; // viscosity constant
    const static float DT = 0.00008f; // integration timestep
    const static float M_PI = 3.14159;

    // smoothing kernels defined in M¨¹ller and their gradients
    const static float POLY6 = 315.f / (65.f * M_PI * pow(H, 9.f));
    const static float SPIKY_GRAD = -45.f / (M_PI * pow(H, 6.f));
    const static float VISC_LAP = 45.f / (M_PI * pow(H, 6.f));

    // simulation parameters
    const static float EPS = H; // boundary epsilon
    const static float BOUND_DAMPING = -0.5f;

    template<typename FNeighbor>
    class LiquidSolver {
    public:
        LiquidSolver() {

        }

        void prepare_update() {

        }

        void add_particle(vec2 pos)
    };
}