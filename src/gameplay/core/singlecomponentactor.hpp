#pragma once

#include<type_traits>
#include "gameplay/core/actor.hpp"
#include "math/vector.hpp"

namespace Game {

	template<typename T>
	class SingleComponentActor : public Actor
	{
		static_assert(std::is_base_of_v<Component, T>, "T has to be a Component type.");
	public:
		template<typename... Args>
		SingleComponentActor(Math::Vec2 _position, Args&&... _args) 
			: Actor(_position),
			m_component(THISACTOR, std::forward<Args>(_args)...)
		{}

		// direct access to the single component
		const T& GetComponent() const { return m_component; }
		T& GetComponent() { return m_component; }
	private:
		T m_component;
	};
}