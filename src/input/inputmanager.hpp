#pragma once

#include <string>
#include <array>
#include "actions.hpp"

namespace Input
{
	class InputInterface
	{
	public:
		virtual ~InputInterface() {};

		virtual bool IsKeyPressed(Action _action) const = 0;
		// Axis values range from [-1,1].
		virtual float GetAxis(Axis _axis) const = 0;
	};

	/* ******************************
	 * 
	 */
	template<typename T, T Default>
	class InputMap
	{
	public:
		InputMap(std::initializer_list<std::pair<const Action, T>> _init)
			: m_keyMap(_init)
		{}

		T GetKey(Action _action) const 
		{
			auto it = m_keyMap.find(_action);

			return it != m_keyMap.end() ? it->second : Default;
		}

	private:
		std::unordered_map<Action, T> m_keyMap;
	};

	struct InputState
	{
		std::array<float, static_cast<size_t>(Axis::COUNT)> axis = {};
		std::array<bool, static_cast<size_t>(Action::COUNT)> actions = {};
	};

	class VirtualInputs : public InputInterface
	{
	public:
		InputState state;
		virtual bool IsKeyPressed(Action _action) const override;
		virtual float GetAxis(Axis _axis) const override;
	};
}