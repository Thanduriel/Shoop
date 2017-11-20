#pragma once

#include <vector>

namespace Game {

	class Component;

	class Actor 
	{
	public:
		Actor();
		virtual ~Actor() {}

		Actor(Actor&&) = delete;
		Actor& operator=(Actor&&) = delete;

		void SetRotation(float _rotation) { m_rotation = _rotation; }
		float GetRotation() const { return m_rotation; }

		void SetScale(float _scale) { m_scale = _scale; }
		float GetScale() const { return m_scale; }

		// Marks this actor for destruction.
		// It will be removed at the end of the frame.
		void Destroy() { m_isDestroyed = true; }
		bool IsDestroyed() const { return m_isDestroyed; }

		std::vector < std::reference_wrapper<Component>>& GetComponents() { return m_components; }
	protected:
		float m_rotation;
		float m_scale;
	private:
		bool m_isDestroyed;
		std::vector < std::reference_wrapper<Component>> m_components;
	};
}