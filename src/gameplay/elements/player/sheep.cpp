#include "sheep.hpp"
#include "resources.hpp"
#include "Box2D/Box2D.h"

namespace Game {

	Sheep::Sheep(Math::Vec2 _position)
		: Actor(_position),
		m_wheelSprite(THISACTOR, Resources::Load<sf::Texture>("wheel"), Math::Vec2(0.6f)),
		m_wheel(THISACTOR),
		m_body(THISACTOR, true)
	{
		// wheel
		b2BodyDef def;
		def.position = _position;
		def.type = b2BodyType::b2_dynamicBody;
	//	def.active = false;
		
		b2CircleShape wheelShape;
		wheelShape.m_radius = 0.26f;

		b2FixtureDef wheelFixture;
		wheelFixture.friction = 1.f;
		wheelFixture.density = 1.f;
		wheelFixture.restitution = 0.f;
		wheelFixture.shape = &wheelShape;

		m_wheel.Create(def, wheelFixture);

		// body
		def.position = _position;

		b2PolygonShape bodyShape;
		bodyShape.SetAsBox(0.26f, 0.39f);

		b2FixtureDef bodyFixture;
		bodyFixture.friction = 0.4f;
		bodyFixture.restitution = 0.f;
		bodyFixture.density = 0.25f;
		bodyFixture.shape = &bodyShape;

		b2CircleShape headShape;
		headShape.m_radius = 0.195f;
		headShape.m_p = Math::Vec2(0.f, 0.78f);

		b2FixtureDef headFixture;
		headFixture.friction = 1.f;
		headFixture.density = 0.1f;
		headFixture.restitution = 0.f;
		headFixture.shape = &headShape;

		m_body.Create(def, { &bodyFixture, &headFixture });
	}
}