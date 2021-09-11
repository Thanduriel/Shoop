#include "jumpcomponent.hpp"
#include "math/functions.hpp"

namespace Game {

	constexpr static float DEFAULT_MAX_STRENGTH = 2.87f;
	constexpr static float DEFAULT_CHARGE_TIME = 1.05f;

	JumpComponent::JumpComponent(Actor& _actor, PhysicsBodyComponent& _targetBody)
		: ProcessComponent(_actor),
		m_targetBody(_targetBody),
		m_maxCharge(DEFAULT_CHARGE_TIME),
		m_charge(0.f),
		m_maxStrength(DEFAULT_MAX_STRENGTH),
		m_charging(true)
	{

	}

	void JumpComponent::Process(float _deltaTime)
	{
		if (m_charging)
			m_charge = std::min(m_maxCharge, m_charge + _deltaTime);
	}

	void JumpComponent::Jump()
	{
	//	Math::Quadratic fn;
	//	const Math::Vec2 v = Math::Vec2(0.f, fn(GetChargeLevel())*m_maxStrength);
		const Math::Vec2 v = Math::Vec2(0.f, GetChargeLevel() * m_maxStrength);
		m_targetBody.Get().ApplyLinearImpulseToCenter( m_actor->GetRotated(v), true);
		m_charge = 0.f;
	}

	void JumpComponent::SetCharging(bool _charge)
	{
		m_charging = _charge;
	}
}