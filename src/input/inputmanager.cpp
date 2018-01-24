//
// Created by jinxpliskin on 11/15/17.
//

#include "inputmanager.hpp"

namespace Input
{
	float InputManager::GetAxis(Axis _axisName)
	{

	}

	float InputManager::GetAxis(Axis _axisName, Device _device)
	{

	}

	bool GetKeyUpward(Key _keyName)
	{
		//for(const Device device : )
	}

	bool GetKeyUpward(Key _keyName, Device _device)
	{

	}

	bool GetKeyDownward(Key _keyName)
	{

	}
	bool GetKeyDownward(Key _keyName, Device _device)
	{

	}

	bool GetKeyPressed(Key _keyName)
	{

	}

	bool GetKeyPressed(Key _keyName, Device _device)
	{

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
	}
}