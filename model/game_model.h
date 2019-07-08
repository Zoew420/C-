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
        int* map_index; // ����ĳ��λ�õ������±� map_index[idx(r,c)]
        ParticleType* p_type;
        vec2* p_pos;
        vec2* p_acc;
        vec2* p_vel;
        AirFlowState air_flow;
    };

    // ʾ�����
    class GameModel {

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