#pragma once

#include "random.hpp"

namespace Generators {

	class CurveGen
	{
	public:
		CurveGen(unsigned _seed);

		/* Generate() ********************
		 * Generates a smooth curve with the given parameters.
		 * @param _size The maximum change in y direction froom the given points.
		 * @param _segement The number of segments the curve is split into.
		 * @param _startValue The first point of the curve.
		 * @return A list of points sampled from the curve from left to right.
		 */
		std::vector<Math::Vec2> Generate(float _size, int _segments, 
			Math::Vec2 _startValue, Math::Vec2 _endValue);
	private:
		RandomGenerator m_rng;
	};
}