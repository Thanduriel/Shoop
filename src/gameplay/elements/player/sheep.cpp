#include "sheep.hpp"
#include "resources.hpp"
#include "Box2D/Box2D.h"

namespace Game {

	Sheep::Sheep(Math::Vec2 _position)
		: Actor(_position),
		m_wheelSprite(THISACTOR, Resources::Load<sf::Texture>("wheel")),
		m_wheelBody(THISACTOR, true)
	{
		m_wheelSprite.SetScale(Math::Vec2(0.5f));
		b2BodyDef def;
		def.position = _position;
		def.type = b2BodyType::b2_dynamicBody;
		def.active = false;
		
		b2CircleShape wheelShape;
		wheelShape.m_radius = 0.3f;

		b2FixtureDef wheelFixture;
		wheelFixture.friction = 1.f;
		wheelFixture.density = 1.f;
		wheelFixture.restitution = 0.f;
		wheelFixture.shape = &wheelShape;

		m_wheelBody.Create(def, wheelFixture);
	}
}