#pragma once

#include "controllercomponent.hpp"

namespace Input {
	class InputInterface;
}

namespace Game {

	class PlayerControllerComponent : public ControllerComponent
	{
	public:
		PlayerControllerComponent(Actor& _actor, Input::InputInterface& _inputInterface);

		void Process(float _deltaTime) override;
	private:
		Input::InputInterface& m_input;
	};
}