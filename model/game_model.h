#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"
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

        struct State {
            int particles;
            vector<int> map_index; // ����ĳ��λ�õ������±� map_index[idx(r,c)]
            vector<ParticleType> p_type;
            vector<vec2> p_pos, p_acc, p_vel;
            AirFlowState air_flow;
            State(int n, int n_map) : particles(n), map_index(n_map), p_type(n), p_pos(n), p_acc(n), p_vel(n), air_flow(n_map) {}
        };

        // �������ǵĳ����Ǽ����ܼ��ģ�������ﲻ�ýṹ������
        // ���ø�ԭʼ�ĵ��е����飬��������ԭ��
        // 1. ���д洢����Ч�ʸ��ߣ�
        // 2. ����ʱ����ʹ��Jacobi�����������֡�Ľ�������������һ֡��Χ�Ľ������
        //    �����Ҫ������������Ping-Pong buffer����С����������
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
        void compute_air_flow() {
            // TODO: ����������
        }
        void compute_position() {
            // TODO: ����λ��
            // ��ײ���
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