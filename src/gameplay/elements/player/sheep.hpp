#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "gameplay/elements/spriteomponent.hpp"

namespace Game {

	class Sheep : public Actor
	{
	public:
		Sheep(Math::Vec2 _position);

	private:
		GlobalSpriteComponent m_wheelSprite;
		PhysicsBodyComponent m_wheel;
		PhysicsBodyComponentD m_body;
		PhysicsJointComponent m_joint;
	};
}