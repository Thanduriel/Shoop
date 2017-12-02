#include "actor.hpp"

namespace Game {

	Actor::Actor(Math::Vec2 _position, float _rotation, Math::Vec2 _scale)
		: Transformation(_position, _rotation, _scale),
		m_isDestroyed(false)
	{}
}