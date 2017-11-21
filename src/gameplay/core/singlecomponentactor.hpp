#pragma once

#include<type_traits>

namespace Game {

	template<typename T>
	class SingleComponentActor : public T
	{
		static_assert(std::is_base_of_v<Component, T>, "T has to be a Component type.");
	public:
		SingleComponentActor() : T(THISACTOR){}
		// todo: add constructors with position and rotation
	};
}