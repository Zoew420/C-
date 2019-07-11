#include "common/event.h"
#include "view/game_view.h"
#include "view_model/game_view_model.h"

using namespace T;
const int msize = 256;
int main() {
	GameWindow win;
    GameModel gm(msize, msize);
    GameViewModel gvm(&gm);

    win.event_update += gvm.on_update;
    win.event_new_particles += gvm.on_new_particles;
    win.event_change_heat += gvm.on_change_heat;

    gvm.event_data_ready += win.on_data_ready;

	win.OnCreate();
}