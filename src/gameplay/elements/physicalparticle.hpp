#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "gameplay/elements/lifetimecomponent.hpp"

namespace Game {
	class PhysicalParticle : public Actor
	{
	public:
		PhysicalParticle(Math::Vec2 _position, float _rotation, Math::Vec2 _velocity,
			const sf::Texture& _sprite, float _lifetime,
			float _size, float _bouncyness = 0.4f, float _friction = 0.3f, float density = 1.f);
	private:
		SpriteComponent m_sprite;
		PhysicsBodyComponent m_body;
		LifetimeComponent m_lifetime;
	};
}