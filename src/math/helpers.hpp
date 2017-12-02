#pragma once

namespace Math {

	constexpr double Pi = 3.1415926535897932;
	constexpr double π = 3.1415926535897932;
	constexpr double e = 2.7182818284;

	template<typename T>
	inline constexpr T ToRadian(T _value)
	{
		return static_cast<T>(_value / 180.0 * π);
	}

	template<typename T>
	inline constexpr T ToDegree(T _value)
	{
		return static_cast<T>(_value / π * 180.0);
	}

	inline constexpr double operator "" _deg(long double _val)
	{
		return _val / 360.0 * 2.0 * π;
	}
	
	inline constexpr double operator "" _rad(long double _val)
	{
		return _val;
	}
}