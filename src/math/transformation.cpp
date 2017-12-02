#include "transformation.hpp"

namespace Math{

	Transformation::Transformation(Vec2 _position, float _rotation, Vec2 _scale)
		: m_position(_position),
		m_rotation(_rotation),
		m_scale(_scale)
	{
		UpdateMatrix();
	}

	void Transformation::SetRotation(float _rotation)
	{
		m_rotation = _rotation;
		UpdateMatrix();
	}

	Vec2 Transformation::Transform(Vec2 _vector) const
	{
		Vec2 v = _vector * m_scale;
		v.x = v.x * m_cosR - v.y * m_sinR;
		v.y = v.y * m_cosR + v.x * m_sinR;
		return v + m_position;

	}

	Transformation Transformation::Transform(const Transformation& _other) const
	{
		Vec2 off = _other.m_position.Rotated(m_rotation) * m_scale;
		return Transformation(m_position + off, m_rotation + _other.m_rotation, m_scale * _other.m_scale);
	}

	void Transformation::UpdateMatrix()
	{
		m_cosR = std::cos(m_rotation);
		m_sinR = std::sin(m_rotation);
	}
}