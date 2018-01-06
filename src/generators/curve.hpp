#pragma once

#include "random.hpp"

namespace Generators {
	
	using Curve = std::vector<Math::Vec2>;
	struct BlendCubic;

	class CurveGen
	{
	public:
		CurveGen();

		Curve Generate(unsigned _seed, int _numPoints, Math::Vec2 _position, Math::Vec2 _size);

		/* SampleSmooth() ********************
		 * Subdivides a given curve into a smooth curve with more points,
		 * preserving the original points.
		 * @tparam BlendFunc A sigmoid function used for interpolation between points.
		 * @param _segement The number of segments a constant function in the same rectangle would have. 
		 *					The real number of points is higher depending on the curvature.
		 * @return A list of points sampled from the curve from left to right.
		 */
		template<typename BlendFunc = BlendCubic>
		Curve SampleSmooth(const Curve& _curvePoints, int _segments)
		{
			using namespace Math;

			Assert(_curvePoints.size() < static_cast<size_t>(_segments),
				"A higher sampling rate is required to create a smooth curve.");
			Assert(_curvePoints.back().x > _curvePoints.front().x,
				"The points need to be ordered from left to right.");

			std::vector<Vec2> points;
			points.reserve(_segments + 2);
			const float ratio = static_cast<float>(_segments) / _curvePoints.size();

			const float distX = _curvePoints.back().x - _curvePoints.front().x;

			float sum = 0.f;
			for (size_t j = 0; j < _curvePoints.size() - 1; ++j)
			{
				const Vec2& start = _curvePoints[j];
				const Vec2& end = _curvePoints[j + 1];

				const Vec2 dif = end - start;
				int n = static_cast<int>(std::ceil(dif.Len() / distX * _segments));

				// the last point is the first of the next interval
				for (int i = 0; i < n; ++i)
				{
					float ratio = static_cast<float>(i) / n;

					points.emplace_back(start.x + dif.x * ratio,
						Interpolate<BlendFunc>(start.y, end.y, ratio));
				}
			}
			points.emplace_back(_curvePoints.back());

			return std::move(points);
		}
	private:
		template<typename BlendF>
		float Interpolate(float _a, float _b, float _x) const
		{
			BlendF blendFunc;

			return (1.f - blendFunc(_x))* _a + blendFunc(_x)*_b;
		}
	};

	// various sigmoid functions

	struct BlendCubic
	{
		float operator()(float _x) const
		{
			return 3.f * _x*_x - 2.f * _x * _x * _x;
		}
	};

	struct Blend5
	{
		float operator()(float x) const
		{
			return x * x * x * (x * (x * 6 - 15.f) + 10.f);
		}
	};

	struct BlendLinear
	{
		float operator()(float x) const
		{
			return x;
		}
	};

	struct BlendCos
	{
		float operator()(float x) const
		{
			return (1.f - cos(x * 3.1415f)) * 0.5f;
		}
	};
}