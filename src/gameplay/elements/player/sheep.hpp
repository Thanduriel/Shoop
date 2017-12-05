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
		SpriteComponent m_wheelSprite;
		PhysicsBodyComponent m_wheelBody;
	};
}