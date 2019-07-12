#pragma once
#include <functional>

namespace Simflow {
    using namespace std;
    class Parallel {
    public:
        static void invoke(std::initializer_list<function<void()>> funcs) {
            // TODO: replace it with parallel version
            for (auto& f : funcs) {
                f();
            }
        }
    };
}