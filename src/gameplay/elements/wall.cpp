#include "wall.hpp"
#include "box2d/box2d.h"

namespace Game {

	Wall::Wall(Math::Vec2 _position, Math::Vec2 _size, PhysicsInfo::Flags _flags)
		: Actor(_position),
		m_body(THISACTOR)
	{
		b2BodyDef def;
		def.position = _position;
		def.type = b2BodyType::b2_staticBody;
		//	def.active = false;

		b2PolygonShape shape;
		shape.SetAsBox(_size.x / 2.f, _size.y / 2.f);

		b2FixtureDef fixture;
		fixture.friction = 1.f;
		fixture.restitution = 0.f;
		fixture.shape = &shape;
		
		m_body.GetInfo().flags = _flags;
		m_body.Create(def, fixture);
	}
}