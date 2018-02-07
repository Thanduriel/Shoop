#pragma once

#include "gameplay/elements/physicscomponent.hpp"

namespace Game {

	class JumpComponent : public ProcessComponent
	{
	public:
		JumpComponent(Actor& _actor, PhysicsBodyComponent& _targetBody);

		void Process(float _deltaTime) override;
		void Jump();
	private:
		float m_maxCharge;
		float m_charge;

		float m_maxStrength;

		PhysicsBodyComponent& m_targetBody;
	};
}