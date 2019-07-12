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

        // TODO: �Ƴ�
        vector<ParticleInfo> gvh;

        // TODO: �Ƴ�
        vector<vector<float>> gvp;//pressure of 2 dimension(height * width)

         // TODO: �Ƴ�
        void trigger_heat_ready() {
            gvh.clear();
            auto heat = model->query_particles();
            for (int i = 0; i < heat.temperature.size(); i++) {
                gvh.push_back(ParticleInfo{ heat.type[i],heat.position[i],heat.temperature[i] });
            }
            event_heat_ready.trigger(gvh);
        }

        // TODO: �Ƴ�
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

        // TODO: �Ƴ�
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

        // TODO: �µ��¼�Դ������һ֡�ڵ���������
        EventSource<FrameData> event_frame_ready;

        // ����׼����ϵ��¼�Դ��ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
                // TODO: �Ƴ��ϵ��¼�Դ
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // TODO: �Ƴ��ϵ��¼�Դ
        EventSource<const vector<vector<float>>&> event_pressure_ready;

        // TODO: �Ƴ��ϵ��¼�Դ
        EventSource<const vector<ParticleInfo>&> event_heat_ready;


        // �����¼��������View֪ͨViewModel�����߼����£�
        shared_ptr<EventHandler<>> on_update = function_handler(function(
            [this]() {
            this->model->update();
            // TODO: �Ƴ��ϵ��¼�����
            Timer t;

            this->trigger_data_ready();
            this->trigger_heat_ready();
            this->trigger_pressure_ready();
            // this->trigger_frame_ready();

            cout << "trigger time: " << t.ms() << endl;
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