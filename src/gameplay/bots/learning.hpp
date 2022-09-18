#pragma once

#ifdef LEARNING_AI
#include <type_traits>
/*namespace std {
	template< class F, class... ArgTypes>
	using result_of = invoke_result < F, ArgTypes...>;
	template< class F, class... ArgTypes>
	using result_of_t = typename invoke_result<F, ArgTypes...>::type;
}*/

#include <torch/torch.h>
#include "../elements/player/aicontrollercomponent.hpp"

namespace Game {

	class MLP : public torch::nn::Cloneable<MLP>
	{
	public:
		void reset() override;
		torch::Tensor forward(const torch::Tensor& _inputs) const;
	};

	class DoopAI
	{
	public:
		// @_axisIntervals Number of discrete intervals for the input axis.
		// Should be odd so that 0 is included.
		DoopAI(int _axisIntervals);

		void operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp);

	private:
		MLP m_neuralNet;
		std::vector<Input::InputState> m_inputs;
	};
}

#endif