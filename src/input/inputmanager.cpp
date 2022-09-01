#include "inputmanager.hpp"

namespace Input {

	bool VirtualInputs::IsKeyPressed(Action _action) const
	{
		return actions[static_cast<size_t>(_action)];
	}

	float VirtualInputs::GetAxis(Axis _axis) const
	{
		return axis[static_cast<size_t>(_axis)];
	}

}