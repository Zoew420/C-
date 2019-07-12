#include "pressure_ready.h"
#include "../game_view.h"

namespace Simflow {
	void PressureReadyEventHandler::handle(const std::vector<vector<float>>& pressure)
	{
		view->Handler_Pressure(pressure);
	}
}
