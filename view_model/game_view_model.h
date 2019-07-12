#pragma once
#include "../common/event.h"
#include "../common/particle.h"
#include "../common/parameter.h"
#include "../common/timer.h"
#include "../model/game_model.h"

namespace Simflow {
    using namespace std;

    template<int width, int height>
    class GameViewModel {
        GameModel<width, height>* model;

        vector<ParticleInfo> data_buffer;

        // TODO: 移除
        vector<ParticleInfo> gvh;

        // TODO: 移除
        vector<vector<float>> gvp;//pressure of 2 dimension(height * width)

         // TODO: 移除
        void trigger_heat_ready() {
            gvh.clear();
            auto heat = model->query_particles();
            for (int i = 0; i < heat.temperature.size(); i++) {
                gvh.push_back(ParticleInfo{ heat.type[i],heat.position[i],heat.temperature[i] });
            }
            event_heat_ready.trigger(gvh);
        }

        // TODO: 移除
        void trigger_pressure_ready() {
            gvp.clear();
            auto& p = model->query_pressure();
            for (int i = 0; i < height; i++) {
                vector<float> row;
                for (int j = 0; j < width; j++) row.push_back(p[i][j]);
                gvp.push_back(row);
            }
            event_pressure_ready.trigger(gvp);
        }

        // TODO: 移除
        void trigger_data_ready() {
            data_buffer.clear();
            auto result = model->query_particles();
            for (int i = 0; i < result.position.size(); i++) {
                data_buffer.push_back(ParticleInfo{ result.type[i],result.position[i],result.temperature[i] });
            }
            event_data_ready.trigger(data_buffer);
        }

        void trigger_frame_ready() {

            data_buffer.clear();
            auto result = model->query_particles();
            for (int i = 0; i < result.position.size(); i++) {
                data_buffer.push_back(ParticleInfo{ result.type[i], result.position[i], result.temperature[i] });
            }

            auto& pressure = model->query_pressure();
            event_frame_ready.trigger(FrameData{ data_buffer, pressure });
        }

    public:

        GameViewModel(GameModel<width, height> * model) : model(model) {}

        // TODO: 新的事件源，包含一帧内的所有数据
        EventSource<FrameData> event_frame_ready;

        // 数据准备完毕的事件源（ViewModel通知View数据准备完毕，可以绘制）
                // TODO: 移除老的事件源
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // TODO: 移除老的事件源
        EventSource<const vector<vector<float>>&> event_pressure_ready;

        // TODO: 移除老的事件源
        EventSource<const vector<ParticleInfo>&> event_heat_ready;


        // 更新事件处理程序（View通知ViewModel进行逻辑更新）
        shared_ptr<EventHandler<>> on_update = function_handler(function(
            [this]() {
            this->model->update();
            // TODO: 移除老的事件触发
            Timer t;

            this->trigger_data_ready();
            this->trigger_heat_ready();
            this->trigger_pressure_ready();
            // this->trigger_frame_ready();

            cout << "trigger time: " << t.ms() << endl;
        }));

        // 绘制新粒子事件处理程序（View通知ViewModel绘制新粒子）
        shared_ptr<EventHandler<ParticleBrush>> on_new_particles = function_handler(function(
            [this](ParticleBrush brush) {
            this->model->set_new_particles(brush);
        }));

        // 改变温度事件处理程序
        shared_ptr<EventHandler<HeatBrush>> on_change_heat = function_handler(function(
            [this](HeatBrush brush) {
            this->model->set_heat(brush);
        }));
    };

}