#pragma once

#include <array>
#include <unordered_map>

namespace Input
{
	enum struct Axis
	{
		Rotate,
		Accelerate,
		COUNT
	};

	enum struct Action
	{
		None,

		Jump,
		Confirm,
		Cancel,

		MenuDown,
		MenuLeft,
		MenuRight,
		MenuUp,

		// axis actions
		RotateCW,
		RotateCCW,
		AccelerateCW,
		AccelerateCCW,

		COUNT
	};

	// map axis to pairs of actions on keyboards
	struct AxisAction
	{
		Action low;
		Action high;
	};

	constexpr std::array<AxisAction,  static_cast<size_t>(Axis::COUNT)> AXIS_ACTIONS =
	{ {
		{Action::RotateCW, Action::RotateCCW},
		{Action::AccelerateCW, Action::AccelerateCCW}
	} };
}
