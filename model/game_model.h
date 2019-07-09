#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"
#include "constant.h"
#include <algorithm>
#include "utility.h"
#include <vector>
#include <queue>

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
            bool nil() { return from < 0; }
            void append(int i) {
                if (nil()) {
                    from = to = i;
                }
                else {
                    assert(i == to + 1);
                    to = i;
                }
            }
            PixelParticleList() {}
            PixelParticleList(int i) : from(i), to(i) {}
            PixelParticleList(int from, int to) : from(from), to(to) {
                assert(from <= to);
            }
        };

        // �������ǵĳ����Ǽ����ܼ��ģ�������ﲻ�ýṹ������
        // ���ø�ԭʼ�ĵ��е����飬��������ԭ��
        // 1. ���д洢����Ч�ʸ��ߣ�
        // 2. ����ʱ����ʹ��Jacobi�����������֡�Ľ�������������һ֡��Χ�Ľ������
        //    �����Ҫ������������Ping-Pong buffer����С����������

        struct StateCur {
            int particles = 0;
            vector<PixelParticleList> map_index; // ����ĳ��λ�õ������±� map_index[idx(r,c)]
            vector<ParticleType> p_type;
            vector<vec2> p_pos, p_vel;
            StateCur(int n_map) : map_index(n_map) {}
            void reset(int n) {
                particles = n;
                p_type.resize(n);
                p_pos.resize(n);
                p_vel.resize(n);
                for (auto& lst : map_index) {
                    lst = PixelParticleList();
                }
            }
        } state_cur;

        struct StateNext {
            int particles = 0;
            vector<ParticleType> p_type;
            vector<vec2> p_pos, p_vel;
            void reset(int n) {
                particles = n;
                p_type.resize(n);
                p_pos.resize(n);
                p_vel.resize(n);
            }
        } state_next;


        int width, height;
        float dt = 0.1;

        bool in_bound(int r, int c) { return r >= 0 && r < height && c >= 0 && c < width; }
        bool in_bound(ivec2 v) { return in_bound(v.y, v.x); }
        int idx(int r, int c) { assert(in_bound(r, c)); return r * width + c; }
        int idx(ivec2 v) { return idx(v.y, v.x); }


        void compute_vel() {
            for (int ip = 0; ip < state_cur.particles; ip++) {
                // get map index
                int im = idx(f2i(state_cur.p_pos[ip]));

                if (state_cur.p_type[ip] != ParticleType::Sand) continue;
                vec2 v_air = vec2(); // TODO: air velocity
                vec2 v_p = state_cur.p_vel[ip]; // particle velocity
                vec2 v_rel = v_p - v_air; // relative velocity
                float p = 1.f; // TODO: air pressure
                float mass = particle_mass(state_cur.p_type[ip]);
                vec2 f_resis = -K_AIR_RESISTANCE * p * v_rel * length(v_rel);
                vec2 f_gravity = K_GRAVITY * vec2(0, 1) * mass;
                vec2 f = f_resis + f_gravity;
                vec2 acc = f / mass;
                state_next.p_vel[ip] = state_cur.p_vel[ip] + acc * dt; // set velocity
            }
        }

        void compute_air_flow() {
            // TODO: ����������
        }

        struct CollisionDetectionResult {
            vec2 pos;
            int target_index;
        };

        // �����ײ���ֲڣ�
        // �Ӵ��յ㿪ʼ��ǰѰ�ң���˿��ܻᴩ�������壬���ǱȽϿ�
        bool detect_collision(vec2 start, vec2 end, CollisionDetectionResult & result) {
            int steps = length(start - end) / K_COLLISION_STEP_LENGTH;
            vec2 delta = normalize(start - end) * K_COLLISION_STEP_LENGTH; // ����=1
            int last_target = -1;
            vec2 cur = end;
            ivec2 self = f2i(start);
            vec2 final_pos = start;
            for (int i = 0; i < steps; i++) {
                ivec2 m_pos = f2i(cur);
                bool ext = true;
                if (in_bound(m_pos)) {
                    PixelParticleList lst = state_cur.map_index[idx(m_pos)];
                    if (lst.nil() || self == m_pos) {
                    }
                    else {
                        ext = false;
                        last_target = random_sample(lst.from, lst.to);
                    }
                }
                if (ext) {
                    final_pos = cur;
                    goto exit;
                }
                cur += delta;
            }
        exit:
            result.pos = final_pos;
            result.target_index = last_target;
            return last_target != -1;
        }

        void compute_position() {
            // ����λ�ã���ײ���
            for (int ip = 0; ip < state_cur.particles; ip++) {
                vec2 v = state_cur.p_vel[ip];
                vec2 pos_old = state_cur.p_pos[ip];
                vec2 pos_new = pos_old + v * dt;
                CollisionDetectionResult c_res;

                bool collided = detect_collision(pos_old, pos_new, c_res);
                if (collided) {
                    // TODO: ���Ը�����ײ�������������ٶȼ���
                    state_next.p_vel[ip] = vec2(); // ����ײ�������ٶ�Ϊ0 
                }
                state_next.p_pos[ip] = c_res.pos;

                ivec2 coord = f2i(c_res.pos);
                if (!in_bound(coord)) {
                    state_next.p_type[ip] = ParticleType::None;
                }
            }
        }

        struct ReorderBuffer {
            vector<int> p_idx; // ��¼�������Ӷ�Ӧ�Ļ����±�
            vector<int> sort; // ��ʼʱΪ0..particles-1�����ݻ����±�����
        } reorder_buf;

        // ���StateNext�����м��㣬������ռ���StateCur��
        void complete() {
            int n_next = state_next.particles;
            reorder_buf.p_idx.clear();
            reorder_buf.sort.clear();
            for (int ip = 0; ip < n_next; ip++) {
                if (state_next.p_type[ip] != ParticleType::None) {
                    reorder_buf.p_idx.push_back(idx(f2i(state_next.p_pos[ip])));
                    reorder_buf.sort.push_back(ip);
                }
            }

            vector<int>& p_idx = reorder_buf.p_idx;
            sort(reorder_buf.sort.begin(), reorder_buf.sort.end(), [&p_idx](int i1, int i2) { return p_idx[i1] - p_idx[i2]; });

            int n_new = reorder_buf.sort.size();
            // ʹ�øղŵ�StateNext��������һ��StateCur

            state_cur.reset(n_new);
            for (int ip = 0; ip < n_new; ip++) {
                int old_ip = reorder_buf.sort[ip];
                // ��������
                vec2 pos = state_next.p_pos[old_ip];
                state_cur.p_pos[ip] = pos;
                state_cur.p_type[ip] = state_next.p_type[old_ip];
                state_cur.p_vel[ip] = state_next.p_vel[old_ip];
                // ���컭������
                PixelParticleList& cur_lst = state_cur.map_index[idx(f2i(pos))];
                cur_lst.append(ip);
            }

            state_next.reset(n_new);
        }

        ParticleBrush cur_brush;
        void handle_new_particles() {
            // �������飬��������׷�ӵ�state_nextβ��
            if (cur_brush.type != ParticleType::None) {
                ivec2 center = f2i(cur_brush.center);
                int r_find = cur_brush.radius + 1;
                for (int x = center.x - r_find; x <= center.x + r_find; x++) {
                    for (int y = center.y - r_find; y <= center.y + r_find; y++) {
                        if (in_bound(x, y) && glm::distance(vec2(x, y), cur_brush.center) <= cur_brush.radius) {
                            if (state_cur.map_index[idx(ivec2(x, y))].nil()) {
                                state_next.particles++;
                                state_next.p_pos.push_back(vec2(x, y));
                                state_next.p_type.push_back(cur_brush.type);
                                state_next.p_vel.push_back(vec2());
                            }
                        }
                    }
                }
                cur_brush.type = ParticleType::None;
            }
        }

    public:
        GameModel(int w, int h) : width(w), height(h), state_cur(w * h), state_next() {};

        void update() {
            compute_vel();
            compute_air_flow();
            compute_position();
            handle_new_particles();
            complete();
        }

        void set_new_particles(ParticleBrush brush) {
            cur_brush = brush;
        }

        struct QueryParticleResult {
            const vector<ParticleType>& type;
            const vector<vec2>& position;
        };

        QueryParticleResult query_particles() {
            return QueryParticleResult{ state_cur.p_type, state_cur.p_pos };
        }
    };
}