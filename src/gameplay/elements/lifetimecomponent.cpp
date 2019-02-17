#include "lifetimecomponent.hpp"
#include "gameplay/core/actor.hpp"

namespace Game {

	LifetimeComponent::LifetimeComponent(Actor& _actor, float _maxTime)
		: ProcessComponent(_actor),
		m_timeMax(_maxTime),
		m_timeLeft(_maxTime)
	{
	}

	void LifetimeComponent::Process(float _deltaTime)
	{
		m_timeLeft -= _deltaTime;
		if (m_timeLeft <= 0.f) m_actor->Destroy();
	}
}