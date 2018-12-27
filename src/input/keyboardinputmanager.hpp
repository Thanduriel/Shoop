#pragma once

#include "SFML/Window.hpp"
#include "actions.hpp"
#include "inputmanager.hpp"

namespace Input
{

	class KeyBoardInputInterface : public InputInterface
	{
	public:
		KeyBoardInputInterface();

		bool IsKeyPressed(Action _action) const override;
		float GetAxis(Axis _axis) const override;

	private:
		InputMap<sf::Keyboard::Key, sf::Keyboard::Key::KeyCount> m_inputMap;
	};
}
