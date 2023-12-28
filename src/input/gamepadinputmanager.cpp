#include "gamepadinputmanager.hpp"
#include "utils/assert.hpp"

namespace Input
{
	using namespace Utils;

	const ConfigSection::Initializer<GamePadButton, 6> Gamepad1Buttons(
		{ {
			{"Jump", GamePadButton::RB},
			{"JumpAlt", GamePadButton::LB},
			{"AccelerateCW", GamePadButton::NONE},
			{"AccelerateCCW", GamePadButton::NONE},
			{"RotateCW", GamePadButton::NONE},
			{"RotateCCW", GamePadButton::NONE}
		} });

	const ConfigSection::Initializer<sf::Joystick::Axis, 2> Gamepad1Axis(
		{ {
			{"Accelerate", sf::Joystick::Axis::U},
			{"Rotate", sf::Joystick::Axis::X},
		} });

	GamePadInputInterface::GamePadInputInterface(const ConfigSection& _config, unsigned _id)
		: m_inputMap({
			{Action::Jump, _config.GetValue<GamePadButton>("Jump")},
			{Action::JumpAlt, _config.GetValue<GamePadButton>("JumpAlt")},
			{Action::AccelerateCW, _config.GetValue<GamePadButton>("AccelerateCW")},
			{Action::AccelerateCCW, _config.GetValue<GamePadButton>("AccelerateCCW")},
			{Action::RotateCW, _config.GetValue<GamePadButton>("RotateCW")},
			{Action::RotateCCW, _config.GetValue<GamePadButton>("RotateCCW")} }),
		m_axisMap(
			{_config.GetValue<sf::Joystick::Axis>("Accelerate"),
			_config.GetValue<sf::Joystick::Axis>("Rotate")}),
		m_id(_id)
	{
		Assert(sf::Joystick::isConnected(_id), "Gamepad with the given id is not connected.");
	}

	bool GamePadInputInterface::IsKeyPressed(Action _action) const
	{
		const GamePadButton button = m_inputMap.GetKey(_action);
		return button == GamePadButton::NONE ? false
			: sf::Joystick::isButtonPressed(m_id, static_cast<unsigned>(button));
	}

	float GamePadInputInterface::GetAxis(Axis _axis) const
	{
		sf::Joystick::Axis axis = m_axisMap[static_cast<size_t>(_axis)];

		float pos = 0.f;
		if(static_cast<int>(axis) != -1)
			pos = sf::Joystick::getAxisPosition(m_id, axis) * 0.01f;

		const AxisAction aa = AXIS_ACTIONS[static_cast<size_t>(_axis)];
		if (IsKeyPressed(aa.low))
			pos -= 1.f;
		if (IsKeyPressed(aa.high))
			pos += 1.f;
		return pos;
	}
}