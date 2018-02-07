namespace Math {

	// various sigmoid functions

	struct BlendCubic
	{
		float operator()(float x) const
		{
			return 3.f * x*x - 2.f * x * x * x;
		}
	};

	struct Blend5
	{
		float operator()(float x) const
		{
			return x * x * x * (x * (x * 6 - 15.f) + 10.f);
		}
	};

	struct BlendCos
	{
		float operator()(float x) const
		{
			return (1.f - cos(x * 3.1415f)) * 0.5f;
		}
	};

	struct BlendLinear
	{
		float operator()(float x) const
		{
			return x;
		}
	};

	// regressive growth
	struct Quadratic
	{
		float operator()(float x) const
		{
			return -x*x + 2*x;
		}
	};
}