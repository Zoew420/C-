#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameViewModel {
    public:
        // ����׼����ϵ��¼�Դ��ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // �����¼��������View֪ͨViewModel�����߼����£�
        // TODO: �ڹ��캯����Ϊ�丳ֵ
        shared_ptr<EventHandler<>> on_update;

        // �����������¼��������View֪ͨViewModel���������ӣ�
        // TODO: �ڹ��캯����Ϊ�丳ֵ
        shared_ptr<EventHandler<ParticleBrush>> on_new_particles;
    };

}