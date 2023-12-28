#include "curve.hpp"

namespace Generators{

	using namespace Math;

	CurveGen::CurveGen()
	{}

	Curve CurveGen::Generate(unsigned _seed, int _numPoints, Vec2 _position, Vec2 _size)
	{
		Curve curve(_numPoints);
		RandomGenerator random(_seed);

		const Vec2 lowerBound = _position - _size * 0.5f;
		const Vec2 upperBound = _position + _size * 0.5f;
		const float yRange = upperBound.y - lowerBound.y;

		// to reach the last point with the last segment -1 is required
		const float step = (upperBound.x - lowerBound.x) / (_numPoints-1);

		for (int i = 0; i < _numPoints; ++i)
			curve[i] = Vec2(lowerBound.x + i * step, lowerBound.y + random.Uniform(0.f, yRange));

		return curve;
	}
}