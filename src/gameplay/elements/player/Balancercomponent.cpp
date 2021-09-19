#include "Balancercomponent.hpp"
#include "math/functions.hpp"
#include "sheep.hpp"

namespace Game {

	constexpr static float32 DEFAULT_GRAVITY = -9.81f;

	BalancerComponent::BalancerComponent(Actor& _actor, PhysicsBodyComponent& _targetBody)
		: ProcessComponent(_actor),
		m_GForce(DEFAULT_GRAVITY),
		m_NForce(m_GForce),
		m_targetBody(_targetBody)
	{

	}

	void BalancerComponent::Process(float _deltaTime)
	{
		Sheep& sheep = static_cast<Sheep&>(*m_actor);
		if (!sheep.IsDead() && sheep.GetGroundContacts()) OBalance();
	}

	//Balanciert gegen die Gewichtskraft
	void BalancerComponent::VBalance()
	{
		m_GForce = DEFAULT_GRAVITY  * m_targetBody.Get().GetMass();

		float32 m_VForce = -m_GForce;

		//hier Kraft mit entsprechender Funktion anpassen
		m_VForce = 1.2f - pow(m_VForce / 120.f, 2.f) + pow(m_VForce / 120.f, 4.f) * 0.3f;
		const Math::Vec2 v = Math::Vec2(0.f, m_VForce);
		m_targetBody.Get().ApplyForceToCenter(v, true);
	}

	//Balanciert gegen die Normalkraft
	void BalancerComponent::OBalance()
	{
		m_GForce = DEFAULT_GRAVITY  * m_targetBody.Get().GetMass();
		m_NForce = cos(m_targetBody.Get().GetAngle()) * m_GForce;

		float32 m_OForce = -m_NForce;

		//hier Kraft mit entsprechender Funktion anpassen
		m_OForce = 1.3f - pow(m_OForce / 120.f, 2.f)+ pow(m_OForce / 120.f, 4.f) * 0.3f;
		const Math::Vec2 v = Math::Vec2(sin(m_targetBody.Get().GetAngle())*m_OForce, cos(m_targetBody.Get().GetAngle())*m_OForce);
		m_targetBody.Get().ApplyForceToCenter(v, true);
	}
}