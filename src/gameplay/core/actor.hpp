#pragma once

#include <vector>
#include <typeinfo>
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

		// Returns the first component of type T associated with this actor.
		// This is really slow! If you need a component multiple times keep the pointer.
		template<typename T, unsigned Skip = 0, typename = std::enable_if_t<std::is_base_of_v<Component,T>>>
		T* GetComponent()
		{
			unsigned num = Skip;

			const std::type_info& tInfo = typeid(T);
			for (Component* comp : m_components)
				if (tInfo == typeid(*comp))
				{
					if (!num)
						return static_cast<T*>(comp);
					else
						--num;
				}
			
			return nullptr;
		}
	private:
		bool m_isDestroyed;
		std::vector<Component*> m_components;
	};
}