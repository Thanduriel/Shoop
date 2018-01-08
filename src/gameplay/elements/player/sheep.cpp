#include "sheep.hpp"
#include "resources.hpp"
#include "Box2D/Box2D.h"

namespace Game {

	using namespace Math;

	constexpr Vec2 WHEEL_OFFSET = Vec2(0.f, -0.91f);
	constexpr float WHEEL_RADIUS = 0.26f;
	constexpr Vec2 HEAD_OFFSET = Vec2(0.f, 0.78f);
	constexpr Vec2 BODY_SIZE = Vec2(0.26f, 0.39f);

	constexpr float SPRITE_SIZE = WHEEL_RADIUS * 3.f * 255.f/147.f;

	Sheep::Sheep(Math::Vec2 _position, const sf::Color& _color)
		: Actor(_position),
		m_wheelSprite(THISACTOR, Resources::Load<sf::Texture>("wheel"), Math::Vec2(WHEEL_RADIUS*2.3f)),
		m_bodySprite(THISACTOR, Resources::Load<sf::Texture>("SheepFront"), SPRITE_SIZE),
		m_legSprite(THISACTOR, Resources::Load<sf::Texture>("sheepBack"), SPRITE_SIZE),
		m_unicycleSprite(THISACTOR, Resources::Load<sf::Texture>("unicycle"), SPRITE_SIZE),
		m_wheel(THISACTOR, &m_wheelSprite),
		m_body(THISACTOR, this),
		m_joint(THISACTOR)
	{
		m_bodySprite.SetPosition(Vec2(0.04f, 0.f));
		m_legSprite.SetPosition(Vec2(0.04f, 0.f));
		m_unicycleSprite.SetPosition(Vec2(0.08f,0.f));
		m_unicycleSprite.GetSprite().setColor(_color);
		m_wheelSprite.SetPosition(WHEEL_OFFSET);
		// wheel
		b2BodyDef def;
		def.position = _position + WHEEL_OFFSET;
		def.type = b2BodyType::b2_dynamicBody;
	//	def.active = false;
		
		b2CircleShape wheelShape;
		wheelShape.m_radius = WHEEL_RADIUS;

		b2FixtureDef wheelFixture;
		wheelFixture.friction = 1.f;
		wheelFixture.density = 1.f;
		wheelFixture.restitution = 0.f;
		wheelFixture.shape = &wheelShape;

		m_wheel.Create(def, wheelFixture);

		// body with head
		def.position = _position;

		b2PolygonShape bodyShape;
		bodyShape.SetAsBox(BODY_SIZE.x, BODY_SIZE.y);

		b2FixtureDef bodyFixture;
		bodyFixture.friction = 0.4f;
		bodyFixture.restitution = 0.f;
		bodyFixture.density = 0.25f;
		bodyFixture.shape = &bodyShape;

		b2CircleShape headShape;
		headShape.m_radius = 0.195f;
		headShape.m_p = HEAD_OFFSET;

		b2FixtureDef headFixture;
		headFixture.friction = 1.f;
		headFixture.density = 0.1f;
		headFixture.restitution = 0.f;
		headFixture.shape = &headShape;
		headFixture.userData = this; // mark the head in some unique way

		m_body.Create(def, { &bodyFixture, &headFixture });

		// connection
		b2RevoluteJointDef jointDef;
		jointDef.bodyA = &m_wheel.Get();
		jointDef.bodyB = &m_body.Get();
		jointDef.type = b2JointType::e_revoluteJoint;
		jointDef.localAnchorA = Math::Vec2(0.f);
		jointDef.localAnchorB = WHEEL_OFFSET;

		m_joint.Create(jointDef);

		// head can take lethal damage
		auto contactFn = [this](b2Fixture& _slf, b2Fixture& _oth)
		{
			// a fixture without Info is assumed to be lethal
			if (_slf.GetUserData() == this && (!_oth.GetUserData()
				|| PhysicsInfo::Get(_oth).flags & PhysicsInfo::IsLethal))
			{
				m_bodySprite.GetSprite().setColor(sf::Color(0xff0000ff));
			}
		};
		m_body.SetOnContactBegin(std::move(contactFn));
	}
}