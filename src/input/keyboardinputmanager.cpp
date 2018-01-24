//
// Created by jinxpliskin on 11/16/17.
//

#include "keyboardinputmanager.hpp"

namespace Input
{
	bool KeyboardInputManager::m_initialized;
	int KeyboardInputManager::m_keyCount;
	bool* KeyboardInputManager::m_previousKeyIsPressed;
	bool* KeyboardInputManager::m_currentKeyIsPressed;

	void KeyboardInputManager::Initialize()
	{
		KeyboardInputManager::m_keyCount = (int) sf::Keyboard::KeyCount;
		m_previousKeyIsPressed = new bool[m_keyCount];
		m_currentKeyIsPressed = new bool[m_keyCount];

		m_initialized = true;
	}

	void KeyboardInputManager::UpdateKeys()
	{
		if (!m_initialized) { Initialize(); }

		for (int i = 0; i < m_keyCount; ++i) {
			m_previousKeyIsPressed[i] = m_currentKeyIsPressed[i];
			m_currentKeyIsPressed[i] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key) i);
		}
	}

	bool KeyboardInputManager::Pressed(sf::Keyboard::Key _key)
	{
		if (!m_initialized) { Initialize(); }

		return m_currentKeyIsPressed[(int) _key];
	}

	bool KeyboardInputManager::Upward(sf::Keyboard::Key _key)
	{
		if (!m_initialized) { Initialize(); }

		return m_previousKeyIsPressed[(int) _key] && !m_currentKeyIsPressed[(int) _key];
	}

	bool KeyboardInputManager::Downward(sf::Keyboard::Key _key)
	{
		if (!m_initialized) { Initialize(); }

		return !m_previousKeyIsPressed[(int) _key] && m_currentKeyIsPressed[(int) _key];
	}
}
