#pragma once
#include "../../common/event.h"
#include "../../common/particle.h"
#include <vector>

namespace T {
	class GameView;

	class PressureReadyEventHandler : public EventHandler<const vector<vector<float>>&>
	{
		GameView* view;
	public:
		PressureReadyEventHandler(GameView* v) : view(v) {}
		void handle(const vector<vector<float>>& pressure);
	};
}