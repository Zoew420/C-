#pragma once
#include "../common/event.h"
#include "../common/particle.h"

namespace T {
    using namespace std;

    class GameViewModel {
    public:
        EventSource<const vector<ParticleInfo>&> event_data_ready;
        shared_ptr<EventHandler<>> on_update;
    };

}