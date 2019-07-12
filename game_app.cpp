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

    gvm.event_data_ready += win.on_data_ready;
	gvm.event_heat_ready += win.on_heat_ready;
	gvm.event_pressure_ready += win.on_pressure_ready;

	win.OnCreate();
}