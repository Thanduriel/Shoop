#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	class JumpComponent;
	class PhysicsBodyComponent;

	class ControllerComponent : public ProcessComponent
	{
	public:
		ControllerComponent(Actor& _actor, JumpComponent& _jump, 
			PhysicsBodyComponent& _wheelBody);

		void Process(float _deltaTime) override;

	private:
		JumpComponent& m_jump;
		PhysicsBodyComponent& m_wheelBody;
	};
}