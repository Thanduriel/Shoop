#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "jumpcomponent.hpp"

namespace Game {

	class Sheep : public Actor
	{
	public:
		Sheep(Math::Vec2 _position, const sf::Color& _color = sf::Color(0xffffffff));

		void Jump() { m_jumpComponent.Jump(); }
	private:
		GlobalSpriteComponent m_wheelSprite;
		SpriteComponent m_legSprite;
		SpriteComponent m_unicycleSprite;
		SpriteComponent m_bodySprite;
		PhysicsBodyComponentD m_wheel;
		PhysicsBodyComponentD m_body;
		PhysicsJointComponent m_joint;
		JumpComponent m_jumpComponent;

		int m_groundContacts;
	};
}