#pragma once

#include <SFML/Window/Joystick.hpp>
#include "actions.hpp"
#include "inputmanager.hpp"
#include "serialization.hpp"
#include "utils/config.hpp"

namespace Input
{
	class GamePadInputInterface : public InputInterface
	{
	public:
		GamePadInputInterface(const Utils::ConfigSection& _config, unsigned _id);

		bool IsKeyPressed(Action _action) const override;
		float GetAxis(Axis _axis) const override;

	private:
		InputMap<GamePadButton, GamePadButton::NONE> m_inputMap;
		std::array<sf::Joystick::Axis, 2> m_axisMap;
		unsigned m_id;
	};

	// default gamepad mappings
	const extern Utils::ConfigSection::Initializer<GamePadButton, 6> Gamepad1Buttons;
	const extern Utils::ConfigSection::Initializer<sf::Joystick::Axis, 2> Gamepad1Axis;
}
