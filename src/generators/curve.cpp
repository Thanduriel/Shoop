#include "curve.hpp"

namespace Generators{

	using namespace Math;


	// various sigmoid functions

	class BlendCubic
	{
	public:
		float operator()(float _x) const
		{
			return 3.f * _x*_x - 2.f * _x * _x * _x;
		}
	};

	class Blend5
	{
	public:
		float operator()(float x) const
		{
			return x * x * x * (x * (x * 6 - 15.f) + 10.f);
		}
	};

	class BlendLinear
	{
	public:
		float operator()(float x) const
		{
			return x;
		}
	};

	class BlendCos
	{
	public:
		float operator()(float x) const
		{
			return (1.f - cos(x * 3.1415f)) * 0.5f;
		}
	};

	template<typename BlendF>
	static float Interpolate(float _a, float _b, float _x)
	{
		BlendF blendFunc;

		return (1.f - blendFunc(_x))* _a + blendFunc(_x)*_b;
	}

	CurveGen::CurveGen()
	{}

	Curve CurveGen::Generate(unsigned _seed, int _numPoints, Vec2 _position, Vec2 _size)
	{
		Curve curve(_numPoints);
		RandomGenerator random(_seed);

		const Vec2 lowerBound = _position - _size * 0.5f;
		const Vec2 upperBound = _position + _size * 0.5f;

		const float step = (upperBound.x - lowerBound.x) / (_numPoints-1);

		for (int i = 0; i < _numPoints; ++i)
			curve[i] = Vec2(lowerBound.x + i * step, random.Uniform(lowerBound.y, upperBound.y));

		return std::move(curve);
	}

	Curve CurveGen::SampleSmooth(const Curve& _curvePoints, int _segments)
	{
		Assert(_curvePoints.size() < static_cast<size_t>(_segments), 
			"A higher sampling rate is required to create a smooth curve.");
		Assert(_curvePoints.back().x > _curvePoints.front().x, 
			"The points need to be ordered from left to right.");

		std::vector<Vec2> points;
		points.reserve(_segments + 2);
		const float ratio = static_cast<float>(_segments) / _curvePoints.size();

		const float distX = _curvePoints.back().x - _curvePoints.front().x;

		float sum = 0.f;
		for (size_t j = 0; j < _curvePoints.size()-1; ++j)
		{
			const Vec2& start = _curvePoints[j];
			const Vec2& end = _curvePoints[j+1];

			const Vec2 dif = end - start;
			int n = static_cast<int>(std::ceil(dif.Len() / distX * _segments));

			// the last point is the first of the next interval
			for (int i = 0; i < n; ++i)
			{
				float ratio = static_cast<float>(i) / n;

				points.emplace_back(start.x + dif.x * ratio,
					Interpolate<BlendCos>(start.y, end.y, ratio));
			}
		}
		points.emplace_back(_curvePoints.back());

		return std::move(points);
	}
}