#pragma once
#include "../common/event.h"
#include "../common/particle.h"
#include "../model/game_model.h"

namespace Simflow {
    using namespace std;


    class GameViewModel {
        GameModel* model;

		vector<ParticleInfo> gvh;
        vector<ParticleInfo> data_buffer;
		vector<vector<float>>gvp;//pressure of 2 dimension(height * width)
		

		void trigger_heat_ready() {
			gvh.clear();
			auto heat = model->query_particles();
			for (int i = 0; i < heat.temperature.size(); i++) {
				gvh.push_back(ParticleInfo{ heat.type[i],heat.position[i],heat.temperature[i] });
			}
			event_heat_ready.trigger(gvh);
		}

		void trigger_pressure_ready() {
			auto p = model->query_pressure();
			for (int i = 0; i < model->height; i++) {
				vector<float> row;
				for (int j = 0; j < model->width; j++)row.push_back(p[i][j]);
				gvp.push_back(row);
			}
			event_pressure_ready.trigger(gvp);
		}

        void trigger_data_ready() {
            data_buffer.clear();
            auto result = model->query_particles();
            for (int i = 0; i < result.position.size(); i++) {
                data_buffer.push_back(ParticleInfo{ result.type[i],result.position[i],result.temperature[i] });
            }
            event_data_ready.trigger(data_buffer);
        }

    public:

        GameViewModel(GameModel* model) : model(model) {
		};

        // ����׼����ϵ��¼�Դ��ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        EventSource<const vector<ParticleInfo>&> event_data_ready;

		EventSource<const vector<vector<float>>&> event_pressure_ready;

		EventSource<const vector<ParticleInfo>&> event_heat_ready;


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

        // �ı��¶��¼��������
        shared_ptr<EventHandler<HeatBrush>> on_change_heat = function_handler(function(
            [this](HeatBrush brush) {
            this->model->set_heat(brush);
        }));
    };

}