#pragma once
#include <chrono>

namespace Simflow {
    using namespace std;

    class Timer {
        using Time = decltype(std::chrono::high_resolution_clock::now());
        Time _begin = chrono::high_resolution_clock::now();
    public:
        Timer() = default;
        Timer(const Timer&) = delete;
        Timer(Timer&&) = delete;
        float ms() {
            Time cur = chrono::high_resolution_clock::now();
            return chrono::duration_cast<chrono::milliseconds>(cur - _begin).count();
        }
    };
}