#include "data_ready.h"
#include "../game_view.h"

namespace T {
	void DataReadyEventHandler::handle(const std::vector<ParticleInfo>& particles)
	{
		view->Handler(particles);
	}
}
