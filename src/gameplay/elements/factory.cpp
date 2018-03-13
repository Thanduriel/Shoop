#include "factory.hpp"
#include "gameplay/core/actor.hpp"

namespace Game {

	FactoryComponent::FactoryComponent(Actor& _actor, Math::Vec2 _position, float _rotation)
		: Component(_actor, Component::Type::Factory),
		Math::Transformation(_position, _rotation)
	{}

	void FactoryComponent::Add(Actor& _actor)
	{
		m_createdActors.emplace_back(&_actor);
	}
}