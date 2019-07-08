#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"

namespace T {
    struct AirFlowState {
        float* map_vx;
        float* map_vy;
        // ..
    };

    struct State {
        int particles;
        int* map_index; // 画布某个位置的粒子下标 map_index[idx(r,c)]
        ParticleType* p_type;
        vec2* p_pos;
        vec2* p_acc;
        vec2* p_vel;
        AirFlowState air_flow;
    };

    // 示意代码
    class GameModel {

        // 由于我们的程序是计算密集的，因此这里不用结构体数组
        // 而用更原始的单列的数组，出于两点原因：
        // 1. 按列存储访问效率更高；
        // 2. 迭代时必须使用Jacobi迭代（计算此帧的结果，必须访问上一帧周围的结果），
        //    因此需要两个数组来做Ping-Pong buffer，最小化拷贝量。
        int width, height;
        PingPong<State> state;

        int idx(int r, int c) { return r * width + c; }
        int idx(ivec2 v) { return idx(v.x, v.y); }
        ivec2 f2i(vec2 v) { return v + vec2(0.5); };
        int f2i(float v) { return v + 0.5; }

        void compute_force() {
            for (int i = 0; i < state.prev()->particles; i++) {
                int im = idx(f2i(state.prev()->p_pos[i]));
                vec2 v_air = vec2(state.prev()->air_flow.map_vx[i], state.prev()->air_flow.map_vy[i]);
                // TODO: ...
            }
        }
        void compute_air_flow() {}
        void compute_position() {}

    public:

        void update() {
            state.swap();
            compute_force();
            compute_air_flow();
            compute_position();
        }
    };
}