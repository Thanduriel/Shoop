#include <istream>
#include "serialization.hpp"

std::istream& operator >> (std::istream& _in, Input::GamePadButton& _key)
{
	int i;
	_in >> i;
	_key = static_cast<Input::GamePadButton>(i);

	return _in;
}

std::istream& operator >> (std::istream& _in, sf::Joystick::Axis& _axis)
{
	int i = -1;
	_in >> i;
	_axis = static_cast<sf::Joystick::Axis>(i);

	return _in;
}

std::istream& operator >> (std::istream& _in, sf::Keyboard::Key& _key)
{
	int i;
	_in >> i;
	_key = static_cast<sf::Keyboard::Key>(i);

	return _in;
}