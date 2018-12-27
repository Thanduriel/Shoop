#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	class JumpComponent;
	class PhysicsBodyComponent;

	class ControllerComponent : public ProcessComponent
	{
	public:
		ControllerComponent(Actor& _actor);

	protected:
		JumpComponent& m_jump;
		PhysicsBodyComponent& m_headBody;
		PhysicsBodyComponent& m_wheelBody;
	};
}