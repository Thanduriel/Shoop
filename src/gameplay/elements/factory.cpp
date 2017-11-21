#include "factory.hpp"
#include "gameplay/core/actor.hpp"

namespace Game {

	FactoryComponent::FactoryComponent(Actor& _actor)
		: Component(_actor, Component::Type::Factory)
	{}

	void FactoryComponent::Add(Actor& _actor)
	{
		m_createdActors.emplace_back(&_actor);
	}
}