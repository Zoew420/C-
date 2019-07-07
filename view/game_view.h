#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameView {
    public:
        // �����¼�Դ��View֪ͨViewModel�����߼����£�
        EventSource<> event_update;

        // �����������¼�Դ��View֪ͨViewModel���������ӣ�
        EventSource<ParticleBrush> event_new_particles;

        // ����׼����ϵĴ�������ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        // TODO: �ڹ��캯����Ϊ�丳ֵ
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
    };
}