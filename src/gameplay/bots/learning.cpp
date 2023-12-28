#include "learning.hpp"
#include "game.hpp"
#include "evaluation.hpp"
#include <filesystem>
#include <thread>
#include <future>
#include <fstream>

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
			return torch::gelu(x);
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

	TwinMLPImpl::TwinMLPImpl(const Options& _twinOptions, const Options& _outputOptions)
		: twinOptions(_twinOptions),
		outputOptions(_outputOptions),
		twinMLP(nullptr),
		outputMLP(nullptr)
	{
		assert(_twinOptions.out_size() * 2 == _outputOptions.in_size());
		reset();
	}

	void TwinMLPImpl::reset()
	{
		twinMLP = MLP(twinOptions);
		register_module("twinMLP", twinMLP);
		outputMLP = MLP(outputOptions);
		register_module("outMLP", outputMLP);
	}

	torch::Tensor TwinMLPImpl::forward(torch::Tensor x)
	{
		const int64_t s = x.sizes().back() / 2;
		const torch::Tensor x1 = twinMLP(x.slice(-1, 0, s));
		const torch::Tensor x2 = twinMLP(x.slice(-1, s));
		return outputMLP(torch::concat({ x1,x2 }, -1));
	}

	// ************************************************************************ //
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

	constexpr size_t NUM_AXIS = static_cast<size_t>(Input::Axis::COUNT);
	constexpr size_t NUM_ACTIONS = static_cast<size_t>(Input::Action::COUNT);
	constexpr size_t PLAYER_INPUT_SIZE = NUM_AXIS + NUM_ACTIONS;
	torch::Tensor ToTensor(const Input::InputState& _inputState)
	{
		torch::Tensor tensor = torch::empty(PLAYER_INPUT_SIZE, c10::TensorOptions(c10::kFloat));
		auto buf = tensor.accessor<float, 1>();
		for (size_t i = 0; i < NUM_AXIS; ++i)
			buf[i] = _inputState.axis[i];
		for (size_t i = 0; i < NUM_ACTIONS; ++i)
			buf[i+NUM_AXIS] = _inputState.actions[i];

		return tensor;
	}

	// ************************************************************************ //
	Dataset::Dataset(const std::string& _path, int _numIntervals, torch::Device _device, 
		bool _winsOnly,
		bool _withStep,
		size_t _maxStepsPerGame,
		bool _alternativeMode)
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

		auto DiscretizeInputs = [&](const GameLog::State& state)
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
			return idx;
		};

		std::vector<torch::Tensor> inputsTemp;
		std::vector<int> outputsTemp;

		auto testSpace = MakeInputSpace(_numIntervals);

		std::vector<GameLog> gameLogs;
		std::vector<std::string> logFiles;
		for (const auto& dirEntry : fs::recursive_directory_iterator(_path))
			logFiles.push_back(dirEntry.path().string());
		if (logFiles.size() % 2 != 0) 
		{
			spdlog::error("Expected an even number of game log files in \"{}\".", _path);
			std::abort();
		}
		// lexicographic order allows us to match logs from the same game from both players
		std::sort(logFiles.begin(), logFiles.end());
		for (size_t fileIdx = 0; fileIdx < logFiles.size(); fileIdx += 2)
		{
			std::vector<GameLog> logsP1 = GameLog::Load(logFiles[fileIdx]);
			std::vector<GameLog> logsP2 = GameLog::Load(logFiles[fileIdx + 1]);
			if (logsP1.size() != logsP2.size())
			{
				spdlog::error("Number of logged games does not match between players with {} != {}.", logsP1.size(), logsP2.size());
				std::abort();
			}

			for (size_t logIdx = 0; logIdx < logsP1.size(); ++logIdx)
			{
				if (logsP1[logIdx].outcome == Outcome::Draw)
					continue;

				const GameLog& log = logsP1[logIdx].outcome == Outcome::Win ? logsP1[logIdx] : logsP2[logIdx];
				const GameLog& logLoss = logsP1[logIdx].outcome == Outcome::Win ? logsP2[logIdx] : logsP1[logIdx];

				const size_t start = _maxStepsPerGame < log.states.size() 
					? log.states.size() - _maxStepsPerGame : 0u;
				for (size_t i = start; i < log.states.size(); ++i)
				{
					if (_alternativeMode)
					{
						torch::Tensor stateSelf = ToTensor(log.states[i].self);
						torch::Tensor stateOther = ToTensor(log.states[i].other);

						inputsTemp.emplace_back(torch::concat({ stateSelf,
							ToTensor(log.states[i].input), 
							stateOther }));
						outputsTemp.push_back(1);
						if (_withStep)
							outputsTemp.push_back(static_cast<int>(log.states.size() - i));

						inputsTemp.emplace_back(torch::concat({ stateOther,
							ToTensor(logLoss.states[i].input), 
							stateSelf }));
						outputsTemp.push_back(0);
						if (_withStep)
							outputsTemp.push_back(static_cast<int>(log.states.size() - i));
					}
					else
					{
						const int winIdx = DiscretizeInputs(log.states[i]);

						inputsTemp.emplace_back(torch::concat({ ToTensor(log.states[i].self),
							ToTensor(log.states[i].other) }));
						outputsTemp.push_back(winIdx);

						if (!_winsOnly)
						{
							const int lossIdx = DiscretizeInputs(logLoss.states[i]);
							//	std::cout << winIdx << " " << lossIdx << "\n";
							outputsTemp.push_back(lossIdx);
						}

						if (_withStep)
							outputsTemp.push_back(static_cast<int>(log.states.size() - i));
					}
				}
			}
		}
		m_inputs = torch::stack(inputsTemp).to(_device);
		//	auto [stddev, mean] =  torch::std_mean(m_inputs, 0);
		//	std::cout << mean << " stddev: \n" << stddev << "\n";
		static const c10::TensorOptions options(c10::kInt);
		m_outputs = torch::from_blob(outputsTemp.data(), { static_cast<int64_t>(outputsTemp.size()) }, options)
			.to(_device, _alternativeMode ? c10::kFloat : c10::kLong, false, true);
		int64_t valuesPerSample = 1;
		// wins / loss inputs are alternating
		if (!_alternativeMode && !_winsOnly)
			++valuesPerSample;
		if (_withStep)
			++valuesPerSample;
		m_outputs = m_outputs.reshape({ m_outputs.size(0) / valuesPerSample, valuesPerSample });
	}

	Dataset::Dataset(const torch::Tensor& inputs, const torch::Tensor& outputs)
		: m_inputs(inputs),
		m_outputs(outputs)
	{
	}

	Dataset Dataset::Split(float _ratio)
	{
		const int64_t splitIdx = (1.f-_ratio) * m_inputs.sizes()[0];
		torch::Tensor newInputs = m_inputs.slice(0, splitIdx);
		torch::Tensor newOutputs = m_outputs.slice(0, splitIdx);

		m_inputs = m_inputs.slice(0, 0, splitIdx);
		m_outputs = m_outputs.slice(0, 0, splitIdx);

		return Dataset(newInputs, newOutputs);
	}

	torch::data::Example<> Dataset::get(size_t index)
	{
		return { m_inputs[index], m_outputs[index] };
	}

	c10::optional<size_t> Dataset::size() const
	{
		return m_inputs.sizes()[0];
	}

	void Dataset::to(torch::Device _device)
	{
		m_inputs = m_inputs.to(_device);
		m_outputs = m_outputs.to(_device);
	}

	// ************************************************************************ //
	// Make a deep copy of a module.
	template<typename Module>
	Module clone(const Module& _module)
	{
		using ModuleImpl = typename Module::Impl;
		return Module(std::dynamic_pointer_cast<ModuleImpl>(_module->clone()));
	}

	torch::Tensor LabelMSE(torch::Tensor pred, torch::Tensor target, int numClasses, float k = 1.f, float M = 1.f)
	{
		using namespace torch::indexing;

		const float c = 1.f / numClasses;
		torch::Tensor correct = k * (pred.index({ Slice(), target }) - M).square();
		// subtracting the true label is easier then just selecting the false ones
		torch::Tensor oth = pred.square().sum(-1) - pred.index({ Slice(), target }).square();

		return c * (correct + oth).mean();
	}

	constexpr int numIntervals = 5;
	constexpr int64_t numLayersTwin = 3;
	constexpr int64_t hiddenSizeTwin = 32;
	constexpr int64_t outSizeTwin = 32;
	constexpr int64_t hiddenSize = 64;
	constexpr int64_t numLayers = 6;
	constexpr int64_t batchSize = 16384;//1024;
	constexpr int64_t numEpochs = 512;
	constexpr double lr = 0.01;
	constexpr double wd = 1e-5;
	constexpr float validDataSplit = 0.25f;

	// only for cross_entropy training
	constexpr bool useWinsOnly = false;
	constexpr float winWeighting = 0.5f;


	constexpr size_t maxStepsPerGame = 32;
	constexpr bool useStepWeighting = true;
	constexpr bool alternativeMode = true;

	template<typename Net>
	Net MakeNet();

	template <>
	auto MakeNet<MLP>() -> MLP {
		int64_t numInputs = GetSheepStateSize() * 2;
		int64_t numOutputs;
		if (alternativeMode)
		{
			numInputs += PLAYER_INPUT_SIZE;
			numOutputs = 1;
		}
		else
		{
			const int64_t numPlayerInputs = MakeInputSpace(numIntervals).size();
			numOutputs = useWinsOnly ? numPlayerInputs : numPlayerInputs * 2;
		}

		return MLP(MLPOptions(numInputs, hiddenSize, numOutputs)
			.layers(numLayers));
	}

	template <>
	auto MakeNet<TwinMLP>() -> TwinMLP {
		const int64_t numOutputs = MakeInputSpace(numIntervals).size();
		const auto twinOptions = MLPOptions(GetSheepStateSize(), hiddenSizeTwin, outSizeTwin).layers(numLayersTwin);
		const auto outOptions = MLPOptions(outSizeTwin*2, hiddenSize, useWinsOnly ? numOutputs : numOutputs * 2).layers(numLayers);
		return TwinMLP(twinOptions, outOptions);
	}


	void Trainer::Train(const std::string& _path, const std::string& _name, CustomTestFn _testFn)
	{
		namespace dat = torch::data;

		const int64_t numOutputs = MakeInputSpace(numIntervals).size();
		NetType net = MakeNet<NetType>();
		NetType bestNet = clone(net);

		torch::Device device(torch::kCPU);
		if (torch::cuda::is_available()) 
		{
			device = torch::kCUDA;
			net->to(device);
			spdlog::info("Using cuda for training.");
		}

		constexpr bool USE_SEQ_SAMPLER = false;
		using Sampler = std::conditional_t<USE_SEQ_SAMPLER,
			dat::samplers::SequentialSampler,
			dat::samplers::RandomSampler>;
		Dataset dataset(_path, numIntervals, device, useWinsOnly, useStepWeighting, maxStepsPerGame, alternativeMode);
		auto valid_loader = dat::make_data_loader<Sampler>(
			dataset.Split(validDataSplit)
			.map(dat::transforms::Stack<>()),
			dat::DataLoaderOptions().batch_size(batchSize));
		auto data_loader = dat::make_data_loader<Sampler>(
			std::move(dataset)
				.map(dat::transforms::Stack<>()),
					dat::DataLoaderOptions().batch_size(batchSize));

		float totalLoss = 0.f;
		float minLoss = std::numeric_limits<float>::max();
		int forwardRuns = 0;

		auto optimizer = torch::optim::AdamW(net->parameters(), torch::optim::AdamWOptions(lr)
			.weight_decay(wd));

		std::future<float> testScore;
		int64_t testEpoch = 0;

		std::ofstream testScoreLog(_name + "_testScore.txt");
		std::ofstream trainLossLog(_name + "_trainLoss.txt");
		std::ofstream validLossLog(_name + "_validLoss.txt");

		auto lossFn = [&](torch::Tensor output, const torch::Tensor& _target)
		{
			using namespace torch::indexing;
			constexpr int64_t reduction = useStepWeighting ? at::Reduction::None : at::Reduction::Mean;

			torch::Tensor loss;
			output = output.squeeze(-1);
			torch::Tensor target = _target;
			if constexpr (useStepWeighting)
				target = target.slice(1, 0, -1);
			target = target.squeeze(-1);

			if constexpr (alternativeMode)
			{
				loss = torch::mse_loss(torch::sigmoid(output), target, reduction);
			}
			else
			{
				if constexpr (useWinsOnly)
					loss = torch::cross_entropy_loss(output, target, {}, reduction);
				else
				{
					loss = winWeighting * torch::cross_entropy_loss(output.slice(1, 0, numOutputs), _target.index({ Slice(), 0 }), {}, reduction)
						+ (1.f - winWeighting) * torch::cross_entropy_loss(output.slice(1, numOutputs), _target.index({ Slice(), 1 }), {}, reduction);
				}
			}

			if constexpr (useStepWeighting)
			{
				const torch::Tensor weights = 1.f / torch::sqrt(_target.index({ Slice(), -1 }).to(torch::kFloat));
				loss = torch::mean(torch::mul(loss, weights));
			}

			return loss;
		};

		int numEpochsWithoutImprovement = 0;
		for (int64_t epoch = 1; epoch <= numEpochs && numEpochsWithoutImprovement < 16; ++epoch)
		{
			net->train();

			// report async test results
			if (testScore.valid() && testScore.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				const float score = testScore.get();
				testScoreLog << testEpoch << " " << score << "\n";
				spdlog::info("test score: {}, epoch: {}", score, testEpoch);
			}
			// start next test task
			if (!testScore.valid())
			{
				auto testNet = clone(net);
				testNet->to(torch::kCPU);
				testScore = std::async(std::launch::async, _testFn, testNet);
				testEpoch = epoch - 1;
			}

			for (dat::Example<>& batch : *data_loader)
			{
				auto closure = [&]()
				{
					net->zero_grad();
					torch::Tensor loss = lossFn(net->forward(batch.data), batch.target);

					totalLoss += loss.item<float>();
					++forwardRuns; // count runs to normalize the training error

					loss.backward();

					return loss;
				};

				try {
					optimizer.step(closure);
				}
				catch (const c10::Error& err)
				{
					spdlog::error("{}", err.msg());
					std::abort();
				}
			}

			net->eval();
			float totalValidLoss = 0.f;
			int validRuns = 0;
			for (dat::Example<>& batch : *valid_loader)
			{
					net->zero_grad();
					torch::Tensor output = net->forward(batch.data).squeeze(-1);
					torch::Tensor loss = lossFn(output, batch.target);

					totalValidLoss += loss.item<float>();
					++validRuns;
			}
			
			++numEpochsWithoutImprovement;
			const float loss = totalLoss / forwardRuns;
			const float validLoss = totalValidLoss / validRuns;
			if (validLoss < minLoss)
			{
				minLoss = validLoss;
				bestNet = clone(net);
				numEpochsWithoutImprovement = 0;
			}
			spdlog::info("epoch: {}, loss: {}, valid: {}", epoch, loss, validLoss);
			trainLossLog << epoch << " " << loss << std::endl;
			validLossLog << epoch << " " << validLoss << std::endl;
		}
		bestNet->to(torch::kCPU);
		torch::save(bestNet, _name + ".pt");
		
		// report final test score
		if (testScore.valid()) 
		{
			const float score = testScore.get();
			testScoreLog << testEpoch << " " << score << "\n";
			spdlog::info("test score: {}, epoch: {}", score, testEpoch);
		}
	}

	// ************************************************************************ //
	ReinforcmentLoop::ReinforcmentLoop(const std::string& _netName, const std::string& _logName)
		: m_netName(_netName),
		m_logName(_logName)
	{
		spdlog::info("Creating RL-loop loop with nets: \"{}\", logs: \"{}\".", _netName, _logName);
	}

	void ReinforcmentLoop::Run(int _numGamesPerEpoch, unsigned _numThreads)
	{
		const unsigned numThreads = _numThreads ? _numThreads : std::max(1u, std::thread::hardware_concurrency() / 2);
		
		spdlog::info("Running RL-loop with {} games per epoch and {} threads.", _numGamesPerEpoch, numThreads);

		std::vector<std::string> neuralNetworkNames;
		// start with newly initialized network
		neuralNetworkNames.push_back("");

//		auto& gameplayConf = game.Config().GetSection("gameplay");

		auto testFn = [this, numThreads](const NetType& _net)
		{
			constexpr const char* testNetName = "__testNet.pt";
			torch::save(_net, testNetName);
			std::vector<RLBot> bots;
			bots.push_back({ testNetName, DoopAI::Mode::SAMPLE_FILTERED_90 });
			bots.push_back({ "", DoopAI::Mode::RANDOM });
			Evaluate(bots, 4096, numThreads);
			g_resultsMatrix->Print();
			const int winsP0 = g_resultsMatrix->Get(0, 1, 0) + g_resultsMatrix->Get(1, 0, 1);
			const int winsP1 = g_resultsMatrix->Get(0, 1, 1) + g_resultsMatrix->Get(1, 0, 0);
			return static_cast<float>(winsP0) / (winsP0 + winsP1);
		};

		constexpr int numEpochs = 8;
		for (int i = 0; i < numEpochs; ++i)
		{
			const std::string epoch = std::to_string(i);
			const std::string path = m_logName + epoch;
			const std::string netName = m_netName + epoch;

			// create data
			if (fs::exists(path))
			{
				spdlog::info("Skipping data generation because \"{}\" already exists.", path);
			}
			else
			{
				spdlog::info("Generating data \"{}\".", path);
				auto start = std::chrono::high_resolution_clock::now();
				fs::create_directory(path);

				auto generateData = [&](unsigned threadNum, int numGames)
				{
					Shoop game(1366, 768);
					auto& section = game.Config().GetSection("learning");
					game.Config().GetSection("general").SetValue("LogGames", 1);
					game.Config().GetSection("gameplay").SetValue("numWinsRequired", numGames);
					game.Config().GetSection("gameplay").SetValue("maxNumGames", numGames);
					section.SetValue("LogPath", path + "/" + std::to_string(threadNum) + "_");
					section.SetValue("NetworkName0", neuralNetworkNames.back());
					section.SetValue("NetworkName1", neuralNetworkNames.back());
					// if a ResultsMatrix exists this value needs to be set
					section.SetValue("PairingIdx", 0);
					if (i == 0)
					{
						section.SetValue("SelectMode0", static_cast<int>(DoopAI::Mode::RANDOM));
						section.SetValue("SelectMode1", static_cast<int>(DoopAI::Mode::RANDOM));
					}
					else
					{
						section.SetValue("SelectMode0", static_cast<int>(DoopAI::Mode::SAMPLE_FILTERED_90));
						section.SetValue("SelectMode1", static_cast<int>(DoopAI::Mode::SAMPLE_FILTERED_90));
					}
					const float explore = 1.f - (i / static_cast<float>(numEpochs)) * 0.5f;
					section.SetValue("ExploreRatio", explore);
					game.Run();
				};
				
				const int gamesPerThread = _numGamesPerEpoch / static_cast<int>(numThreads);
				std::vector<std::thread> threads;
				for (unsigned t = 0; t < numThreads - 1; ++t)
					threads.emplace_back(generateData, t, gamesPerThread);
				generateData(numThreads - 1, gamesPerThread);
				for (auto& t : threads)
					t.join();

				auto end = std::chrono::high_resolution_clock::now();
				spdlog::info("Data generation took {}s.", std::chrono::duration<float>(end - start).count());
			}

			// train new network
			spdlog::info("Training network {}.", netName);
			auto start = std::chrono::high_resolution_clock::now();
			Trainer train;
			train.Train(path, netName, testFn);
			neuralNetworkNames.push_back(netName);
			auto end = std::chrono::high_resolution_clock::now();
			spdlog::info("Training took {}s.", std::chrono::duration<float>(end - start).count());
		}
	}

	void ReinforcmentLoop::Evaluate()
	{
		std::vector<RLBot> bots;
		bots.push_back({ "rand", DoopAI::Mode::RANDOM });
		
		for (int i = 0; i < 1; ++i)
			bots.push_back({ "winLossNet_" + std::to_string(i), DoopAI::Mode::SAMPLE });
		/*	bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::SAMPLE });
			bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::SAMPLE_FILTERED });
			bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::MAX });
			bots.push_back({ "net_" + std::to_string(3), DoopAI::Mode::SAMPLE });
			bots.push_back({ "net_" + std::to_string(4), DoopAI::Mode::SAMPLE });*/

		Evaluate(bots, 1024, 1/*std::thread::hardware_concurrency() / 2*/);

		g_resultsMatrix->Print();
		std::cout << "----------------------------------------------\n";
		g_resultsMatrix->Print(true);
	}

	void ReinforcmentLoop::Evaluate(const std::vector<RLBot>& _bots, int _numGames, size_t _numThreads, bool _playMirror)
	{
		g_resultsMatrix.reset(new ResultsMatrix(static_cast<int>(_bots.size()), 4));

		std::vector<std::pair<size_t, size_t>> pairings;
		for (size_t i = 0; i < _bots.size(); ++i)
			for (size_t j = 0; j < _bots.size(); ++j)
			{
				if(_playMirror || i != j)
					pairings.emplace_back(i, j);
			}

		auto evaluateGames = [&](size_t begin, size_t end) {
			Shoop game(1366, 768);

			auto& learningConf = game.Config().GetSection("learning");
			learningConf.SetValue("LogGames", 0);
			learningConf.SetValue("ExploreRatio", 0.0);

			auto& gameplayConf = game.Config().GetSection("gameplay");
			gameplayConf.SetValue("numWinsRequired", std::numeric_limits<int>::max());
			gameplayConf.SetValue("maxNumGames", _numGames);

			for (size_t k = begin; k < end; ++k)
			{
				const auto [i, j] = pairings[k];
				learningConf.SetValue("NetworkName0", _bots[i].neuralNetworkName);
				learningConf.SetValue("SelectMode0", static_cast<int>(_bots[i].mode));
				learningConf.SetValue("NetworkName1", _bots[j].neuralNetworkName);
				learningConf.SetValue("SelectMode1", static_cast<int>(_bots[j].mode));
				learningConf.SetValue("PairingIdx", g_resultsMatrix->GetPairingIdx(i, j));
				game.Run();
			}
		};

		std::vector<std::thread> threads;
		_numThreads = std::min(_numThreads, pairings.size());
		const size_t pairingsPerThread = pairings.size() / _numThreads;
		for(size_t i = 0; i < _numThreads-1; ++i)
			threads.emplace_back(evaluateGames, i * pairingsPerThread, (i+1)*pairingsPerThread);
		evaluateGames((_numThreads - 1) * pairingsPerThread, pairings.size());

		for (auto& thread : threads)
			thread.join();
	}

} // namespace Learning


	// ************************************************************************ //

namespace Game {

	using namespace Learning;

	// use global atomic seed to ensure that the RNG varies between bots
	std::atomic<int> g_rngSeed = 123456;

	DoopAI::DoopAI(const std::string& _netName, Mode _mode, float _exploreRatio)
		: m_inputs(MakeInputSpace(numIntervals)),
		m_numInputs(static_cast<int64_t>(m_inputs.size())),
		m_neuralNet(MakeNet<NetType>()),
		m_mode(_mode),
		m_exploreRatio(_exploreRatio),
		m_random(++g_rngSeed)
	{
		const std::string netName = _netName + ".pt";
		if (fs::exists(netName))
			torch::load(m_neuralNet, netName);

		if constexpr (alternativeMode)
		{
			std::vector<torch::Tensor> playerInputs;
			playerInputs.reserve(m_numInputs);
			for (const auto& inp : m_inputs)
				playerInputs.emplace_back(ToTensor(inp));
			m_inputsAsTensor = torch::stack(playerInputs, 0);
		}
	}

	void DoopAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		// explore: do a random move
		if ((m_exploreRatio > 0.f && m_random.Uniform() <= m_exploreRatio) || m_mode == Mode::RANDOM)
		{
			const int select = m_random.Uniform(0, static_cast<int32_t>(m_inputs.size()) - 1);
			_inp.state = m_inputs[select];
			return;
		}

		torch::Tensor x;
		try {
			if constexpr (alternativeMode)
			{
				x = torch::concat({ ToTensor(_self).expand({m_numInputs, -1}), 
						m_inputsAsTensor, 
						ToTensor(_oth).expand({m_numInputs, -1}) }, -1);
				x = torch::sigmoid(m_neuralNet->forward(x).squeeze(-1));
				x /= x.sum();
			}
			else
			{
				x = torch::concat({ ToTensor(_self), ToTensor(_oth) });
				x = m_neuralNet->forward(x);

				if constexpr (!useWinsOnly)
				{
					torch::Tensor predWin = torch::softmax(x.slice(0, 0, m_numInputs), 0);
					torch::Tensor predLoss = torch::softmax(x.slice(0, m_numInputs), 0);
					// difference is in [-1,1]
					x = (predWin - predLoss) * 0.5f + 0.5f;
					x *= 2.f / m_numInputs;
					//		spdlog::info("========================== {}", x.sum().item<float>());
					//		for (int64_t i = 0; i < predWin.size(0); ++i)
					//			spdlog::info("{} w:{} l:{} t:{}", i, predWin[i].item<float>(), predLoss[i].item<float>(), x[i].item<float>());
				}
				else
				{
					x = torch::softmax(x, 0);
				}
			}

			auto SelectRandom = [this](const torch::Tensor& t)
			{
				const float select = m_random.Uniform();
				float probSum = 0.f;
				for (int64_t i = 0; i < t.numel(); ++i)
				{
					probSum += t[i].item<float>();
					if (select < probSum)
					{
						return i;
					}
				}

				return t.numel() - 1;
			};

			int64_t idx = 0;
			switch (m_mode)
			{
			case Mode::MAX:
				idx = torch::argmax(x).item<int64_t>();
				break;
			case Mode::SAMPLE_FILTERED:
			{
				torch::Tensor sortedIdx = torch::argsort(x, static_cast<int64_t>(0)).slice(0, 0, x.numel() / 2);
				x.index_put_({ sortedIdx }, torch::zeros(sortedIdx.sizes(), x.options()));
				x /= x.sum();
				idx = SelectRandom(x);
				break;
			}
			case Mode::SAMPLE_FILTERED_90:
			{
				torch::Tensor sortedIdx = torch::argsort(x, static_cast<int64_t>(0)).slice(0, 0, static_cast<int64_t>(x.numel() * 0.9f));
				x.index_put_({ sortedIdx }, torch::zeros(sortedIdx.sizes(), x.options()));
				x /= x.sum();
				idx = SelectRandom(x);
				break;
			}
			case Mode::SAMPLE:
				idx = SelectRandom(x);
				break;
			default:
				break;
			}
			_inp.state = m_inputs[idx];

		}
		catch (const c10::Error& err)
		{
			std::cout << err.msg() << "\n";
			std::abort();
		}
		
	}
}