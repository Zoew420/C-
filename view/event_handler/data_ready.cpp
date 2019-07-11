#include "data_ready.h"
#include "../game_view.h"

namespace Simflow {
	void DataReadyEventHandler::handle(const std::vector<ParticleInfo>& particles)
	{
		view->Handler(particles);
	}
}
