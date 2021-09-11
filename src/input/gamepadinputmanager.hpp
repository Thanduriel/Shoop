#pragma once

#include <SFML/Window/Joystick.hpp>
#include "actions.hpp"
#include "inputmanager.hpp"
#include "utils/config.hpp"

namespace Input
{
	enum GamePadButton { A, B, X, Y, LB, RB, Select, Start, COUNT };

	class GamePadInputInterface : public InputInterface
	{
	public:
		GamePadInputInterface(const Utils::ConfigSection& _config, unsigned _id);

		bool IsKeyPressed(Action _action) const override;
		float GetAxis(Axis _axis) const override;

	private:
		InputMap<GamePadButton, GamePadButton::COUNT> m_inputMap;
		std::array<sf::Joystick::Axis, 2> m_axisMap;
		unsigned m_id;
	};

	// default gamepad mappings
	const extern Utils::ConfigSection::Initializer<GamePadButton, 2> Gamepad1Buttons;
	const extern Utils::ConfigSection::Initializer<sf::Joystick::Axis, 2> Gamepad1Axis;
}

// allow reading of keys from a config
std::istream& operator >> (std::istream& _in, Input::GamePadButton& _key);
std::istream& operator >> (std::istream& _in, sf::Joystick::Axis& _axis);
