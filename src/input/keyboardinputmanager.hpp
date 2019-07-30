#pragma once

#include "SFML/Window.hpp"
#include "actions.hpp"
#include "inputmanager.hpp"
#include "utils/config.hpp"

namespace Input
{

	class KeyBoardInputInterface : public InputInterface
	{
	public:
		KeyBoardInputInterface(const Utils::ConfigSection& _config);

		bool IsKeyPressed(Action _action) const override;
		float GetAxis(Axis _axis) const override;

	private:
		InputMap<sf::Keyboard::Key, sf::Keyboard::Key::KeyCount> m_inputMap;
	};

	// default keyboard mappings
	const extern Utils::ConfigSection::Initializer<sf::Keyboard::Key, 5> Keyboard1;
	const extern Utils::ConfigSection::Initializer<sf::Keyboard::Key, 5> Keyboard2;
}

// allow reading of keys from a config
std::istream& operator >> (std::istream& _in, sf::Keyboard::Key& _key);
