//
// Created by jinxpliskin on 11/16/17.
//

#pragma once

namespace Input
{
	enum Device
	{
		KeyboardArrows	= 0x10000,
		KeyboardWASD	= 0x10001,
		Gamepad1		= 0x01000,
		Gamepad2		= 0x01001,
		Gamepad3		= 0x01010,
		Gamepad4		= 0x01011,
	};

	enum DeviceFlags
	{
		KeyboardFlag 	= 0x10000,
		GamePadFlag 	= 0x01000,
	};

	static bool IsKeyboard(Device _device) { return _device & DeviceFlags::KeyboardFlag; };
	static bool IsGamePad(Device _device) { return _device & DeviceFlags::GamePadFlag; };
}