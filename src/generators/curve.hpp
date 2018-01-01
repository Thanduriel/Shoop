#pragma once

#include "random.hpp"

namespace Generators {

	using Curve = std::vector<Math::Vec2>;

	class CurveGen
	{
	public:
		CurveGen();

		Curve Generate(unsigned _seed, int _numPoints, Math::Vec2 _position, Math::Vec2 _size);

		/* SampleSmooth() ********************
		 * Subdivides a given curve into a smooth curve with more points,
		 * preserving the original points.
		 * @param _segement The number of segments a constant function in the same rectangle would have. 
		 *					The real number of points is higher depending on the curvature.
		 * @return A list of points sampled from the curve from left to right.
		 */
		Curve SampleSmooth(const Curve& _curvePoints, int _segments);
	private:
	};
}