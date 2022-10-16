#include "learning.hpp"
#include "game.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace Learning {
	using namespace Game;

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
		SheepState state = _state;
		state.position = (state.position - Math::Vec2(8.f, 7.4f)) / 5.f;
		state.velocity /= 2.5f;
		state.wheelAngularVelocity /= 30.f;

		return torch::from_blob(const_cast<SheepState*>(&state), { GetSheepStateSize() }, options).clone();
	}

	// ************************************************************************ //
	Dataset::Dataset(const std::string& _path, int _numIntervals)
	{
		const std::vector<float> values = MakeLinSpace(_numIntervals);
		std::vector<float> intervals(values.size() + 1);
		intervals.front() = values.front();
		// make a little larger so that max values are assigned correctly
		intervals.back() = values.back() + 1.f;
		for (size_t i = 1; i < values.size(); ++i)
			intervals[i] = (values[i - 1] + values[i]) * 0.5f;

		constexpr size_t numAxis = static_cast<size_t>(Input::Axis::COUNT);
		constexpr size_t numActions = GAME_ACTIONS.size();

		std::vector<torch::Tensor> inputsTemp;
		std::vector<int> outputsTemp;

		auto testSpace = MakeInputSpace(_numIntervals);

		std::vector<GameLog> gameLogs;
		for (const auto& dirEntry : fs::recursive_directory_iterator(_path))
		{
			std::vector<GameLog> logs = GameLog::Load(dirEntry.path().string());
			gameLogs.insert(gameLogs.end(), logs.begin(), logs.end());
		}
		for (const GameLog& log : gameLogs)
		{
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
		m_inputs = torch::stack(inputsTemp);
		//	auto [stddev, mean] =  torch::std_mean(m_inputs, 0);
		//	std::cout << mean << " stddev: \n" << stddev << "\n";
		static const c10::TensorOptions options(c10::kInt);
		m_outputs = torch::from_blob(outputsTemp.data(), { static_cast<int64_t>(outputsTemp.size()) }, options)
			.to(c10::kLong, false, true);
	}

	torch::data::Example<> Dataset::get(size_t index)
	{
		return { m_inputs[index], m_outputs[index] };
	}

	c10::optional<size_t> Dataset::size() const
	{
		return m_inputs.sizes()[0];
	}

	// ************************************************************************ //
	// Make a deep copy of a module.
	template<typename Module>
	Module clone(const Module& _module)
	{
		using ModuleImpl = typename Module::Impl;
		return Module(std::dynamic_pointer_cast<ModuleImpl>(_module->clone()));
	}

	constexpr int numIntervals = 5;
	constexpr int64_t hiddenSize = 48;
	constexpr int64_t numLayers = 4;
	constexpr int64_t batchSize = 256;
	constexpr const char* netName = "net.pt";

	void Trainer::Train(const std::string& _path, const std::string& _name)
	{
		namespace dat = torch::data;

		const double lr = 0.005;
		const double wd = 1e-5;
		const int64_t numOutputs = MakeInputSpace(numIntervals).size();
		MLP net(MLPOptions(GetSheepStateSize() * 2, hiddenSize, numOutputs).layers(numLayers));
		MLP bestNet = clone(net);

		torch::Device device(torch::kCPU);
		if (torch::cuda::is_available())
			device = torch::kCUDA;

		constexpr bool USE_SEQ_SAMPLER = false;
		using Sampler = std::conditional_t<USE_SEQ_SAMPLER,
			dat::samplers::SequentialSampler,
			dat::samplers::RandomSampler>;
		auto data_loader = dat::make_data_loader<Sampler>(
			Dataset(_path, numIntervals).map(dat::transforms::Stack<>()),
			dat::DataLoaderOptions().batch_size(batchSize));

		float totalLoss = 0.0;
		float minLoss = std::numeric_limits<float>::max();
		int forwardRuns = 0;

		auto optimizer = torch::optim::AdamW(net->parameters(), torch::optim::AdamWOptions(lr)
			.weight_decay(wd));

		const int64_t numEpochs = 256;
		for (int64_t epoch = 1; epoch <= numEpochs; ++epoch)
		{
			net->train();
			for (dat::Example<>& batch : *data_loader)
			{
				torch::Tensor data = batch.data.to(device);
				torch::Tensor target = batch.target.to(device);

				auto closure = [&]()
				{
					net->zero_grad();
					torch::Tensor output = net->forward(data);
					torch::Tensor loss = torch::cross_entropy_loss(output, target);
					totalLoss += loss.item<float>();
					++forwardRuns; // count runs to normalize the training error

					loss.backward();

					return loss;
				};

				try {
					optimizer.step(closure);
				}
				catch (c10::Error err)
				{
					std::cout << err.msg() << "\n";
					std::abort();
				}
			}

			const float loss = totalLoss / forwardRuns;
			if (minLoss < loss)
			{
				minLoss = loss;
				bestNet = clone(net);
			}
			std::cout << "epoch: " << epoch << " loss: " << loss << "\n";
		}
		torch::save(bestNet, _name + ".pt");

	}

	// ************************************************************************ //
	void ReinforcmentLoop::Run()
	{
		std::vector<std::string> neuralNetworkNames;
		neuralNetworkNames.push_back("");

		Shoop game(1366, 768);
		auto& section = game.Config().GetSection("learning");
		game.Config().GetSection("general").SetValue("LogGames", 1);
//		auto& gameplayConf = game.Config().GetSection("gameplay");

		constexpr int numEpochs = 8;
		for (int i = 0; i < numEpochs; ++i)
		{
			const std::string epoch = std::to_string(i);
			const std::string path = "gamelogs_" + epoch;
			const std::string netName = "net_" + epoch;

			// create data
			spdlog::info("Generating data {}.", path);
			fs::create_directory(path);
			section.SetValue("LogPath", path);
			section.SetValue("NetworkName0", neuralNetworkNames.back());
			section.SetValue("NetworkName1", neuralNetworkNames.back());
			const float explore = i == 0 ? 1.f : (1.f - (i / static_cast<float>(numEpochs))) * 0.5f;
			section.SetValue("ExploreRatio", explore);
			section.SetValue("LogGames", 1);
			game.Run();

			// train new network
			spdlog::info("Training network {}.", netName);
			Trainer train;
			train.Train(path, netName);
			neuralNetworkNames.push_back(netName);
		}
	}

	void ReinforcmentLoop::Evaluate()
	{
		Shoop game(1366, 768);
		std::vector<std::string> neuralNetworkNames;

		for (int i = 0; i < 8; ++i)
			neuralNetworkNames.push_back("net_" + std::to_string(i));

		auto& learningConf = game.Config().GetSection("learning");
		learningConf.SetValue("LogGames", 0);
		learningConf.SetValue("ExploreRatio", 0.0);

		auto& gameplayConf = game.Config().GetSection("gameplay");
		gameplayConf.SetValue("numWinsRequired", std::numeric_limits<int>::max());
		gameplayConf.SetValue("maxNumGames", 512);

		for(size_t i = 0; i < neuralNetworkNames.size(); ++i)
			for (size_t j = i; j < neuralNetworkNames.size(); ++j)
			{
				spdlog::info("{} vs {}", neuralNetworkNames[i], neuralNetworkNames[j]);
				learningConf.SetValue("NetworkName0", neuralNetworkNames[i]);
				learningConf.SetValue("NetworkName1", neuralNetworkNames[j]);
				game.Run();
				learningConf.SetValue("NetworkName0", neuralNetworkNames[j]);
				learningConf.SetValue("NetworkName1", neuralNetworkNames[i]);
				game.Run();
			}
	}

} // namespace Learning


	// ************************************************************************ //

namespace Game {

	using namespace Learning;

	DoopAI::DoopAI(const std::string& _netName, Mode _mode, float _exploreRatio)
		: m_inputs(MakeInputSpace(numIntervals)),
		m_neuralNet(MLPOptions(GetSheepStateSize() * 2, hiddenSize, static_cast<int64_t>(m_inputs.size()))
			.layers(numLayers)),
		m_mode(_mode),
		m_exploreRatio(_exploreRatio),
		m_random(0x142bfa)
	{
		const std::string netName = _netName + ".pt";
		if (fs::exists(netName))
			torch::load(m_neuralNet, netName);
	}

	void DoopAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		// explore: do a random move
		if (m_exploreRatio > 0.f && m_random.Uniform() <= m_exploreRatio)
		{
			const int select = m_random.Uniform(0, static_cast<int32_t>(m_inputs.size()) - 1);
			_inp.state = m_inputs[select];
			return;
		}

		torch::Tensor x;
		try {
			x = torch::concat({ ToTensor(_self), ToTensor(_oth) });
			x = torch::softmax(m_neuralNet->forward(x), 0);
		}
		catch (c10::Error err)
		{
			std::cout << err.msg() << "\n";
			std::abort();
		}
		
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