#pragma once
#include"../../common/event.h"
#include"../../common/particle.h"
#include"../../glm/glm.hpp"

namespace T {
	class GameView;

	class MouseclickEventHandle : public EventHandler<const vec2&>
	{
		GameView* view;
	public:
		MouseclickEventHandle(GameView* v) : view(v) {}
		void handle(const vec2& point);
	};
}