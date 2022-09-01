#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	class JumpComponent;
	class PhysicsBodyComponent;

	class ControllerComponent : public ProcessComponent
	{
	public:
		ControllerComponent(Actor& _actor);
	};
}