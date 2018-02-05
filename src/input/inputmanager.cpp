//
// Created by jinxpliskin on 11/15/17.
//

#include "inputmanager.hpp"

namespace Input
{
	float InputManager::GetAxis(Axis _axisName)
	{
		return false;
	}

	float InputManager::GetAxis(Axis _axisName, Device _device)
	{
		return false;
	}

	bool GetKeyUpward(Key _keyName)
	{
		//for(const Device device : )
		return false;
	}

	bool GetKeyUpward(Key _keyName, Device _device)
	{
		return false;
	}

	bool GetKeyDownward(Key _keyName)
	{
		return false;
	}
	bool GetKeyDownward(Key _keyName, Device _device)
	{
		return false;
	}

	bool GetKeyPressed(Key _keyName)
	{
		return false;
	}

	bool GetKeyPressed(Key _keyName, Device _device)
	{
		return false;
	}

	bool GetKey(Key _keyName, Device _device, Input::InputManager::Mode _mode)
	{
		bool previousState;
		bool currentState;

		if(Input::IsKeyboard(_device))
		{
			switch(_device)
			{
				case Input::Device::KeyboardArrows:
					switch (_keyName)
					{

					}

					break;
			}
		}

		return false;
	}
}