#pragma once

#include <cmath>
#include <type_traits>
#include "Box2D/Common/b2Math.h"
#include "SFML/System/Vector2.hpp"
#include "utils/assert.hpp"

namespace Math {

	template<typename T>
	class Vector2
	{
	public:
		T x, y;

		// constructors
		// Default construction sets all values to zero.
		Vector2() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}
		// Initialize with values.
		constexpr Vector2(T _x, T _y) : x(_x), y(_y) {}
		// Initialize all components with the same value.
		constexpr explicit Vector2(T _val) : x(_val), y(_val) {}
		// conversion from different typed vector
		template<typename U, typename = std::enable_if_t< !std::is_same_v<T, U> >>
		constexpr explicit Vector2(const Vector2<U>& _orig) : x(static_cast<T>(_orig.x)), y(static_cast<T>(_orig.y)) {}

		// Conversion LibVec -> MathVec
		Vector2(const sf::Vector2<T>& _orig) : x(_orig.x), y(_orig.y) {}
		// for different base types only explicit conversion
		template<typename U, typename = std::enable_if_t< !std::is_same_v<T,U> >>
		explicit Vector2(const sf::Vector2<U>& _orig) : x(static_cast<T>(_orig.x)), y(static_cast<T>(_orig.y)) {}
		// Box2D only has a floating point vector.
		template<typename U = T> requires std::is_floating_point_v<U>
		Vector2(const b2Vec2& _orig) : x(_orig.x), y(_orig.y) {}
		
		// Casts to other vector types
		template<typename U, typename = std::enable_if_t< !std::is_same_v<T, U> >>
		explicit operator Vector2<U>() const { return sf::Vector2(static_cast<U>(x), static_cast<U>(y)); }
		
		// Conversion MathVec -> LibVec 
		operator sf::Vector2<T>() const								{ return sf::Vector2<T>(x, y); }
		template<typename U, typename = std::enable_if_t< !std::is_same_v<T, U> >>
		explicit operator sf::Vector2<U>() const					{ return sf::Vector2(static_cast<U>(x), static_cast<U>(y)); }
		
		template<typename U = T> requires std::is_floating_point_v<U>
		operator b2Vec2() const										{ return b2Vec2(x, y); }

		// Common operations
		T LenSqr() const { return x * x + y * y; }
		auto Len() const { return std::sqrt(LenSqr()); }

		void Normalize() { T l = Len(); Assert(l != 0, "Can not normalize a 0-Vector."); x /= l; y /= l; }
		Vector2<T> Normalized() const { Vector2<T> v(*this); v.Normalize(); return v; }
		void Rotate(T _angle);
		Vector2<T> Rotated(T _angle) const { Vector2<T> v(*this); v.Rotate(_angle); return v; }

		// Combined Assignment operators
		Vector2<T>& operator+=(const Vector2<T>& _rhs) { x += _rhs.x; y += _rhs.y; return *this; }
		Vector2<T>& operator-=(const Vector2<T>& _rhs) { x -= _rhs.x; y -= _rhs.y; return *this; }
		Vector2<T>& operator*=(const Vector2<T>& _rhs) { x *= _rhs.x; y *= _rhs.y; return *this; }
		Vector2<T>& operator/=(const Vector2<T>& _rhs) { x /= _rhs.x; y /= _rhs.y; return *this; }

		Vector2<T> operator*=(const T _s) { x *= _s; y *= _s; return *this; }
		Vector2<T> operator/=(const T _s) { x /= _s; y /= _s; return *this; }
		Vector2<T> operator+=(const T _s) { x += _s; y += _s; return *this; }
		Vector2<T> operator-=(const T _s) { x -= _s; y -= _s; return *this; }
	};

	// Vector x Vector operators
	template<typename T>
	inline Vector2<T> operator+(const Vector2<T>& _lhs, const Vector2<T>& _rhs) { return Vector2<T>(_lhs.x + _rhs.x, _lhs.y + _rhs.y); }

	template<typename T>
	inline Vector2<T> operator-(const Vector2<T>& _lhs, const Vector2<T>& _rhs) { return Vector2<T>(_lhs.x - _rhs.x, _lhs.y - _rhs.y); }

	template<typename T>
	inline bool operator==(const Vector2<T>& _lhs, const Vector2<T>& _rhs){ return _lhs.x == _rhs.x && _lhs.y == _rhs.y;}
	template<typename T>
	inline bool operator!=(const Vector2<T>& _lhs, const Vector2<T>& _rhs){ return _lhs.x != _rhs.x && _lhs.y != _rhs.y; }

	template<typename T>
	Vector2<T> operator*(const Vector2<T>& _lhs, const Vector2<T>& _rhs) { return Vector2<T>(_lhs.x * _rhs.x, _lhs.y * _rhs.y); }

	template<typename T>
	Vector2<T> operator/(const Vector2<T>& _lhs, const Vector2<T>& _rhs){ return Vector2<T>(_lhs.x / _rhs.x, _lhs.y / _rhs.y); }

	// Scalar x Vector operators
	template<typename T>
	Vector2<T> operator*(const Vector2<T>& _vec, const T _s) { return Vector2<T>(_vec.x * _s, _vec.y * _s); }
	template<typename T>
	Vector2<T> operator*(const T _s, const Vector2<T>& _vec) { return Vector2<T>(_vec.x * _s, _vec.y * _s); }

	template<typename T>
	Vector2<T> operator/(const Vector2<T>& _vec, const T _s) { return Vector2<T>(_vec.x / _s, _vec.y / _s); }
	template<typename T>
	Vector2<T> operator/(const T _s, const Vector2<T>& _vec) { return Vector2<T>(_vec.x / _s, _vec.y / _s); }

	template<typename T>
	Vector2<T> operator+(const Vector2<T>& _vec, const T _s) { return Vector2<T>(_vec.x + _s, _vec.y + _s); }
	template<typename T>
	Vector2<T> operator+(const T _s, const Vector2<T>& _vec) { return Vector2<T>(_vec.x + _s, _vec.y + _s); }

	template<typename T>
	Vector2<T> operator-(const Vector2<T>& _vec, const T _s) { return Vector2<T>(_vec.x - _s, _vec.y - _s); }
	template<typename T>
	Vector2<T> operator-(const T _s, const Vector2<T>& _vec) { return Vector2<T>(_vec.x - _s, _vec.y - _s); }

	// other functions
	template<typename T>
	T Dot(const Vector2<T>& _lhs, const Vector2<T>& _rhs){ return _lhs.x * _rhs.x + _lhs.y * _rhs.y; }

	template<typename T>
	T DistanceSq(const Vector2<T>& _lhs, const Vector2<T>& _rhs) { return  (_lhs-_rhs).LenSqr(); }

	template<typename T>
	auto Distance(const Vector2<T>& _lhs, const Vector2<T>& _rhs) { return  (_lhs - _rhs).Len(); }

	using Vec2 = Vector2<float>;
	using Vec2I = Vector2<int>;
	using Vec2U = Vector2<unsigned>;

	// ******************************************************************** //
	// long implementations
	template<typename T>
	inline void Vector2<T>::Rotate(T _angle)
	{
		T cosA = std::cos(_angle);
		T sinA = std::sin(_angle);
		T nx = x * cosA - y * sinA;
		T ny = y * cosA + x * sinA;
		x = nx;
		y = ny;
	}
}