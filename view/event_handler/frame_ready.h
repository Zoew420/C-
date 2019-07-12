#pragma once
#include "../../common/event.h"
#include "../../common/particle.h"
#include "../../common/parameter.h"

namespace Simflow {
	class GameView;

	class FrameReadyEventHandler : public EventHandler<FrameData>
	{
		GameView *view;
	public:
		FrameReadyEventHandler(GameView *v): view(v) {}
		void handle(FrameData data);
	};
}
