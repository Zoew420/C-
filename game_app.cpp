#include "common/event.h"
#include "view/game_view.h"
#include "view_model/game_view_model.h"

using namespace T;

int main() {
    GameView gv;
    GameViewModel gvm;
    gv.event_update += gvm.on_update;
    gvm.event_data_ready += gv.on_data_ready;
}