#pragma once
#include "../../common/event.h"
#include "../../common/particle.h"
#include <vector>

namespace T {
	class GameView;

	class DataReadyEventHandler : public EventHandler<const vector<ParticleInfo>&> {
		GameView* view;
	public:
		DataReadyEventHandler(GameView* v) : view(v) {}
		void handle(const vector<ParticleInfo>& particles);
	};
}