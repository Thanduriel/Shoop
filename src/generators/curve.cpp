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

	CurveGen::CurveGen(unsigned _seed)
		: m_rng(_seed)
	{}

	std::vector<Vec2> CurveGen::Generate(float _size, int _segments, Vec2 _startValue, Vec2 _endValue)
	{
		std::vector<Vec2> points(_segments);

		points.front() = _startValue;
		points.back() = _endValue;

		const Vec2 dif = _endValue - _startValue;

		for (int i = 1; i < _segments - 1; ++i)
		{
			float ratio = static_cast<float>(i) / _segments;
			points[i].x = _startValue.x + dif.x * ratio;
			points[i].y = Interpolate<BlendCos>(_startValue.y, _endValue.y, ratio);
		}

		return std::move(points);
	}
}