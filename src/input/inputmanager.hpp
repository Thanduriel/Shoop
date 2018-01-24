//
// Created by jinxpliskin on 11/15/17.
//
#pragma once

#include <string>
#include <array>
#include <variant>
#include "device.hpp"
#include "abstactinput.hpp"

namespace Input
{
	/* ******************************
	 * Global manager for Input Queries
	 */
	class InputManager
	{
	public:
		enum Mode {Pressed, Downward, Upward};

		static float GetAxis(Axis _axisName);
		static float GetAxis(Axis _axisName, Device _device);
		static bool GetKeyUpward(Key _keyName);
		static bool GetKeyUpward(Key _keyName, Device _device) { return GetKey(_keyName, _device, Upward); }
		static bool GetKeyDownward(Key _keyName);
		static bool GetKeyDownward(Key _keyName, Device _device) { return GetKey(_keyName, _device, Downward); }
		static bool GetKeyPressed(Key _keyName);
		static bool GetKeyPressed(Key _keyName, Device _device) { return GetKey(_keyName, _device, Pressed); }

		static bool GetKey(Key _keyName, Device _device, Mode _mode);

	private:
	};
}