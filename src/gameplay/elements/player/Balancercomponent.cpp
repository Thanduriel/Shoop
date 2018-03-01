#include "Balancercomponent.hpp"
#include "math/functions.hpp"

namespace Game {

	constexpr static float32 DEFAULT_GRAVITY = -9.81f;

	BalancerComponent::BalancerComponent(Actor& _actor, PhysicsBodyComponentD& _targetBody)
		: ProcessComponent(_actor),
		m_targetb2Body(_targetBody.Get()),
		m_GForce(DEFAULT_GRAVITY),
		m_NForce(m_GForce)
	{

	}

	void BalancerComponent::Process(float _deltaTime) {}

	//Balanciert gegen die Gewichtskraft
	void BalancerComponent::VBalance()
	{
		m_GForce = DEFAULT_GRAVITY  * m_targetb2Body.GetMass();

		float32 m_VForce = -m_GForce;

		//hier Kraft mit entsprechender Funktion anpassen
		m_VForce = 1.1f - pow(m_VForce / 120.f, 2.f) + pow(m_VForce / 120.f, 4.f) * 0.3f;
		const Math::Vec2 v = Math::Vec2(0.f, m_GForce);
		m_targetb2Body.ApplyForceToCenter(v, true);
	}

	//Balanciert gegen die Normalkraft
	void BalancerComponent::OBalance()
	{
		m_GForce = DEFAULT_GRAVITY  * m_targetb2Body.GetMass();
		m_NForce = cos(m_targetb2Body.GetAngle()) * m_GForce;

		float32 m_OForce = -(sin(m_targetb2Body.GetAngle()) * m_NForce);
		//hier Kraft mit entsprechender Funktion anpassen
		m_OForce = 1.1f - pow(m_OForce / 120.f, 2.f)+ pow(m_OForce / 120.f, 4.f) * 0.3f;
		const Math::Vec2 v = Math::Vec2(sin(m_targetb2Body.GetAngle())*m_OForce, cos(m_targetb2Body.GetAngle())*m_OForce);
		m_targetb2Body.ApplyForceToCenter(v, true);
	}
}