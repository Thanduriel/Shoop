#include "wall.hpp"
#include "Box2D/Box2D.h"

namespace Game {

	Wall::Wall(Math::Vec2 _position, Math::Vec2 _size, bool _isLethal)
		: Actor(_position),
		m_body(THISACTOR),
		m_info{ _isLethal ? PhysicsInfo::IsLethal : 0u }
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
		fixture.userData = &m_info;

		m_body.Create(def, fixture);
	}
}