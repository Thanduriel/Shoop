#include "learning.hpp"
#include <filesystem>

namespace Game {

	namespace Learning {

	MLPImpl::MLPImpl(const Options& options_) : options(options_), out(nullptr) 
	{
		reset();
	}

	void MLPImpl::reset()
	{
		linLayers.clear();
		normLayers.clear();
		assert(options.layers() > 0);
		auto linOpts = torch::nn::LinearOptions(options.in_size(), options.hidden_size())
			.bias(options.with_bias());
		linLayers.emplace_back(linOpts);
		const auto normOpts = torch::nn::LayerNormOptions({ options.hidden_size() })
			.elementwise_affine(options.elementwise_affine());
		normLayers.emplace_back(normOpts);

		linOpts.in_features(options.hidden_size());
		for (int64_t i = 1; i < options.layers(); ++i)
		{
			linLayers.emplace_back(linOpts);
			normLayers.emplace_back(normOpts);
		}

		for (size_t i = 0; i < linLayers.size(); ++i)
		{
			const std::string num = std::to_string(i);
			register_module("lin_" + num, linLayers[i]);
			if (i < normLayers.size())
				register_module("norm_" + num, normLayers[i]);
		}

		out = torch::nn::Linear(
			torch::nn::LinearOptions(options.hidden_size(), options.out_size()).bias(options.with_bias()));
		register_module("out", out);
	}

	torch::Tensor MLPImpl::forward(torch::Tensor x) 
	{
		auto forwardLayer = [this](size_t idx, torch::Tensor x) {
			x = linLayers[idx]->forward(x);
			x = normLayers[idx]->forward(x);
			return torch::tanh(x);
		};
		x = forwardLayer(0, x);

		if (options.skip_connections()) 
		{
			for (size_t i = 1; i < linLayers.size(); ++i)
				x = forwardLayer(i, x) + x;
		}
		else 
		{
			for (size_t i = 1; i < linLayers.size(); ++i)
				x = forwardLayer(i, x);
		}

		return out(x);
	}
	
	} // namespace Learning

	using namespace Learning;

	std::vector<float> MakeLinSpace(int _numIntervals)
	{
		std::vector<float> values(_numIntervals);
		for (int i = 0; i < _numIntervals; ++i)
		{
			values[i] = -1.f + 2.f * (i / static_cast<float>(_numIntervals - 1));
		}

		return values;
	}

	std::vector<Input::InputState> MakeInputSpace(int _numIntervals)
	{
		constexpr size_t numAxis = static_cast<size_t>(Input::Axis::COUNT);
		constexpr size_t numActions = GAME_ACTIONS.size();

		size_t total = 1;
		for (size_t i = 0; i < numAxis; ++i)
			total *= _numIntervals;
		// actions are binary -> each doubles the number
		total <<= numActions;

		std::vector<Input::InputState> inputSpace(1);
		inputSpace.reserve(total);

		const std::vector<float> linAxisSpace = MakeLinSpace(_numIntervals);

		for (size_t ax = 0; ax < numAxis; ++ax)
		{
			const size_t numVals = inputSpace.size();
			// set first value for already existing combinations
			for (size_t k = 0; k < numVals; ++k)
				inputSpace[k].axis[ax] = linAxisSpace[0];
			for (int j = 1; j < _numIntervals; ++j)
			{
				const float val = linAxisSpace[j];
				for (size_t k = 0; k < numVals; ++k)
				{
					inputSpace.push_back(inputSpace[k]);
					inputSpace.back().axis[ax] = val;
				}
			}
		}

		for (size_t i = 0; i < numActions; ++i)
		{
			const size_t ac = static_cast<size_t>(GAME_ACTIONS[i]);
			const size_t numVals = inputSpace.size();
			for (size_t k = 0; k < numVals; ++k)
				inputSpace[k].actions[ac] = false;

			for (size_t k = 0; k < numVals; ++k)
			{
				inputSpace.push_back(inputSpace[k]);
				inputSpace.back().actions[ac] = true;
			}
		}

		return inputSpace;
	}

	constexpr int64_t GetSheepStateSize() 
	{
		constexpr int64_t NUM_VALUES = 8;
		static_assert(sizeof(SheepState) == sizeof(float) * NUM_VALUES);
		return sizeof(SheepState) / sizeof(float);
	}

	// Constructs a view based on the SheepState.
	torch::Tensor ToTensor(const SheepState& _state)
	{
		static const c10::TensorOptions options(c10::kFloat);

		return torch::from_blob(const_cast<SheepState*>(&_state), { GetSheepStateSize() }, options);
	}

	// ************************************************************************ //
	Dataset::Dataset(const std::string& _path, int _numIntervals)
	{
		namespace fs = std::filesystem;

		const std::vector<float> values = MakeLinSpace(_numIntervals);
		std::vector<float> intervals(values.size() + 1);
		intervals.front() = values.front();
		intervals.back() = values.back();
		for (size_t i = 1; i < values.size(); ++i)
			intervals[i] = (values[i - 1] + values[i]) * 0.5f;

		constexpr size_t numAxis = static_cast<size_t>(Input::Axis::COUNT);
		constexpr size_t numActions = GAME_ACTIONS.size();

		std::vector<torch::Tensor> inputsTemp;
		std::vector<int> outputsTemp;

		for (const auto& dirEntry : fs::recursive_directory_iterator(_path))
		{
			GameLog log; 
			log.Load(dirEntry.path().string());
			if (log.outcome == Outcome::Win)
			{
				for (const GameLog::State& state : log.states)
				{
					int idx = 0;
					int stride = 1;
					for (size_t ax = 0; ax < numAxis; ++ax) 
					{
						auto it = std::upper_bound(intervals.begin(), intervals.end(), state.input.axis[ax]);
						idx += stride * (std::distance(intervals.begin(), it) - 1);
						stride *= _numIntervals;
					}
					for (size_t i = 0; i < numActions; ++i)
					{
						const size_t ac = static_cast<size_t>(GAME_ACTIONS[i]);
						if (state.input.actions[ac])
							idx += stride;
						stride *= 2;
					}
					inputsTemp.emplace_back(torch::concat({ ToTensor(state.self), ToTensor(state.other) }));
					outputsTemp.push_back(idx);
				}
			}
		}
		inputs = torch::stack(inputsTemp);
		static const c10::TensorOptions options(c10::kInt);
		outputs = torch::from_blob(outputsTemp.data(), { static_cast<int64_t>(outputsTemp.size()) }, options).clone();
	}

	// ************************************************************************ //
	DoopAI::DoopAI(int _axisIntervals, Mode _mode, float _exploreRatio)
		: m_inputs(MakeInputSpace(_axisIntervals)),
		m_neuralNet(MLPOptions(GetSheepStateSize() * 2, 64, static_cast<int64_t>(m_inputs.size()))
			.layers(6)),
		m_mode(_mode),
		m_exploreRatio(_exploreRatio),
		m_random(0x142bfa)
	{
		Dataset testSet("gamelogs", _axisIntervals);
	}

	void DoopAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		// explore: do a random move
		if (m_random.Uniform() <= m_exploreRatio)
		{
			const int select = m_random.Uniform(0, static_cast<int32_t>(m_inputs.size()) - 1);
			_inp.state = m_inputs[select];
			return;
		}

		torch::Tensor x = torch::concat({ ToTensor(_self), ToTensor(_oth) });
		x = m_neuralNet->forward(x);
		// for SAMPLE it may be possible to skip initialization if select ~1.f
		int64_t idx = x.numel() - 1;
		switch (m_mode)
		{
		case Mode::MAX:
			idx = torch::argmax(x).item<int64_t>();
			break;
		case Mode::SAMPLE:
			x = torch::softmax(x, 0);
			const float select = m_random.Uniform();
			float probSum = 0.f;
			for (int64_t i = 0; i < x.numel(); ++i) 
			{
				probSum += x[i].item<float>();
				if (select < probSum)
				{
					idx = i;
					break;
				}
			}
			break;
		}
		_inp.state = m_inputs[idx];
		
	}
}