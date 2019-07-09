#include "game_mouseclickEventHandle.h"
#include"../game_view.h"

namespace T {
	void MouseclickEventHandle::handle(const glm::vec2 & point)
	{
		view->UpdataParticles(point);
	}
}
