#include "keyboardinputmanager.hpp"

using Key = sf::Keyboard::Key;

namespace Input
{
	using namespace std::string_literals;
	using namespace Utils;

	const Utils::ConfigSection::Initializer<sf::Keyboard::Key,6> Keyboard1(
	{ {
		{"Jump", Key::RControl},
		{"JumpAlt", Key::RAlt},
		{"AccelerateCW", Key::Up},
		{"AccelerateCCW", Key::Down},
		{"RotateCW", Key::Left},
		{"RotateCCW", Key::Right}
	} });

	const Utils::ConfigSection::Initializer<sf::Keyboard::Key, 6> Keyboard2(
	{ {
		{"Jump", Key::Space},
		{"JumpAlt", Key::LAlt},
		{"AccelerateCW", Key::W},
		{"AccelerateCCW", Key::S},
		{"RotateCW", Key::A},
		{"RotateCCW", Key::D}
	} });

	KeyBoardInputInterface::KeyBoardInputInterface(const ConfigSection& _config)
		: m_inputMap({ 
			{Action::Jump, _config.GetValue<Key>("Jump")},
			{Action::JumpAlt, _config.GetValue<Key>("JumpAlt")},
			{Action::AccelerateCW, _config.GetValue<Key>("AccelerateCW")},
			{Action::AccelerateCCW, _config.GetValue<Key>("AccelerateCCW")},
			{Action::RotateCW, _config.GetValue<Key>("RotateCW")},
			{Action::RotateCCW, _config.GetValue<Key>("RotateCCW")} })
	{}

	bool KeyBoardInputInterface::IsKeyPressed(Action _action) const
	{
		const Key key = m_inputMap.GetKey(_action);
		return key == Key::Unknown ? false
			: sf::Keyboard::isKeyPressed(key);
	}

	float KeyBoardInputInterface::GetAxis(Axis _axis) const
	{
		const AxisAction aa = AXIS_ACTIONS[static_cast<size_t>(_axis)];

		float axis = 0.f;
		if (IsKeyPressed(aa.low))
			axis -= 1.f;
		if (IsKeyPressed(aa.high))
			axis += 1.f;

		return axis;
	}
}

std::istream& operator >> (std::istream& _in, Key& _key)
{
	int i;
	_in >> i;
	_key = static_cast<Key>(i);

	return _in;
}
