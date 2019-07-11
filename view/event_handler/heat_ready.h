#pragma once
#include "../../common/event.h"
#include "../../common/particle.h"
#include <vector>

namespace T {
	class GameView;

	class HeatReadyEventHandler : public EventHandler<const vector<ParticleInfo>&>
	{
		GameView *view;
	public:
		HeatReadyEventHandler(GameView *v): view(v){}
		void handle(const std::vector<ParticleInfo>& heat);
	};
}