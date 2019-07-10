#pragma once
#include "../common/particle.h"
#include "../common/event.h"
#include "../common/ping_pong.h"
#include "air_solver.h"
#include "constant.h"
#include <algorithm>
#include "utility.h"
#include <vector>
#include <queue>

namespace T {
    using namespace std;

    // 示意代码
    class GameModel {
    public:

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

        // 由于我们的程序是计算密集的，因此这里不用结构体数组
        // 而用更原始的单列的数组，出于两点原因：
        // 1. 按列存储访问效率更高；
        // 2. 迭代时必须使用Jacobi迭代（计算此帧的结果，必须访问上一帧周围的结果），
        //    因此需要两个数组来做Ping-Pong buffer，最小化拷贝量。

        struct StateCur {
            int particles = 0;
            vector<PixelParticleList> map_index; // 画布某个位置的粒子下标 map_index[idx(r,c)]
            vector<ParticleType> p_type;
            vector<float> p_heat;
            vector<vec2> p_pos, p_vel;
            StateCur(int n_map) : map_index(n_map) {}
            void reset(int n) {
                particles = n;
                p_type.resize(n);
                p_pos.resize(n);
                p_vel.resize(n);
                p_heat.resize(n);
                for (auto& lst : map_index) {
                    lst = PixelParticleList();
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


        int width, height;

        bool in_bound(int c, int r) { return r >= 0 && r < height && c >= 0 && c < width; }
        bool in_bound(ivec2 v) { return in_bound(v.x, v.y); }
        int bound_dist(ivec2 v) {
            return min({ v.x, width - 1 - v.x, v.y, height - 1 - v.y });
        }
        int idx(int c, int r) { return r * width + c; }
        int idx(ivec2 v) { return idx(v.x, v.y); }
        int idx_air(int c, int r) { return r / K_AIRFLOW_DOWNSAMPLE * (width / K_AIRFLOW_DOWNSAMPLE) + c / K_AIRFLOW_DOWNSAMPLE; }
        int idx_air(ivec2 v) { return idx_air(v.x, v.y); }

        void prepare() {
            state_next.reset(state_cur.particles);
            for (int i = 0; i < state_cur.particles; i++) {
                state_next.p_type[i] = state_cur.p_type[i];
                state_next.p_pos[i] = state_cur.p_pos[i];
                state_next.p_vel[i] = state_cur.p_vel[i];
                state_next.p_heat[i] = state_next.p_heat[i];
            }
        }

        vec2 safe_sample_air(ivec2 p_air) {
            if (p_air.x < 0) p_air.x = 0;
            if (p_air.x >= width / K_AIRFLOW_DOWNSAMPLE) p_air.x = width / K_AIRFLOW_DOWNSAMPLE - 1;
            if (p_air.y < 0) p_air.y = 0;
            if (p_air.y >= height / K_AIRFLOW_DOWNSAMPLE) p_air.y = height / K_AIRFLOW_DOWNSAMPLE - 1;
            int im_air = p_air.y * (width / K_AIRFLOW_DOWNSAMPLE) + p_air.x;
            return vec2(airflow_solver.getVX()[im_air], airflow_solver.getVY()[im_air]);
        }

        vec2 bilinear_sample_air(ivec2 pos) {
            pos -= ivec2(K_AIRFLOW_DOWNSAMPLE) / 2;
            ivec2 base = pos / K_AIRFLOW_DOWNSAMPLE;
            vec2 fr = glm::fract(vec2(pos) / float(K_AIRFLOW_DOWNSAMPLE));
            vec2 p[4] = {
                safe_sample_air(base),
                safe_sample_air(base + ivec2(1,0)),
                safe_sample_air(base + ivec2(0,1)),
                safe_sample_air(base + ivec2(1,1))
            };
            float w[4] = {
                (1 - fr.x) * (1 - fr.y),
                fr.x * (1 - fr.y),
                (1 - fr.x) * fr.y,
                fr.x * fr.y
            };
            vec2 sum = vec2();
            for (int i = 0; i < 4; i++) {
                sum += p[i] * w[i];
            }
            return sum;
        }


		float average_heat(ivec2 ipos_near) {
			if (in_bound(ipos_near)) {//是否在画布里
				int im1 = idx(ipos_near);//得到它在画布上的index
				PixelParticleList &list = state_cur.map_index[im1];//找到该像素点的所有粒子编号
				if (!list.nil()) {//该像素点不为空
					float avg = 0.0f;
					for (int i = list.from; i <= list.to; i++) {
						avg += state_cur.p_heat[i];
					}
					avg /= (list.to - list.from);
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


		void compute_heat() {
			// TODO:
			// 对于每个粒子，查找其附近的粒子，计算下一帧的温度
			//对于每个粒子，计算其温度简化为其自身温度和加上上下左右粒子温度差值的平均值
			for (int ip = 0; ip < state_cur.particles; ip++) {
				//get map index
				ivec2 ipos = f2i(state_cur.p_pos[ip]);
				int im = idx(ipos);
				ivec2 ipos1 = ivec2(ipos.x, ipos.y - 1);
				ivec2 ipos2 = ivec2(ipos.x, ipos.y + 1);
				ivec2 ipos3 = ivec2(ipos.x - 1, ipos.y);
				ivec2 ipos4 = ivec2(ipos.x + 1, ipos.y);
				float t1 = average_heat(ipos1);
				float t2 = average_heat(ipos2);
				float t3 = average_heat(ipos3);
				float t4 = average_heat(ipos4);
				float delt_t = t1 + t2 + t3 + t4 - 4 * state_cur.p_heat[ip];
				state_cur.p_heat[ip] = K_DT * particle_diff(state_cur.p_type[ip]) * delt_t + state_cur.p_heat[ip];
			}
		}

        void compute_vel() {
            compute_vel_basic();
            compute_vel_liquid();
        }

        void compute_vel_basic() {
            for (int ip = 0; ip < state_cur.particles; ip++) {
                state_next.p_vel[ip] = state_cur.p_vel[ip];


                // get map index
                ivec2 ipos = f2i(state_cur.p_pos[ip]);
                int im = idx(ipos);
                int im_air = idx_air(ipos);

                //vec2 v_air = vec2(airflow_solver.getVX()[im_air], airflow_solver.getVY()[im_air]); // air velocity
                vec2 v_air = bilinear_sample_air(ipos);

                vec2 v_p = state_cur.p_vel[ip]; // particle velocity
                vec2 v_rel = v_p - v_air; // relative velocity
                float p = airflow_solver.p[im_air]; // air pressure
                p = glm::max(0.f, 1 + p / 5);
                float mass = particle_mass(state_cur.p_type[ip]);

                vec2 f_resis = -K_AIR_RESISTANCE * p * v_rel * length(v_rel);
                float limit = length(f_resis / mass * K_DT) / length(v_rel);
                if (limit > 1) f_resis /= limit; // IMPORTANT: prevent numerical explosion

                vec2 f_gravity = K_GRAVITY * vec2(0, 1) * mass;
                vec2 f = f_resis + f_gravity;
                vec2 acc = f / mass;

                state_next.p_vel[ip] += acc * K_DT; // set velocity
            }
        }

        struct LiquidBuffer {
            vector<int> idx_mapping;
            vector<float> rho, p;
            void reset(int n) {
                idx_mapping.resize(n);
                rho.resize(n);
                p.resize(n);
            }
        } liquid_buf;

        template<typename F>
        void iterate_neighbor_particles(ivec2 pos, int r_neibor, F & f) {
            for (int dx = -r_neibor; dx <= r_neibor; dx++) {
                for (int dy = -r_neibor; dy <= r_neibor; dy++) {
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


        void compute_vel_liquid() {
            // 1. 所有粒子计算SPH应力（优化：液体附近粒子）
            // 2. 各个粒子加速度累加到state_next上
            int r_neibor = f2i(ceilf(H));
            liquid_buf.reset(0);
            int il = 0;
            for (int ip = 0; ip < state_cur.particles; ip++) {
                if (state_cur.p_type[ip] == ParticleType::Water) {
                    ivec2 pos = f2i(state_cur.p_pos[ip]);
                    float p = 0, rho = 0; // 计算p、rho
                    iterate_neighbor_particles(pos, r_neibor, [this, &p, &rho, &ip](int t_ip) {
                        if (this->state_cur.p_type[t_ip] != ParticleType::Water) return;
                        vec2 pos_diff = this->state_cur.p_pos[t_ip] - this->state_cur.p_pos[ip];
                        float r2 = dot(pos_diff, pos_diff);
                        if (r2 < HSQ) {
                            float t_mass = particle_mass(this->state_cur.p_type[t_ip]);
                            rho += t_mass * POLY6 * pow(HSQ - r2, 3.f);
                        }
                    });
                    p = GAS_CONST * (rho - REST_DENS);
                    liquid_buf.idx_mapping.push_back(il);
                    liquid_buf.p.push_back(p);
                    liquid_buf.rho.push_back(rho);
                    il++;
                }
                else {
                    liquid_buf.idx_mapping.push_back(-1);
                }
            }

            for (int ip = 0; ip < state_cur.particles; ip++) {
                ivec2 pos = f2i(state_cur.p_pos[ip]);
                float mass = particle_mass(state_cur.p_type[ip]);
                iterate_neighbor_particles(pos, r_neibor, [this, mass, &ip](int t_ip) {
                    vec2 f_press = vec2();
                    vec2 f_visc = vec2();
                    if (t_ip == ip) return;
                    if (this->state_cur.p_type[t_ip] != ParticleType::Water) return;

                    vec2 pos_diff = this->state_cur.p_pos[t_ip] - this->state_cur.p_pos[ip];
                    vec2 vel_diff = this->state_cur.p_vel[t_ip] - this->state_cur.p_vel[ip];
                    float t_mass = particle_mass(this->state_cur.p_type[t_ip]);
                    float r = length(pos_diff);
                    if (r <= EPS) {
                        // 防止normalize零向量
                        // 此处随机给一个方向
                        pos_diff = vec2(random(-1, 1), random(-1, 1));
                    }
                    if (r < H)
                    {
                        int t_il = liquid_buf.idx_mapping[t_ip];
                        // compute pressure force contribution
                        f_press += -normalize(pos_diff) * t_mass * liquid_buf.p[t_il] / liquid_buf.rho[t_il] * SPIKY_GRAD * pow(H - r, 2.f);
                        //// compute viscosity force contribution
                        f_visc += VISC * t_mass * vel_diff / (liquid_buf.rho[t_il]) * VISC_LAP * (H - r);

                        vec2 f = f_press + f_visc;
                        vec2 acc = vec2();
                        if (this->state_cur.p_type[ip] == ParticleType::Water) {
                            int il = liquid_buf.idx_mapping[ip];
                            acc = f / liquid_buf.rho[il];
                        }
                        else {
                            acc = f / mass;
                        }
                        this->state_next.p_vel[ip] += acc * K_DT;
                    }

                });
            }
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
            //airflow_solver.vortConfinement();
        }

        struct CollisionDetectionResult {
            vec2 pos;
            int target_index;
        };

        // 检测碰撞（粗糙）
        // 从从终点开始往前寻找，因此可能会穿过薄物体，但是比较快
        bool detect_collision(vec2 start, vec2 end, bool ignore_liquid, CollisionDetectionResult & result) {
            //int last_target = -1;
            //vec2 final_pos = end;
            //goto exit;
            //float len = length(start - end);
            //if (len == 0.0f) goto exit;
            int last_target = -1;
            vec2 final_pos = start;
            float len = length(start - end);
            if (len == 0.0f) goto exit;

            int steps = len / K_COLLISION_STEP_LENGTH;
            vec2 delta = normalize(start - end) * K_COLLISION_STEP_LENGTH; // 步长=1


            vec2 cur = end;
            ivec2 self = f2i(start);

            if (steps == 0) {
                steps = 1;
            }

            for (int i = 0; i < steps; i++) {
                ivec2 m_pos = f2i(cur);
                bool ext = true;
                if (in_bound(m_pos)) {
                    PixelParticleList lst = state_cur.map_index[idx(m_pos)];
                    if (lst.nil() || self == m_pos) {
                    }
                    else {
                        int t = random_sample(lst.from, lst.to);
                        if (state_cur.p_type[t] == ParticleType::Water && ignore_liquid) {
                        }
                        else {
                            ext = false;
                            last_target = t;
                        }
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
            // 更新位置，碰撞检测
            for (int ip = 0; ip < state_cur.particles; ip++) {
                ParticleType cur_type = state_cur.p_type[ip];
                if (cur_type == ParticleType::Iron) continue;

                vec2 v = state_cur.p_vel[ip];
                vec2 pos_old = state_cur.p_pos[ip];
                vec2 pos_new = pos_old + v * K_DT;
                CollisionDetectionResult c_res;


                bool collided = detect_collision(pos_old, pos_new, cur_type == ParticleType::Water, c_res);
                if (collided) {
                    ParticleType target_type = state_cur.p_type[c_res.target_index];

                    if (target_type != ParticleType::Iron) {
                        float rnd1 = rand() / float(RAND_MAX);
                        float rnd2 = rand() / float(RAND_MAX);
                        vec2 rnd = vec2(rnd1, rnd2) * 0.1f;
                        // TODO: 更好的速度计算
                        swap(state_next.p_vel[c_res.target_index], state_next.p_vel[ip]);
                        state_next.p_vel[c_res.target_index] += rnd;
                        state_next.p_vel[ip] -= rnd;
                    }
                    else if (target_type == ParticleType::Iron) {
                        // DIRTY HACK HERE
                        // TODO: REMOVE IT
                        state_next.p_vel[ip] = -0.5f * state_next.p_vel[ip];
                    }
                }

                state_next.p_pos[ip] = c_res.pos;

                ivec2 coord = f2i(c_res.pos);
                if (!in_bound(coord)) {
                    state_next.p_type[ip] = ParticleType::None;
                }
            }
        }

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
            }

        }

        ParticleBrush cur_brush;
        void handle_new_particles() {
            // 扩大数组，将新粒子追加到state_next尾部
            if (cur_brush.type != ParticleType::None) {
                ivec2 center = f2i(cur_brush.center);
                int r_find = cur_brush.radius + 1;
                for (int x = center.x - r_find; x <= center.x + r_find; x++) {
                    for (int y = center.y - r_find; y <= center.y + r_find; y++) {
                        if (in_bound(x, y) && glm::distance(vec2(x, y), cur_brush.center) <= cur_brush.radius) {
                            if (state_cur.map_index[idx(ivec2(x, y))].nil()) {
                                state_next.particles++;
                                vec2 jitter = vec2(random(-1, 1)) * 0.05f;
                                state_next.p_pos.push_back(vec2(x, y) + jitter);
                                state_next.p_type.push_back(cur_brush.type);
                                state_next.p_vel.push_back(vec2());
                                state_next.p_heat.push_back(0);
                            }
                        }
                    }
                }
                cur_brush.type = ParticleType::None;
            }
        }

    public:
        GameModel(int w, int h) : width(w), height(h), state_cur(w * h), state_next() {
            assert(w % K_AIRFLOW_DOWNSAMPLE == 0);
            assert(h % K_AIRFLOW_DOWNSAMPLE == 0);
            airflow_solver.init(h / K_AIRFLOW_DOWNSAMPLE, w / K_AIRFLOW_DOWNSAMPLE, K_DT);
            airflow_solver.reset();
        };

        void update() {
            prepare();
            compute_heat();
            compute_vel();
            compute_position();
            compute_air_flow();
            handle_new_particles();
            complete();
        }

        void set_new_particles(ParticleBrush brush) {
            cur_brush = brush;
        }

        void set_heat(HeatBrush brush) {
            // TODO
        }

        struct QueryParticleResult {
            const vector<ParticleType>& type;
            const vector<vec2>& position;
			const vector<float>& temperature;
        };

        QueryParticleResult query_particles() {
            return QueryParticleResult{ state_cur.p_type, state_cur.p_pos, state_cur.p_heat};
        }
    };
}