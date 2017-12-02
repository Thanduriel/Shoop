#pragma once

#include "vector.hpp"

namespace Math {

	class Transformation
	{
	public:
		Transformation(Vec2 _position = Vec2{}, float _rotation = 0.f, Vec2 _scale = Vec2(1.f));

		Vec2 GetPosition() const { return m_position; }
		void SetPosition(Vec2 _position) { m_position = _position; }

		Vec2 GetScale() const { return m_scale; }
		void SetScale(Vec2 _scale) { m_scale = _scale; }

		float GetRotation() const { return m_rotation; }
		void SetRotation(float _rotation);

		// Transforms a vector from this local space to world space.
		Vec2 Transform(Vec2 _vector) const;
		// Returns a Transformation that effectively applies first other and then this,
		// transforming local positions in _other to (global) of this.
		Transformation Transform(const Transformation& _other) const;
	protected:
		Vec2 m_position;
		float m_rotation;
		Vec2 m_scale;

	private:
		void UpdateMatrix();
		float m_cosR, m_sinR;
	};
}