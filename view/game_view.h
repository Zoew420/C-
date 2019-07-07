#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameView {
    public:
        EventSource<> event_update;
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
    };
}