#include "common/event.h"
#include "view/game_view.h"
#include "view_model/game_view_model.h"

using namespace Simflow;
const int msize = 512;
int main() {
	GameWindow win;
    GameModel<msize, msize> gm;
    GameViewModel gvm(&gm);

    win.event_update += gvm.on_update;
    win.event_new_particles += gvm.on_new_particles;
    win.event_change_heat += gvm.on_change_heat;

	gvm.event_frame_ready += win.on_frame_ready;

	win.OnCreate();
}