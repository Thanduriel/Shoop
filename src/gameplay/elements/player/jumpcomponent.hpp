#pragma once

#include "gameplay/elements/physicscomponent.hpp"

namespace Game {

	class JumpComponent : public ProcessComponent
	{
	public:
		JumpComponent(Actor& _actor, PhysicsBodyComponent& _targetBody);

		void Process(float _deltaTime) override;
		void Jump();
		void SetCharging(bool _charge);
		float GetChargeLevel() const { return m_charge / m_maxCharge; }
	private:
		float m_maxCharge;
		float m_charge;
		bool m_charging;

		float m_maxStrength;

		PhysicsBodyComponent& m_targetBody;
	};
}