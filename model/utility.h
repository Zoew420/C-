#pragma once
#include "../glm/glm.hpp"

namespace T {
    using namespace glm;

    inline int random_sample(int from_inclusive, int to_inclusive) {
        // TODO
        return from_inclusive;
    }
    inline ivec2 f2i(vec2 v) { return v + vec2(0.5); };
    inline int f2i(float v) { return v + 0.5; }
}