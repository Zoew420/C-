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

        EventSource<FrameData> event_frame_ready;

        // 更新事件处理程序（View通知ViewModel进行逻辑更新）
        shared_ptr<EventHandler<>> on_update = function_handler(function(
            [this]() {
            this->model->update();
            // TODO: 移除老的事件触发
            Timer t;

            //this->trigger_data_ready();
            //this->trigger_heat_ready();
            //this->trigger_pressure_ready();
            this->trigger_frame_ready();

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