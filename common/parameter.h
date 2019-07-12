#pragma once
#include "particle.h"
#include "array2d.h"
#include <vector>

namespace Simflow {
    using namespace std;

    struct FrameData {
        const vector<ParticleInfo>& particles;
        const Array2D<float>& pressure;
    };
}