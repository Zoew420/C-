#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"
#include "constant.h"
#include <vector>

namespace T {
    using namespace std;


    // ʾ�����
    class GameModel {

        struct AirFlowState {
            vector<float> map_vx, map_vy;
            // TODO: ..
            AirFlowState(int n_map) : map_vx(n_map), map_vy(n_map) {};
        };

        // ��¼һ�����ص���ȫ��������
        struct PixelParticleList {
            int from = -1, to = -1;
            bool nil() { return from < 0 ; }
            PixelParticleList() {}
            PixelParticleList(int i) : from(i), to(i) {}
            PixelParticleList(int from, int to) : from(from), to(to) {}
        };

        struct State {
            int particles;
            vector<PixelParticleList> map_index; // ����ĳ��λ�õ������±� map_index[idx(r,c)]
            vector<ParticleType> p_type;
            vector<vec2> p_pos, p_vel;
            State(int n, int n_map) : particles(n), map_index(n_map), p_type(n), p_pos(n), p_vel(n) {}
        };

        // �������ǵĳ����Ǽ����ܼ��ģ�������ﲻ�ýṹ������
        // ���ø�ԭʼ�ĵ��е����飬��������ԭ��
        // 1. ���д洢����Ч�ʸ��ߣ�
        // 2. ����ʱ����ʹ��Jacobi�����������֡�Ľ�������������һ֡��Χ�Ľ������
        //    �����Ҫ������������Ping-Pong buffer����С����������
        int width, height;
        PingPong<State> state;
        float dt = 0.01;

        int idx(int r, int c) { return r * width + c; }
        int idx(ivec2 v) { return idx(v.x, v.y); }
        ivec2 f2i(vec2 v) { return v + vec2(0.5); };
        int f2i(float v) { return v + 0.5; }

        void compute_vel() {
            for (int ip = 0; ip < state.prev()->particles; ip++) {
                // get map index
                int im = idx(f2i(state.prev()->p_pos[ip]));

                // TODO: air velocity
                vec2 v_air = vec2();

                // particle velocity
                vec2 v_p = state.prev()->p_vel[ip];

                // relative velocity
                vec2 v_rel = v_p - v_air;

                // TODO: air pressure
                float p = 1.f;

                float mass = particle_mass(state.prev()->p_type[ip]);

                vec2 f_resis = -K_AIR_RESISTANCE * p * v_rel * length(v_rel);
                vec2 f_gravity = K_GRAVITY * vec2(0, 1) * mass;

                vec2 f = f_resis + f_gravity;
                vec2 acc = f / mass;

                // set velocity
                state.cur()->p_vel[ip] = state.prev()->p_vel[ip] + acc * dt;
            }
        }

        void compute_air_flow() {
            // TODO: ����������
        }
        void compute_position() {
            // TODO: ����λ��
            // ��ײ���
            for (int ip = 0; ip < state.prev()->particles; ip++) {

            }
        }
        void reorder() {

        }

    public:
        GameModel(int w, int h) : width(w), height(h), state(State(0, w* h), State(0, w* h)) {};
        void update() {
            state.swap();
            compute_vel();
            compute_air_flow();
            compute_position();
            reorder();
        }
    };
}