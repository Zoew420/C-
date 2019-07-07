#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameViewModel {
    public:
        // 数据准备完毕的事件源（ViewModel通知View数据准备完毕，可以绘制）
        EventSource<const vector<ParticleInfo>&> event_data_ready;

        // 更新事件处理程序（View通知ViewModel进行逻辑更新）
        // TODO: 在构造函数内为其赋值
        shared_ptr<EventHandler<>> on_update;

        // 绘制新粒子事件处理程序（View通知ViewModel绘制新粒子）
        // TODO: 在构造函数内为其赋值
        shared_ptr<EventHandler<ParticleBrush>> on_new_particles;
    };

}