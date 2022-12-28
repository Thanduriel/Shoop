#pragma once

#ifdef LEARNING_AI
#include <type_traits>

#include <torch/torch.h>
#include "../elements/player/aicontrollercomponent.hpp"
#include "generators/random.hpp"


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
	//	TORCH_ARG(ACT, activation) = ACT::tanh;
	};

	struct MLPImpl : public torch::nn::Cloneable<MLPImpl > 
	{
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

	struct TwinMLPImpl : public torch::nn::Cloneable<TwinMLPImpl>
	{
		using Options = MLPOptions;

		explicit TwinMLPImpl(const Options& _twinOptions, const Options& _outOptions);

		void reset() override;

		torch::Tensor forward(torch::Tensor x);

		Options twinOptions;
		Options outputOptions;

		MLP twinMLP;
		MLP outputMLP;
	};
	TORCH_MODULE(TwinMLP);

	using NetType = MLP;

	class Dataset : public torch::data::Dataset<Dataset>
	{
	public:
		Dataset(const std::string& _path, int _numIntervals, 
			torch::Device _device = torch::kCPU, 
			bool _winsOnly = true,
			bool _withStep = true,
			size_t _maxStepsPerGame = std::numeric_limits<size_t>::max(),
			bool _alternativeMode = false);

		Dataset(const torch::Tensor& inputs, const torch::Tensor& outputs);

		// @param _ratio Number of samples to move into the new dataset
		Dataset Split(float _ratio);

		torch::data::Example<> get(size_t index) override;
		c10::optional<size_t> size() const override;
		void to(torch::Device _device);
	private:
		torch::Tensor m_inputs;
		torch::Tensor m_outputs;
	};

	class Trainer
	{
	public:
		using CustomTestFn = std::function<float(const NetType&)>;
		void Train(const std::string& _path, const std::string& _name, CustomTestFn _testFn);
	};

} // namespace Learning

namespace Game {

	class DoopAI
	{
	public:
		enum struct Mode {
			MAX,
			SAMPLE,
			SAMPLE_FILTERED, // filter 50 %
			SAMPLE_FILTERED_90, // filter 90 %
			RANDOM // equivalent to 100% explore ratio
		};
		// @_axisIntervals Number of discrete intervals for the input axis.
		// Should be odd so that 0 is included.
		DoopAI(const std::string& _netName, Mode _mode, float _exploreRatio);

		void operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp);

	private:
		std::vector<Input::InputState> m_inputs;
		torch::Tensor m_inputsAsTensor;
		int64_t m_numInputs;
		Learning::NetType m_neuralNet;
		Mode m_mode;
		float m_exploreRatio;
		Generators::RandomGenerator m_random;
	};
} // namespace Game

namespace Learning {
	class ReinforcmentLoop
	{
	public:
		ReinforcmentLoop(const std::string& _netName = "net_", 
			const std::string& _logName = "gamelogs_");

		void Run(int _numGamesPerEpoch, unsigned _numThreads = 0);
		void Evaluate();

		struct RLBot
		{
			std::string neuralNetworkName;
			Game::DoopAI::Mode mode;
		};
		void Evaluate(const std::vector<RLBot>& _bots, int _numGames, size_t _numThreads, bool _playMirror = true);
	private:
		std::string m_netName;
		std::string m_logName;
	};

} // namespace Game

#endif