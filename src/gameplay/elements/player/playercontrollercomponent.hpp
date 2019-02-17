#pragma once

#include "controllercomponent.hpp"
#include <vector> //todo: remove ControllerContainer

namespace Input {
	class InputInterface;
}

namespace Game {

	using PlayerId = int;

	class PlayerControllerComponent : public ControllerComponent
	{
	public:
		PlayerControllerComponent(Actor& _actor, Input::InputInterface& _inputInterface);

		void Process(float _deltaTime) override;
	private:
		Input::InputInterface& m_input;
	};

	// will probably change
	using ControllerContainer = std::vector<PlayerControllerComponent*>;
}