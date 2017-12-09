#pragma once

#include "gameplay/core/actor.hpp"
#include "physicscomponent.hpp"

namespace Game {

	class Wall : public Actor
	{
	public:
		Wall(Math::Vec2 _position, Math::Vec2 _size);

	protected:
		PhysicsBodyComponentD m_body;
	};
}