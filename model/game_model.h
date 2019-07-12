#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/array2d.h"
#include "../common/timer.h"
#include "air_solver.h"
#include "constant.h"
#include <algorithm>
#include "utility.h"
#include <vector>
#include <queue>
#include <chrono>

namespace Simflow {
    using namespace std;

    // 示意代码
    template<int width, int height>
    class GameModel {
    public:
        int frame_counter = 0;

        AirSolver airflow_solver;

        // 记录一个像素点内全部的粒子
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

        struct BlockLiquidList {
            vector<int> idx_lp;
            BlockLiquidList() : idx_lp(16) {}
        };


        struct StateCur {
            int particles = 0;
            vector<PixelParticleList> map_index; // 画布某个位置的粒子下标 map_index[idx(r,c)]
            vector<BlockLiquidList> map_block_liquid;
            vector<ParticleType> p_type;
            vector<float> p_heat;
            vector<vec2> p_pos, p_vel;
            StateCur(int n_map) : map_index(n_map), map_block_liquid(n_map / K_LIQUID_GRID_DOWNSAMPLE / K_LIQUID_GRID_DOWNSAMPLE) {}
            void reset(int n) {
                particles = n;
                p_type.resize(n);
                p_pos.resize(n);
                p_vel.resize(n);
                p_heat.resize(n);
                for (auto& lst : map_index) {
                    lst = PixelParticleList();
                }
                for (auto& lst : map_block_liquid) {
                    lst.idx_lp.clear();
                }
            }
        } state_cur;

        struct StateNext {
            int particles = 0;
            vector<ParticleType> p_type;
            vector<float> p_heat;
            vector<vec2> p_pos, p_vel;
            void reset(int n) {
                particles = n;
                p_type.resize(n);
                p_pos.resize(n);
                p_vel.resize(n);
                p_heat.resize(n);
            }
        } state_next;

        //int width, height;
        //int width_air, height_air;
        //int width_liquid, height_liquid;


        bool in_bound(int c, int r) { return r >= 0 && r < height && c >= 0 && c < width; }
        bool in_bound(ivec2 v) { return in_bound(v.x, v.y); }
        int bound_dist(ivec2 v) {
            return min({ v.x, width - 1 - v.x, v.y, height - 1 - v.y });
        }
        int idx(int c, int r) { return r * width + c; }
        int idx(ivec2 v) { return idx(v.x, v.y); }
        int idx_liquid(int c, int r) { return r / K_LIQUID_GRID_DOWNSAMPLE * (width / K_LIQUID_GRID_DOWNSAMPLE) + c / K_LIQUID_GRID_DOWNSAMPLE; }
        int idx_liquid(ivec2 v) { return idx_liquid(v.x, v.y); }
        int idx_air(int c, int r) { return r / K_AIRFLOW_DOWNSAMPLE * (width / K_AIRFLOW_DOWNSAMPLE) + c / K_AIRFLOW_DOWNSAMPLE; }
        int idx_air(ivec2 v) { return idx_air(v.x, v.y); }

        void prepare() {
            state_next.reset(state_cur.particles);
            for (int i = 0; i < state_cur.particles; i++) {
                state_next.p_type[i] = state_cur.p_type[i];
                state_next.p_pos[i] = state_cur.p_pos[i];
                state_next.p_vel[i] = state_cur.p_vel[i];
                state_next.p_heat[i] = state_cur.p_heat[i];
            }
        }


#pragma region 温度计算

        float average_heat(const vector<float> & heat, ivec2 ipos_near, float& weight) {
            weight = 0.0;
            if (in_bound(ipos_near)) {//是否在画布里
                int im1 = idx(ipos_near);//得到它在画布上的index
                PixelParticleList& list = state_cur.map_index[im1];//找到该像素点的所有粒子编号
                if (!list.nil()) {//该像素点不为空
                    float avg = 0.0f;
                    for (int i = list.from; i <= list.to; i++) {
                        avg += heat[i];
                        weight += 1;
                    }
                    avg /= (list.to - list.from + 1);
                    return avg;
                }
                else {
                    return 0;
                }
            }
            else {
                return 0;
            }
        }

        struct HeatBuffer {
            vector<float> im_heat;
            vector<float> im_heat0;
            void reset(int n) {
                im_heat.resize(n);
                im_heat0.resize(n);
            }
            void swap() {
                std::swap(im_heat, im_heat0);
            }
        } heat_buf;


        void compute_heat() {
            heat_buf.reset(state_cur.particles);
            for (int ip = 0; ip < state_cur.particles; ip++) {
                heat_buf.im_heat[ip] = state_cur.p_heat[ip];
            }

            // 对于每个粒子，查找其附近的粒子，计算下一帧的温度
            //对于每个粒子，计算其温度简化为其自身温度和加上上下左右粒子温度差值的平均值
            for (int ik = 0; ik < K_HEAT_ITERATIONS; ik++) {
                heat_buf.swap();

                for (int ip = 0; ip < state_cur.particles; ip++) {
                    //get map index
                    ivec2 ipos = f2i(state_cur.p_pos[ip]);
                    int im = idx(ipos);
                    ivec2 ipos1 = ivec2(ipos.x, ipos.y - 1);
                    ivec2 ipos2 = ivec2(ipos.x, ipos.y + 1);
                    ivec2 ipos3 = ivec2(ipos.x - 1, ipos.y);
                    ivec2 ipos4 = ivec2(ipos.x + 1, ipos.y);
                    float w[4], t[4];
                    t[0] = average_heat(heat_buf.im_heat0, ipos1, w[0]);
                    t[1] = average_heat(heat_buf.im_heat0, ipos2, w[1]);
                    t[2] = average_heat(heat_buf.im_heat0, ipos3, w[2]);
                    t[3] = average_heat(heat_buf.im_heat0, ipos4, w[3]);
                    float w_sum = 0;
                    float wt_sum = 0;
                    for (int i = 0; i < 4; i++) {
                        w_sum += w[i];
                        wt_sum += w[i] * t[i];
                    }
                    float delt_t = w_sum > 0.0f ? (wt_sum / (w_sum)-heat_buf.im_heat0[ip]) : 0;
                    heat_buf.im_heat[ip] = K_DT / K_HEAT_ITERATIONS * particle_diff(state_cur.p_type[ip]) * delt_t + heat_buf.im_heat0[ip];
                }
            }

            for (int ip = 0; ip < state_cur.particles; ip++) {
                state_next.p_heat[ip] = heat_buf.im_heat[ip];
            }
        }

#pragma endregion

#pragma region 速度计算

        void compute_vel() {
            //compute_vel_basic();
            compute_vel_all();
            constraint_solid();
        }

        void constraint_solid() {
            for (int ip = 0; ip < state_cur.particles; ip++) {
                ParticleType cur_type = state_cur.p_type[ip];
                if (cur_type == ParticleType::Iron) {
                    state_next.p_vel[ip] = vec2();
                }
            }
        }

        vec2 sample_acc_air_g(int ip) {
            ParticleType cur_type = state_cur.p_type[ip];
            if (cur_type == ParticleType::Iron) return vec2();

            state_next.p_vel[ip] = state_cur.p_vel[ip];

            // get map index
            ivec2 ipos = f2i(state_cur.p_pos[ip]);
            int im = idx(ipos);
            int im_air = idx_air(ipos);

            vec2 v_air = bilinear_sample_air_v(ipos);

            vec2 v_p = state_cur.p_vel[ip]; // particle velocity
            vec2 v_rel = v_p - v_air; // relative velocity
            float p = airflow_solver.p[im_air]; // air pressure
            p = glm::max(0.f, 1 + p / 5);
            float mass = particle_mass(cur_type);

            vec2 f_resis = -K_AIR_RESISTANCE * p * v_rel * length(v_rel);
            float limit = length(f_resis / mass * K_DT) / length(v_rel);
            if (limit > 1) f_resis /= limit; // IMPORTANT: prevent numerical explosion

            vec2 f_gravity = K_GRAVITY * vec2(0, 1) * mass;
            vec2 f = f_resis + f_gravity;
            vec2 acc = f / mass;

            return acc;
        }

        struct LiquidBuffer {
            vector<vec2> p_im_pos;
            vector<vec2> p_im_vel;
            vector<int> p_idx_mapping;
            vector<float> lp_rho, p;
            void reset_lp(int n_liquid) {
                lp_rho.resize(n_liquid);
                p.resize(n_liquid);
            }
            void reset_p(int n_all) {
                p_idx_mapping.resize(n_all);
                p_im_pos.resize(n_all);
                p_im_vel.resize(n_all);
            }
        } liquid_buf;

        template<typename F>
        void iterate_neighbor_particles(ivec2 pos, int r_neibor, F & f) {
            for (int dy = -r_neibor; dy <= r_neibor; dy++) {
                for (int dx = r_neibor; dx >= -r_neibor; dx--) {
                    ivec2 n_pos = pos + ivec2(dx, dy);
                    if (!in_bound(n_pos)) continue;
                    if (dx * dx + dy * dy > r_neibor * r_neibor) continue;
                    PixelParticleList & lst = state_cur.map_index[idx(n_pos)];
                    if (!lst.nil()) {
                        for (int t_ip = lst.from; t_ip <= lst.to; t_ip++) {
                            f(t_ip);
                        }
                    }
                }
            }
        }

        template<typename F>
        void iterate_neighbor_liquid(ivec2 pos, int r_neighbor, F & f) {
            ivec2 bfrom = (pos - ivec2(r_neighbor)) / K_LIQUID_GRID_DOWNSAMPLE;
            ivec2 bto = (pos + ivec2(r_neighbor)) / K_LIQUID_GRID_DOWNSAMPLE;
            bfrom = max(ivec2(0, 0), bfrom);
            bto = min(ivec2(width, height) / K_LIQUID_GRID_DOWNSAMPLE - 1, bto);
            for (int by = bfrom.y; by <= bto.y; by++) {
                for (int bx = bfrom.x; bx <= bto.x; bx++) {
                    int b_pos = by * (width / K_LIQUID_GRID_DOWNSAMPLE) + bx;
                    for (int ip_liquid : state_cur.map_block_liquid[b_pos].idx_lp) {
                        f(ip_liquid);
                    }
                }
            }
        }

        inline float kernel_fn(float dist) {
            return 20 * pow(K_LIQUID_RADIUS - dist, 2);
        }

        void compute_vel_all() {
            // 1. 所有粒子计算SPH应力（优化：液体附近粒子）
            // 2. 各个粒子加速度累加到state_next上
            int r_neibor = f2i(ceilf(K_LIQUID_RADIUS));
            liquid_buf.reset_p(state_cur.particles);

            int il = 0;
            for (int ip = 0; ip < state_cur.particles; ip++) {
                liquid_buf.p_im_pos[ip] = state_cur.p_pos[ip];
                liquid_buf.p_im_vel[ip] = state_cur.p_vel[ip];
                if (state_cur.p_type[ip] == ParticleType::Water) {
                    liquid_buf.p_idx_mapping[ip] = il;
                    il++;
                }
                else {
                    liquid_buf.p_idx_mapping[ip] = -1;
                }
            }

            for (int ik = 0; ik < K_LIQUID_ITERATIONS; ik++) {
                liquid_buf.reset_lp(0);

                for (int ip = 0; ip < state_cur.particles; ip++) {
                    vec2 acc = vec2();
                    ivec2 pos = f2i(liquid_buf.p_im_pos[ip]);
                    ParticleType cur_type = state_cur.p_type[ip];
                    if (cur_type == ParticleType::Iron) continue;
                    float mass = particle_mass(cur_type);
                    iterate_neighbor_liquid(pos, r_neibor, [this, mass, &ip, &acc](int t_ip) {
                        if (state_cur.p_type[ip] != ParticleType::Water) return;

                        vec2 f_press = vec2();
                        vec2 f_visc = vec2();
                        if (t_ip == ip) return;

                        vec2 pos_diff = liquid_buf.p_im_pos[t_ip] - liquid_buf.p_im_pos[ip];
                        vec2 vel_diff = liquid_buf.p_im_vel[t_ip] - liquid_buf.p_im_vel[ip];
                        //vec2 pos_diff = state_cur.p_pos[t_ip] - state_cur.p_pos[ip];
                        //vec2 vel_diff = state_cur.p_vel[t_ip] - state_cur.p_vel[ip];

                        float t_mass = particle_mass(state_cur.p_type[t_ip]);

                        vec2 jitter = vec2(random(-1, 1), random(-1, 1));
                        pos_diff += jitter * 0.1f;

                        float r = length(pos_diff);
                        if (r <= 0.01) {
                            // 防止normalize零向量
                            // 此处随机给一个方向
                            pos_diff = vec2(random(-1, 1), random(-1, 1));
                        }
                        if (r < K_LIQUID_RADIUS)
                        {
                            int t_il = liquid_buf.p_idx_mapping[t_ip];
                            vec2 f_custom = -normalize(pos_diff) * mass * kernel_fn(r);
                            vec2 f = f_custom;
                            acc += f / mass;
                        }

                    });
                    if (length(acc) > 1000) {
                        int debug = 1;
                    }
                    acc += sample_acc_air_g(ip);

                    liquid_buf.p_im_vel[ip] += acc * K_DT / float(K_LIQUID_ITERATIONS);
                    liquid_buf.p_im_pos[ip] += liquid_buf.p_im_vel[ip] * K_DT / float(K_LIQUID_ITERATIONS);

                }
            }

            for (int ip = 0; ip < state_cur.particles; ip++) {
                vec2 pos_delta = liquid_buf.p_im_pos[ip] - state_cur.p_pos[ip];
                vec2 v_delta = pos_delta / K_DT;
                state_next.p_vel[ip] = v_delta;
            }

        }

#pragma endregion

#pragma region 气流

        //int myclamp(int v, int min, int max) {
        //    v = v < min ? min : v;
        //    v = v > max ? max : v;
        //    return v;
        //}

        int safe_air_idx(ivec2 p_air) {
            int wlim = width / K_AIRFLOW_DOWNSAMPLE - 1;
            int hlim = height / K_AIRFLOW_DOWNSAMPLE - 1;
            p_air.x = clamp(p_air.x, 0, wlim);
            p_air.y = clamp(p_air.y, 0, hlim);
            return p_air.y * (width / K_AIRFLOW_DOWNSAMPLE) + p_air.x;
        }

        vec2 safe_sample_air_v(ivec2 p_air) {
            int im_air = safe_air_idx(p_air);
            return vec2(airflow_solver.getVX()[im_air], airflow_solver.getVY()[im_air]);
        }

        float safe_sample_air_p(ivec2 p_air) {
            int im_air = safe_air_idx(p_air);
            return airflow_solver.p[im_air];
        }

        vec2 myfract(vec2 v) {
            return vec2(float(int(v.x)) - v.x, float(int(v.y)) - v.y);
        }

        template<typename F>
        auto bilinear_sample_air(ivec2 pos, F & f) -> decltype(f(ivec2())) {
            using T = decltype(f(ivec2()));
            pos -= ivec2(K_AIRFLOW_DOWNSAMPLE) / 2;
            ivec2 base = pos / K_AIRFLOW_DOWNSAMPLE;
            vec2 fr = myfract(vec2(pos) / float(K_AIRFLOW_DOWNSAMPLE));
            T p[4] = {
                f(base),
                f(base + ivec2(1,0)),
                f(base + ivec2(0,1)),
                f(base + ivec2(1,1))
            };
            float w[4] = {
                (1 - fr.x) * (1 - fr.y),
                fr.x * (1 - fr.y),
                (1 - fr.x) * fr.y,
                fr.x * fr.y
            };
            T sum = T();
            for (int i = 0; i < 4; i++) {
                sum += p[i] * w[i];
            }
            return sum;
        }

        vec2 bilinear_sample_air_v(ivec2 pos) {
            //vec2 v = bilinear_sample_air(pos, [this](ivec2 pos) { return vec2(); });
            //v += bilinear_sample_air(pos, [this](ivec2 pos) { return vec2(); });
            //v += bilinear_sample_air(pos, [this](ivec2 pos) { return vec2(); });

            return bilinear_sample_air(pos, [this](ivec2 pos) { return safe_sample_air_v(pos); });
        }

        float bilinear_sample_air_p(ivec2 pos) {
            return bilinear_sample_air(pos, [this](ivec2 pos) { return safe_sample_air_p(pos); });
        }

        void compute_air_flow() {

            for (int i = 0; i < state_cur.particles; i++) {
                ivec2 pos = f2i(state_cur.p_pos[i]);
                if (bound_dist(pos) <= 2) continue;
                int im_air = idx_air(pos);
                if (state_cur.p_type[i] != ParticleType::Iron) {
                    vec2 diff = state_cur.p_vel[i] - vec2(airflow_solver.getVX()[im_air], airflow_solver.getVY()[im_air]);
                    airflow_solver.getVX()[im_air] += diff.x / K_AIRFLOW_DOWNSAMPLE / K_AIRFLOW_DOWNSAMPLE;
                    airflow_solver.getVY()[im_air] += diff.y / K_AIRFLOW_DOWNSAMPLE / K_AIRFLOW_DOWNSAMPLE;
                }
                else {
                    vec2 diff = -vec2(airflow_solver.getVX()[im_air], airflow_solver.getVY()[im_air]);
                    airflow_solver.getVX()[im_air] += diff.x / K_AIRFLOW_DOWNSAMPLE / K_AIRFLOW_DOWNSAMPLE;
                    airflow_solver.getVY()[im_air] += diff.y / K_AIRFLOW_DOWNSAMPLE / K_AIRFLOW_DOWNSAMPLE;
                }
            }

            airflow_solver.animVel();
        }

#pragma endregion

#pragma region 运动及碰撞检测

        struct CollisionDetectionResult {
            vec2 pos;
            int target_index;
        };

        bool detect_collision(vec2 start, vec2 end, bool ignore_liquid, CollisionDetectionResult & result) {
            vec2 final_pos = end;//no collision->to the end
            int last_target = -1;

            float len = length(start - end);
            if (len == 0.0f) goto exit;

            int steps = length(end - start) / K_COLLISION_STEP_LENGTH;

            vec2 delta = normalize(end - start) * K_COLLISION_STEP_LENGTH;
            vec2 cur = start;//排除所在的第一个位置，从第二个开始

            // 跳出当前像素
            while (f2i(cur) == f2i(start)) {
                cur += delta;
            }

            if (steps <= 1)//如果不满一格或刚好一格，取end做判断
            {
                if (!in_bound(f2i(cur))) {
                    final_pos = cur;
                    goto exit;
                }
                PixelParticleList temp = state_cur.map_index[idx(f2i(cur))];
                if (!temp.nil()) {
                    int t = random_sample(temp.from, temp.to);
                    if (state_cur.p_type[t] == ParticleType::Water && ignore_liquid) goto exit;
                    last_target = t;
                    if (idx(f2i(start)) == idx(f2i(cur)))final_pos = cur;
                    else final_pos = start;
                }
                goto exit;
            }

            for (int i = 1; i < steps; i++) {
                ivec2 m_pos = f2i(cur);
                bool ext = false;

                if (in_bound(m_pos)) {
                    PixelParticleList lst = state_cur.map_index[idx(m_pos)];
                    if (!lst.nil() && idx(f2i(cur)) != idx(f2i(cur - delta))) {
                        ext = true;
                        final_pos = cur - delta;
                        last_target = random_sample(lst.from, lst.to);
                    }
                }
                else {
                    final_pos = cur;
                    ext = true;
                }
                if (ext) {
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
            // 更新位置，碰撞检测
            for (int ip = 0; ip < state_cur.particles; ip++) {
                ParticleType cur_type = state_cur.p_type[ip];
                if (cur_type == ParticleType::Iron) continue;

                vec2 v = state_cur.p_vel[ip];
                vec2 pos_old = state_cur.p_pos[ip];
                vec2 pos_new = pos_old + v * K_DT;
                CollisionDetectionResult c_res;


                bool collided = detect_collision(pos_old, pos_new, false, c_res);
                if (collided) {
                    ParticleType target_type = state_cur.p_type[c_res.target_index];
                    float v1x0, v1y0, v2x0, v2y0;
                    float v1x1, v1y1, v2x1, v2y1;

                    float m1, m2;
                    m1 = particle_mass(cur_type);
                    m2 = particle_mass(target_type);

                    //v1: active one
                    //v2: passive one
                    v1x0 = state_next.p_vel[ip].x;
                    v2x0 = state_next.p_vel[c_res.target_index].x;

                    v1y0 = state_next.p_vel[ip].y;
                    v2y0 = state_next.p_vel[c_res.target_index].y;

                    v1x1 = 1.0f * (m1 * v1x0 + m2 * v2x0 + K_COLLISION_RESTITUTION * m2 * (v2x0 - v1x0)) / (m1 + m2);
                    v1y1 = 1.0f * (m1 * v1y0 + m2 * v2y0 + K_COLLISION_RESTITUTION * m2 * (v2y0 - v1y0)) / (m1 + m2);

                    v2x1 = 1.0f * (m1 * v1x0 + m2 * v2x0 + K_COLLISION_RESTITUTION * m1 * (v1x0 - v2x0)) / (m1 + m2);
                    v2y1 = 1.0f * (m1 * v1y0 + m2 * v2y0 + K_COLLISION_RESTITUTION * m1 * (v1y0 - v2y0)) / (m1 + m2);

                    state_next.p_vel[ip] = vec2(v1x1, v1y1);
                    state_next.p_vel[c_res.target_index] = vec2(v2x1, v2y1);
                }
                vec2 jitter = vec2(random(-1, 1), random(-1, 1));
                c_res.pos += jitter * 0.1f;
                state_next.p_pos[ip] = c_res.pos;

                ivec2 coord = f2i(c_res.pos);
                if (!in_bound(coord)) {
                    state_next.p_type[ip] = ParticleType::None;
                }
            }
        }
#pragma endregion

#pragma region 完成帧
        struct ReorderBuffer {
            vector<int> p_idx; // 记录各个粒子对应的画布下标
            vector<int> sort; // 初始时为0..particles-1，根据画布下标排序
        } reorder_buf;

        // 完成StateNext的所有计算，将结果收集到StateCur中
        void complete() {
            reorder_buf.p_idx.clear();
            reorder_buf.sort.clear();
            for (int ip = 0; ip < state_next.particles; ip++) {
                ivec2 pos = f2i(state_next.p_pos[ip]);
                if (pos == ivec2(35, 28)) {
                    int a = 1;
                }
                reorder_buf.p_idx.push_back(idx(pos));
                if (state_next.p_type[ip] != ParticleType::None) {
                    reorder_buf.sort.push_back(ip);
                }
            }

            vector<int>& p_idx = reorder_buf.p_idx;
            sort(reorder_buf.sort.begin(), reorder_buf.sort.end(), [&p_idx](int i1, int i2) { return p_idx[i1] < p_idx[i2]; });

            int n_new = reorder_buf.sort.size();
            // 使用刚才的StateNext，生成下一个StateCur

            state_cur.reset(n_new);
            for (int ip = 0; ip < n_new; ip++) {
                int old_ip = reorder_buf.sort[ip];
                if (old_ip == 59) {
                    int a = 1;
                }
                // 复制数据
                vec2 pos = state_next.p_pos[old_ip];
                state_cur.p_pos[ip] = pos;
                state_cur.p_type[ip] = state_next.p_type[old_ip];
                state_cur.p_vel[ip] = state_next.p_vel[old_ip];
                state_cur.p_heat[ip] = state_next.p_heat[old_ip];
                // 构造画布索引
                PixelParticleList& cur_lst = state_cur.map_index[idx(f2i(pos))];
                cur_lst.append(ip);

                if (state_cur.p_type[ip] == ParticleType::Water) {
                    BlockLiquidList& cur_liquid_lst = state_cur.map_block_liquid[idx_liquid(f2i(pos))];
                    cur_liquid_lst.idx_lp.push_back(ip);
                }
            }

        }
#pragma endregion

#pragma region 交互

        ParticleBrush cur_particle_brush;
        void handle_new_particles() {
            // 扩大数组，将新粒子追加到state_next尾部
            if (cur_particle_brush.type != ParticleType::None) {
                ivec2 center = f2i(cur_particle_brush.center);
                int r_find = cur_particle_brush.radius + 1;
                for (int x = center.x - r_find; x <= center.x + r_find; x++) {
                    for (int y = center.y - r_find; y <= center.y + r_find; y++) {
                        if (in_bound(x, y) && glm::distance(vec2(x, y), cur_particle_brush.center) <= cur_particle_brush.radius) {
                            if (state_cur.map_index[idx(ivec2(x, y))].nil()) {
                                state_next.particles++;
                                vec2 jitter = vec2(random(-1, 1), random(-1, 1)) * 0.2f;
                                state_next.p_pos.push_back(vec2(x, y) + jitter);
                                state_next.p_type.push_back(cur_particle_brush.type);
                                state_next.p_vel.push_back(vec2());
                                state_next.p_heat.push_back(25);
                            }
                        }
                    }
                }
                cur_particle_brush.type = ParticleType::None;
            }
        }

        bool has_heat_brush = false;
        HeatBrush cur_heat_brush;
        void handle_change_heat() {
            if (has_heat_brush) {
                ivec2 center = f2i(cur_heat_brush.center);
                int r_find = cur_heat_brush.radius + 1;
                for (int y = center.y - r_find; y <= center.y + r_find; y++) {
                    for (int x = center.x - r_find; x <= center.x + r_find; x++) {
                        if (in_bound(x, y) && glm::distance(vec2(x, y), cur_heat_brush.center) <= cur_heat_brush.radius) {
                            PixelParticleList lst = state_cur.map_index[idx(ivec2(x, y))];
                            if (!lst.nil()) {
                                for (int ip = lst.from; ip <= lst.to; ip++) {
                                    state_next.p_heat[ip] += (cur_heat_brush.increase ? 1 : -1) * K_HEAT_DELTA;
                                }
                            }
                        }
                    }
                }
                has_heat_brush = false;
            }
        }

#pragma endregion

        Array2D<float> pressure;

    public:
        GameModel() : state_cur(width * height), state_next(), pressure(height, width) { //width(w), height(h), 
            assert(width % K_AIRFLOW_DOWNSAMPLE == 0);
            assert(height % K_AIRFLOW_DOWNSAMPLE == 0);
            assert(width % K_LIQUID_GRID_DOWNSAMPLE == 0);
            assert(height % K_LIQUID_GRID_DOWNSAMPLE == 0);

            //pressure = new float* [height];
            ///*heat = new float* [height];*/
            //for (int i = 0; i < height; i++)pressure[i] = new float[width]();
            /*	for (int i = 0; i < height; i++)heat[i] = new float [width]();*/

            airflow_solver.init(height / K_AIRFLOW_DOWNSAMPLE, width / K_AIRFLOW_DOWNSAMPLE, K_DT);
            airflow_solver.reset();
        };


        void update() {
            frame_counter++;

            Timer t;

            if (frame_counter == 161) {
                int debug = 1;
            }
            prepare();


            compute_heat();
            compute_vel();
            compute_air_flow();

            compute_position();
            handle_change_heat();
            handle_new_particles();
            complete();

            cout << "frametime: " << t.ms() << endl;
        }

        void set_new_particles(ParticleBrush brush) {
            cur_particle_brush = brush;
        }

        void set_heat(HeatBrush brush) {
            has_heat_brush = true;
            cur_heat_brush = brush;
        }

        struct QueryParticleResult {
            const vector<ParticleType>& type;
            const vector<vec2>& position;
            const vector<float>& temperature;
        };

        QueryParticleResult query_particles() {
            return QueryParticleResult{ state_cur.p_type, state_cur.p_pos, state_cur.p_heat };
        }

        const Array2D<float>& query_pressure() {
            //Timer t;
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    pressure[j][i] = bilinear_sample_air_p(ivec2(i, j));
                }
            }
            //cout << t.ms() << endl;
            return pressure;
        }

    };
}