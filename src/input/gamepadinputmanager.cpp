//
// Created by jinxpliskin on 11/16/17.
//

#include <SFML/Window/Joystick.hpp>
#include "gamepadinputmanager.hpp"

namespace Input
{
	GamePadInputManager::GamePadInput::GamePadInput()
	{
		lt = -1.0F;
		rt = -1.0F;
	}

	GamePadInputManager::GamePadInput::~GamePadInput()
	{
	}

	GamePadInputManager::GamePadInput *GamePadInputManager::m_gamePadInputs;

	const float GamePadInputManager::m_deadZone = 0.05F;
	const int GamePadInputManager::m_numSupportedPads = 8;

	bool GamePadInputManager::m_initialized;

	int GamePadInputManager::m_numConnectedPads;

	void GamePadInputManager::Initialize()
	{
		m_gamePadInputs = new GamePadInput[m_numSupportedPads];

		sf::Joystick::update();

		m_numConnectedPads = 0;
		for (unsigned int i = 0; i < m_numSupportedPads; ++i)
		{
			if (sf::Joystick::isConnected(i))
			{
				RegisterPad(i);
			}
		}

		m_initialized = true;
	}

	void GamePadInputManager::RegisterPad(unsigned int _gamePadIndex)
	{
		if (!m_gamePadInputs[_gamePadIndex].doesExist)
		{
			m_gamePadInputs[_gamePadIndex].doesExist = true;
			m_numConnectedPads++;
		}
	}

	void GamePadInputManager::UnregisterPad(unsigned int _gamePadIndex)
	{
		if (m_gamePadInputs[_gamePadIndex].doesExist)
		{
			m_gamePadInputs[_gamePadIndex].doesExist = false;
			m_numConnectedPads--;
		}
	}

	void GamePadInputManager::UpdateGamePads()
	{
		if(!m_initialized) { Initialize(); }

		sf::Joystick::update();

		for(unsigned int index = 0; index < m_numSupportedPads; ++index)
		{
			if(!sf::Joystick::isConnected(index))
			{
				UnregisterPad(index);
			}
			else
			{
				RegisterPad(index);

				GamePadInput input = m_gamePadInputs[index];

				for(int i = 0; i < (int)GamePadButton::COUNT; ++i)
				{
					input.previousButtonIsPressed[i] = input.currentButtonIsPressed[i];
					input.currentButtonIsPressed[i] = sf::Joystick::isButtonPressed(index, i);
				}

				input.lt = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::Z) * 0.01F;
				input.rt = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::R) * 0.01F;

				m_gamePadInputs[index] = input;
			}
		}
	}

	float GamePadInputManager::Pressed(GamePadButton _button, unsigned int _gamePadIndex)
	{
		if (!m_initialized) { Initialize(); }

		return m_gamePadInputs[_gamePadIndex].currentButtonIsPressed[(int)_button];
	}

	float GamePadInputManager::Upward(GamePadButton _button, unsigned int _gamePadIndex)
	{
		if (!m_initialized) { Initialize(); }

		GamePadInput inputs = m_gamePadInputs[_gamePadIndex];
		return inputs.previousButtonIsPressed[(int)_button] && !inputs.currentButtonIsPressed[(int)_button];
	}

	float GamePadInputManager::Downward(GamePadButton _button, unsigned int _gamePadIndex)
	{
		if (!m_initialized) { Initialize(); }

		GamePadInput inputs = m_gamePadInputs[_gamePadIndex];
		return !inputs.previousButtonIsPressed[(int)_button] && inputs.currentButtonIsPressed[(int)_button];
	}
}