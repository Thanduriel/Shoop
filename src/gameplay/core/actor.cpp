#include "actor.hpp"
#include "component.hpp"

namespace Game {

	Actor::Actor(Math::Vec2 _position, float _rotation, Math::Vec2 _scale)
		: Transformation(_position, _rotation, _scale),
		m_isDestroyed(false),
		m_handle(std::make_shared<Actor*>(this))
	{}

	Actor::~Actor()
	{
		*m_handle = nullptr;
		for (Component* comp : m_components)
			comp->m_actor = nullptr;
	}
}