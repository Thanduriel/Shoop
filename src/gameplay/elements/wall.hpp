#pragma once

#include "gameplay/core/actor.hpp"
#include "physicscomponent.hpp"

namespace Game {

	class Wall : public Actor
	{
	public:
		
		 // @param _position The center of the rectangle
		Wall(Math::Vec2 _position, Math::Vec2 _size, bool _isLethal = false);

	protected:
		PhysicsBodyComponentD m_body;
	};
}