#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	class JumpComponent;
	class PhysicsBodyComponent;

	enum struct Outcome : int {
		Draw,
		Win,
		Loss
	};

	class ControllerComponent : public ProcessComponent
	{
	public:
		ControllerComponent(Actor& _actor);

		virtual void Reset(Outcome _outcome) {}
	};
}