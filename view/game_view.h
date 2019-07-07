#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameView {
    public:
        // 更新事件源（View通知ViewModel进行逻辑更新）
        EventSource<> event_update;

        // 绘制新粒子事件源（View通知ViewModel绘制新粒子）
        EventSource<ParticleBrush> event_new_particles;

        // 数据准备完毕的处理函数（ViewModel通知View数据准备完毕，可以绘制）
        // TODO: 在构造函数内为其赋值
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
    };
}