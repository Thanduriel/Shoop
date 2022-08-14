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
		PlayerControllerComponent(Actor& _actor, Input::InputInterface& _inputInterface,
			bool _autoCharge = false);

		void Process(float _deltaTime) override;

		Input::InputInterface& GetInputs() { return m_input; }
	private:
		Input::InputInterface& m_input;
		bool m_autoCharge;
	};

	// will probably change
	using ControllerContainer = std::vector<ControllerComponent*>;
}