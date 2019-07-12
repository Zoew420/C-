#include "frame_ready.h"
#include "../game_view.h"

namespace Simflow {
	void FrameReadyEventHandler::handle(FrameData data)
	{
		view->Handler(data);
	}
}