#pragma once

#include <vector>
#include "math/transformation.hpp"

namespace Game {

	class Component;

	class Actor : public Math::Transformation
	{
	public:
		Actor(Math::Vec2 _position = Math::Vec2{}, float _rotation = 0.f, Math::Vec2 _scale = Math::Vec2(1.f));
		virtual ~Actor();

		Actor(Actor&&) = delete;
		Actor& operator=(Actor&&) = delete;

		// Marks this actor for destruction.
		// It will be removed at the end of the frame.
		void Destroy() { m_isDestroyed = true; }
		bool IsDestroyed() const { return m_isDestroyed; }

		std::vector<Component*>& GetComponents() { return m_components; }

	private:
		bool m_isDestroyed;
		std::vector<Component*> m_components;
	};
}