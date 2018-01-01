#include "ground.hpp"
#include "generators/curve.hpp"
#include "Box2D/Box2D.h"

namespace Game {

	using namespace Math;

	constexpr float SEGMENTS_PER_M = 1.6f;

	GroundPlane::GroundPlane(Math::Vec2 _startPoint, Math::Vec2 _endPoint, uint32_t _seed)
		: Actor(Vec2(0.f)),
		m_body(THISACTOR)
	{
		const float difX = _endPoint.x - _startPoint.x;
		const int numSegments = static_cast<int>(std::ceil(SEGMENTS_PER_M * difX));

		Assert(_startPoint.x < _endPoint.x, "_startPoint has to be left of _endPoint.");

		Generators::CurveGen generator;

		std::vector<Vec2> curve = generator.SampleSmooth(/*{_startPoint, (_startPoint+_endPoint)*0.3f, _endPoint}*/
			generator.Generate(0x8E5A5C43, 6, (_startPoint + _endPoint)*0.5f, _endPoint - _startPoint), numSegments);

		b2BodyDef def;
		def.position = Vec2(0.f);
		def.type = b2BodyType::b2_staticBody;

		b2Vec2 vertices[3];

		// generate physical triangle chain
		const int numTriangles = numSegments - 1;
		std::vector<b2PolygonShape> shapes(numTriangles);
		std::vector<b2FixtureDef> fixtures(numTriangles);
		std::vector<b2FixtureDef*> fixturePtrs(numTriangles);
		for (int i = 0; i < numTriangles; ++i)
		{
			vertices[0] = curve[i];
			vertices[1] = (curve[i] + curve[i + 1]) * 0.5f - Vec2(0.f, 0.2f);
			vertices[2] = curve[i + 1];
			shapes[i].Set(vertices, 3);

			fixtures[i].friction = 1.f;
			fixtures[i].restitution = 0.f;
			fixtures[i].shape = &shapes[i];
			
			fixturePtrs[i] = &fixtures[i];
		}

		m_body.Create(def, fixturePtrs);
	}
}