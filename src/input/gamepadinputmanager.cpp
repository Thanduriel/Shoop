#include "gamepadinputmanager.hpp"
#include "utils/assert.hpp"

namespace Input
{
	using namespace Utils;

	const ConfigSection::Initializer<GamePadButton, 2> Gamepad1Buttons(
		{ {
			{"Jump", GamePadButton::RB},
			{"JumpAlt", GamePadButton::LB},
		} });

	const ConfigSection::Initializer<sf::Joystick::Axis, 2> Gamepad1Axis(
		{ {
			{"Accelerate", sf::Joystick::Axis::U},
			{"Rotate", sf::Joystick::Axis::X},
		} });

	GamePadInputInterface::GamePadInputInterface(const ConfigSection& _config, unsigned _id)
		: m_inputMap({
			{Action::Jump, _config.GetValue<GamePadButton>("Jump")},
			{Action::JumpAlt, _config.GetValue<GamePadButton>("JumpAlt")} }),
		m_axisMap(
			{_config.GetValue<sf::Joystick::Axis>("Accelerate"),
			_config.GetValue<sf::Joystick::Axis>("Rotate")}),
		m_id(_id)
	{
		Assert(sf::Joystick::isConnected(_id), "Gamepad with the given id is not connected.");
	}

	bool GamePadInputInterface::IsKeyPressed(Action _action) const
	{
		return sf::Joystick::isButtonPressed(m_id, static_cast<unsigned>(m_inputMap.GetKey(_action)));
	}

	float GamePadInputInterface::GetAxis(Axis _axis) const
	{
		return sf::Joystick::getAxisPosition(m_id, m_axisMap[static_cast<size_t>(_axis)]) * 0.01f;
	}
}

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