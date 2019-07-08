#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"
#include "constant.h"
#include <vector>

namespace T {
    using namespace std;


    // 示意代码
    class GameModel {

        struct AirFlowState {
            vector<float> map_vx, map_vy;
            // TODO: ..
            AirFlowState(int n_map) : map_vx(n_map), map_vy(n_map) {};
        };

        struct State {
            int particles;
            vector<int> map_index; // 画布某个位置的粒子下标 map_index[idx(r,c)]
            vector<ParticleType> p_type;
            vector<vec2> p_pos, p_acc, p_vel;
            AirFlowState air_flow;
            State(int n, int n_map) : particles(n), map_index(n_map), p_type(n), p_pos(n), p_acc(n), p_vel(n), air_flow(n_map) {}
        };

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
            for (int ip = 0; ip < state.prev()->particles; ip++) {
                // get map index
                int im = idx(f2i(state.prev()->p_pos[ip]));

                // get the air velocity
                vec2 v_air = vec2(state.prev()->air_flow.map_vx[im], state.prev()->air_flow.map_vy[im]);
                
                // get the particle velocity
                vec2 v_p = state.prev()->p_vel[ip];

                // get the relative velocity
                vec2 v_rel = v_p - v_air;

                // get the air pressure
                float p = 1.f; // TODO: get from air_flow

                vec2 f_resis = -K_AIR_RESISTANCE * p * v_rel * length(v_rel);
                vec2 f_gravity = K_GRAVITY * vec2(0,1) * 
            }
        }
        void compute_air_flow() {
            // TODO: 计算气流场
        }
        void compute_position() {
            // TODO: 更新位置
            // 碰撞检测
        }

    public:
        GameModel(int w, int h) : width(w), height(h), state(State(0, w* h), State(0, w* h)) {};
        void update() {
            state.swap();
            compute_force();
            compute_air_flow();
            compute_position();
        }
    };
}