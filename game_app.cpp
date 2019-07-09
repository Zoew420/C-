#include "common/event.h"
#include "view/game_view.h"
#include "view_model/game_view_model.h"

using namespace T;
const int msize = 128;
int main() {
	GameWindow win;
    GameModel gm(msize, msize);
    GameViewModel gvm(&gm);
    GameView gv;
    gv.event_update += gvm.on_update;
    gv.event_new_particles += gvm.on_new_particles;
    gvm.event_data_ready += gv.on_data_ready;
	win.OnCreate();
}