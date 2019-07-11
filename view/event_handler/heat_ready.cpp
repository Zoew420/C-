#include "heat_ready.h"
#include "../game_view.h"

namespace Simflow {
	void HeatReadyEventHandler::handle(const std::vector<ParticleInfo>& heat)
	{
		view->Handler_Heat(heat);
	}
}
