#include "ground.hpp"
#include "generators/curve.hpp"
#include "box2d/box2d.h"

namespace Game {

	using namespace Math;

	constexpr float SEGMENTS_PER_M = 1.6f;

	GroundPlane::GroundPlane(const Generators::Curve& _curve, Vec2 _position)
		: Actor(_position),
		m_body(THISACTOR)
	{
		const int numSegments = static_cast<int>(_curve.size());

		b2BodyDef def;
		def.position = _position;
		def.type = b2BodyType::b2_staticBody;

		b2Vec2 vertices[3];

		// generate physical triangle chain
		const int numTriangles = numSegments - 1;
		std::vector<b2PolygonShape> shapes(numTriangles);
		std::vector<b2FixtureDef> fixtures(numTriangles);
		std::vector<b2FixtureDef*> fixturePtrs(numTriangles);
		for (int i = 0; i < numTriangles; ++i)
		{
			vertices[0] = _curve[i];
			vertices[1] = (_curve[i] + _curve[i + 1]) * 0.5f - Vec2(0.f, 0.2f);
			vertices[2] = _curve[i + 1];
			shapes[i].Set(vertices, 3);

			fixtures[i].friction = 1.f;
			fixtures[i].restitution = 0.f;
			fixtures[i].shape = &shapes[i];
			
			fixturePtrs[i] = &fixtures[i];
		}

		m_body.GetInfo().flags |= PhysicsInfo::IsGround;
		m_body.Create(def, fixturePtrs);
	}
}