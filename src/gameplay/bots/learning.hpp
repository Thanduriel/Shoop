#pragma once

#ifdef LEARNING_AI

namespace Game {

	class Doop
	{
	public:
		void operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp);

	private:
	};
}

#endif