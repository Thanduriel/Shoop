#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "jumpcomponent.hpp"
#include "Balancercomponent.hpp"

namespace Game {

	class Sheep : public Actor
	{
	public:
		Sheep(Math::Vec2 _position, const sf::Color& _color = sf::Color(0xffffffff));

		//Balanciert gegen Gewichtskraft
		void VBalance() { m_balancerComponent.VBalance(); }
		//Balanciert gegen Normalkraft
		void OBalance() { m_balancerComponent.OBalance(); }

		int GetGroundContacts() const { return m_groundContacts; }
		PhysicsBodyComponent& GetBody() { return m_body; }
	private:
		GlobalSpriteComponent m_wheelSprite;
		SpriteComponent m_legSprite;
		SpriteComponent m_unicycleSprite;
		SpriteComponent m_bodySprite;
		PhysicsBodyComponentD m_wheel;
		PhysicsBodyComponentD m_body;
		PhysicsJointComponent m_joint;
		JumpComponent m_jumpComponent;
		BalancerComponent m_balancerComponent;
		
		int m_groundContacts;
	};
}