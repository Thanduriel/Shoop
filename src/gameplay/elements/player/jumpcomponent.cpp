#include "jumpcomponent.hpp"
#include "math/functions.hpp"

namespace Game {

	constexpr static float DEFAULT_MAX_STRENGTH = 4.f;
	constexpr static float DEFAULT_CHARGE_TIME = 4.f;

	JumpComponent::JumpComponent(Actor& _actor, PhysicsBodyComponent& _targetBody)
		: ProcessComponent(_actor),
		m_targetBody(_targetBody),
		m_maxCharge(DEFAULT_CHARGE_TIME),
		m_charge(DEFAULT_CHARGE_TIME),
		m_maxStrength(DEFAULT_MAX_STRENGTH)
	{

	}

	void JumpComponent::Process(float _deltaTime)
	{
		m_charge = std::max(m_maxCharge, m_charge + _deltaTime);
	}

	void JumpComponent::Jump()
	{
		Math::Quadratic fn;
		const Math::Vec2 v = Math::Vec2(0.f, fn(m_charge / m_maxCharge)*m_maxStrength);
		m_targetBody.Get().ApplyLinearImpulseToCenter( m_actor.GetRotated(v), true);

		m_charge = 0.f;
	}
}