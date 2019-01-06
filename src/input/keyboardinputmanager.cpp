#include "keyboardinputmanager.hpp"

namespace Input
{
	using Key = sf::Keyboard::Key;
	using namespace std::string_literals;
	using namespace Utils;

	const Utils::ConfigSection::Initializer<sf::Keyboard::Key,5> Keyboard1(
	{ {
		{"Jump", Key::RControl},
		{"AccelerateCW", Key::Up},
		{"AccelerateCCW", Key::Down},
		{"RotateCW", Key::Left},
		{"RotateCCW", Key::Right}
	} });

	const Utils::ConfigSection::Initializer<sf::Keyboard::Key, 5> Keyboard2(
		{ {
			{"Jump", Key::Space},
			{"AccelerateCW", Key::W},
			{"AccelerateCCW", Key::S},
			{"RotateCW", Key::A},
			{"RotateCCW", Key::D}
		} });

	KeyBoardInputInterface::KeyBoardInputInterface()
		: m_inputMap({ 
			{Action::Jump, Key::Space},
			{Action::AccelerateCW, Key::Up},
			{Action::AccelerateCCW, Key::Down},
			{Action::RotateCW, Key::Left},
			{Action::RotateCCW, Key::Right} })
	{}

	bool KeyBoardInputInterface::IsKeyPressed(Action _action) const
	{
		return sf::Keyboard::isKeyPressed(m_inputMap.GetKey(_action));
	}

	float KeyBoardInputInterface::GetAxis(Axis _axis) const
	{
		const AxisAction aa = AXIS_ACTIONS[static_cast<size_t>(_axis)];

		float axis = 0.f;
		if (sf::Keyboard::isKeyPressed(m_inputMap.GetKey(aa.low)))
			axis -= 1.f;
		if (sf::Keyboard::isKeyPressed(m_inputMap.GetKey(aa.high)))
			axis += 1.f;

		return axis;
	}
}
