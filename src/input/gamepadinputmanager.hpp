//
// Created by jinxpliskin on 11/16/17.
//

#pragma once

#include <array>

namespace Input
{
	enum GamePadButton { A, B, X, Y, LB, RB, Select, Start, COUNT };

	class GamePadInputManager
	{
	public:
		static float Pressed(GamePadButton _button, unsigned int _gamePadIndex);
		static float Upward(GamePadButton _button, unsigned int _gamePadIndex);
		static float Downward(GamePadButton _button, unsigned int _gamePadIndex);

		//static Vector2 GetLeftStick(unsigned int _gamePadIndex);
		//static Vector2 GetRightStick(unsigned int _gamePadIndex);

		static int GetNumConnectedGamePads();
		static bool IsConnected(unsigned int _gamePadIndex);

		static void UpdateGamePads();

	private:
		static void Initialize();
		static void RegisterPad(unsigned int _gamePadIndex);
		static void UnregisterPad(unsigned int _gamePadIndex);

		struct GamePadInput
		{
		public:
			GamePadInput();
			~GamePadInput();

			bool doesExist;

			//Vector2 leftStick;
			//Vector2 rightStick;

			// -1, when released. 1, when pressed.
			float lt;
			float rt;

			std::array<bool, (int)GamePadButton::COUNT> previousButtonIsPressed;
			std::array<bool, (int)GamePadButton::COUNT> currentButtonIsPressed;

		};

		static GamePadInput* m_gamePadInputs;

		static const float m_deadZone;
		static const int m_numSupportedPads;

		static bool m_initialized;

		static int m_numConnectedPads;

		//static Vector2 AdjustByDeadZone(Vector2 v) {	if(v.lengthSqr < m_deadZone) {Vector2.Zero;} else {return v;} }

		// Disallow creating an instance of this object
		GamePadInputManager() {};
	};

}
