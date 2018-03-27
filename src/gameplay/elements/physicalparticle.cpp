#include "physicalparticle.hpp"

namespace Game {

	PhysicalParticle::PhysicalParticle(Math::Vec2 _position, float _rotation, Math::Vec2 _velocity,
		const sf::Texture& _sprite, float _lifetime,
		float _size, float _bouncyness, float _friction, float _density)
		: m_sprite(THISACTOR, _sprite, _size),
		m_body(THISACTOR, this),
		m_lifetime(THISACTOR, _lifetime)
	{
		b2BodyDef def;
		def.position = _position;
		def.type = b2BodyType::b2_dynamicBody;

		b2CircleShape shape;
		shape.m_radius = _size*0.6f;

		b2FixtureDef fixture;
		fixture.friction = _friction;
		fixture.density = _density;
		fixture.restitution = _bouncyness;
		fixture.shape = &shape;

		m_body.Create(def, fixture);
		m_body.Get().SetLinearVelocity(_velocity);
		m_body.GetInfo().flags = 0x0;
	}
}