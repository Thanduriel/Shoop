#pragma once

#include "gameplay/elements/physicscomponent.hpp"

namespace Game {

	class BalancerComponent : public ProcessComponent
	{
	public:
		BalancerComponent(Actor& _actor, PhysicsBodyComponentD& _targetBody);

		void Process(float _deltaTime) override;
		void VBalance();
		void OBalance();
	private:
		float32 m_GForce;
		float32 m_NForce;

		PhysicsBodyComponentD& m_targetBody;
	};
}
