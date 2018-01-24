//
// Created by jinxpliskin on 11/16/17.
//
#pragma once

#include "SFML/Window.hpp"

namespace Input
{

	class KeyboardInputManager
	{
	public:
		static bool Pressed(sf::Keyboard::Key _key);
		static bool Upward(sf::Keyboard::Key _key);
		static bool Downward(sf::Keyboard::Key _key);

		static void UpdateKeys();

	private:
		static bool m_initialized;
		static int m_keyCount;
		static bool* m_previousKeyIsPressed;
		static bool* m_currentKeyIsPressed;

		static void Initialize();

		// Disallow creating an instance of this object
		KeyboardInputManager() {};
	};

}
