#pragma once
#include "../common/event.h"
#include "../common/particle.h"
#include "../model/game_model.h"

namespace Simflow {
    using namespace std;


    class GameViewModel {
        GameModel* model;

        vector<ParticleInfo> data_buffer;
        void trigger_data_ready() {
            data_buffer.clear();
            auto result = model->query_particles();
            for (int i = 0; i < result.position.size(); i++) {
                data_buffer.push_back(ParticleInfo{ result.type[i],result.position[i],result.temperature[i] });
            }
            event_data_ready.trigger(data_buffer);
        }

    public:

        GameViewModel(GameModel* model) : model(model) {};

        // 数据准备完毕的事件源（ViewModel通知View数据准备完毕，可以绘制）
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // 更新事件处理程序（View通知ViewModel进行逻辑更新）
        shared_ptr<EventHandler<>> on_update = function_handler(function(
            [this]() {
            this->model->update();
            this->trigger_data_ready();
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