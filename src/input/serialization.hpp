#pragma once

#include "SFML/Window.hpp"

namespace Input
{
	enum GamePadButton
	{ 
		A, B, X, Y, LB, RB, Select, Start, COUNT, 
		NONE = -1
	};
}

// allow reading of keys from a config
std::istream& operator >> (std::istream& _in, Input::GamePadButton& _key);
std::istream& operator >> (std::istream& _in, sf::Joystick::Axis& _axis);
std::istream& operator >> (std::istream& _in, sf::Keyboard::Key& _key);