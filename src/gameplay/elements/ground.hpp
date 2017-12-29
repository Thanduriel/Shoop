#pragma once

#include "gameplay/core/actor.hpp"
#include "physicscomponent.hpp"

namespace Game {

	class GroundPlane : public Actor
	{
	public:
		GroundPlane(Math::Vec2 _startPoint, Math::Vec2 _endPoint, uint32_t _seed = 0x1bba35);
	private:
		PhysicsBodyComponentD m_body;
	};
}