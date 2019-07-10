#pragma once
#include "../glm/glm.hpp"

namespace T {
    using namespace glm;

    inline int random_sample(int from_inclusive, int to_inclusive) {
        int rnd = rand() << 15 | rand();
        return rnd % (to_inclusive - from_inclusive + 1) + from_inclusive;
    }

    inline float random(float from, float to) {
        return float(rand()) / RAND_MAX * (to - from) + from;
    }

    inline ivec2 f2i(vec2 v) { return v + vec2(0.5); };
    inline int f2i(float v) { return v + 0.5; }
}