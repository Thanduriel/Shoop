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
#include "generators/random.hpp"

namespace Game {

	namespace Learning {

	struct MLPOptions {
		MLPOptions(int64_t in_size, int64_t hidden_size, int64_t out_size)
			: in_size_(in_size), hidden_size_(hidden_size), out_size_(out_size) {
		}

		TORCH_ARG(int64_t, in_size);
		TORCH_ARG(int64_t, hidden_size);
		TORCH_ARG(int64_t, out_size);
		TORCH_ARG(int64_t, layers) = 1;
		TORCH_ARG(bool, with_bias) = true;
		TORCH_ARG(bool, skip_connections) = true;
		TORCH_ARG(bool, elementwise_affine) = true;
		//TORCH_ARG(ACT, activation) = ACT::tanh;
	};

	struct MLPImpl : public torch::nn::Cloneable<MLPImpl > {
		using Options = MLPOptions;

		explicit MLPImpl(const Options& _options);

		void reset() override;

		torch::Tensor forward(torch::Tensor x);

		Options                             options;
		torch::nn::Linear                   out;
		std::vector<torch::nn::Linear>      linLayers;
		std::vector<torch::nn::LayerNorm>   normLayers;
	};
	TORCH_MODULE(MLP);

	class Dataset
	{
	public:
		Dataset(const std::string& _path, int _numIntervals);

	private:
		torch::Tensor inputs;
		torch::Tensor outputs;
	};

	} // namespace Learning

	class DoopAI
	{
	public:
		enum struct Mode {
			MAX,
			SAMPLE
		};
		// @_axisIntervals Number of discrete intervals for the input axis.
		// Should be odd so that 0 is included.
		DoopAI(int _axisIntervals, Mode _mode, float _exploreRatio);

		void operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp);

	private:
		std::vector<Input::InputState> m_inputs;
		Learning::MLP m_neuralNet;
		Mode m_mode;
		float m_exploreRatio;
		Generators::RandomGenerator m_random;
	};
}

#endif