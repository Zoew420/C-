#pragma once
#include "../common/event.h"
#include "../common/particle.h"
#include "../model/game_model.h"

namespace T {
    using namespace std;


    class GameViewModel {
        GameModel* model;

        vector<ParticleInfo> data_buffer;
        void trigger_data_ready() {
            data_buffer.clear();
            auto result = model->query_particles();
            for (int i = 0; i < result.position.size(); i++) {
                data_buffer.push_back(ParticleInfo{ result.type[i],result.position[i] });
            }
            event_data_ready.trigger(data_buffer);
        }

    public:
        // ����׼����ϵ��¼�Դ��ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // �����¼��������View֪ͨViewModel�����߼����£�
        shared_ptr<EventHandler<>> on_update = function_handler(function(
            [this]() {
            this->model->update();
            this->trigger_data_ready();
        }));

        // �����������¼��������View֪ͨViewModel���������ӣ�
        shared_ptr<EventHandler<ParticleBrush>> on_new_particles = function_handler(function(
            [this](ParticleBrush brush) {
            this->model->set_new_particles(brush);
        }));
    };

}