#include "learning.hpp"
#include "game.hpp"
#include "evaluation.hpp"
#include <filesystem>
#include <thread>
#include <future>

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
	Dataset::Dataset(const std::string& _path, int _numIntervals, torch::Device _device)
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
		m_inputs = torch::stack(inputsTemp).to(_device);
		//	auto [stddev, mean] =  torch::std_mean(m_inputs, 0);
		//	std::cout << mean << " stddev: \n" << stddev << "\n";
		static const c10::TensorOptions options(c10::kInt);
		m_outputs = torch::from_blob(outputsTemp.data(), { static_cast<int64_t>(outputsTemp.size()) }, options)
			.to(_device, c10::kLong, false, true);
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
	constexpr int64_t hiddenSize = 48;
	constexpr int64_t numLayers = 4;
	constexpr int64_t batchSize = 1024;
	constexpr int64_t numEpochs = 512;
	constexpr bool useCrossEntropy = true;

	void Trainer::Train(const std::string& _path, const std::string& _name, CustomTestFn _testFn)
	{
		namespace dat = torch::data;

		const double lr = 0.01;
		const double wd = 1e-5;
		const int64_t numOutputs = MakeInputSpace(numIntervals).size();
		MLP net(MLPOptions(GetSheepStateSize() * 2, hiddenSize, numOutputs).layers(numLayers));
		MLP bestNet = clone(net);

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
		auto data_loader = dat::make_data_loader<Sampler>(
			Dataset(_path, numIntervals, device).map(dat::transforms::Stack<>()),
			dat::DataLoaderOptions().batch_size(batchSize));

		float totalLoss = 0.0;
		float minLoss = std::numeric_limits<float>::max();
		int forwardRuns = 0;

		auto optimizer = torch::optim::AdamW(net->parameters(), torch::optim::AdamWOptions(lr)
			.weight_decay(wd));

		std::future<float> testScore;
		int64_t testEpoch = 0;

		for (int64_t epoch = 1; epoch <= numEpochs; ++epoch)
		{
			net->train();

			// report async test results
			if (testScore.valid() && testScore.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				spdlog::info("test score: {}, epoch: {}", testScore.get(), testEpoch);
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
					torch::Tensor output = net->forward(batch.data);
					torch::Tensor loss;
					if constexpr (useCrossEntropy)
						loss = torch::cross_entropy_loss(output, batch.target);
					else 
						loss = LabelMSE(output, batch.target, numOutputs, 1.f, 15.f);
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
					spdlog::error("{}", err.msg());
					std::abort();
				}
			}
			
			const float loss = totalLoss / forwardRuns;
			if (minLoss < loss)
			{
				minLoss = loss;
				bestNet = clone(net);
			}
			spdlog::info("epoch: {}, loss: {}", epoch, loss);
		}
		bestNet->to(torch::kCPU);
		torch::save(bestNet, _name + ".pt");

	}

	// ************************************************************************ //
	ReinforcmentLoop::ReinforcmentLoop(const std::string& _netName, const std::string _logName)
		: m_netName(_netName),
		m_logName(_logName)
	{
	}

	void ReinforcmentLoop::Run(int _numGamesPerEpoch)
	{
		std::vector<std::string> neuralNetworkNames;
		// start with newly initialized network
		neuralNetworkNames.push_back("");

//		auto& gameplayConf = game.Config().GetSection("gameplay");

		auto testFn = [this](const MLP& _net)
		{
			constexpr const char* testNetName = "__testNet.pt";
			torch::save(_net, testNetName);
			std::vector<RLBot> bots;
			bots.push_back({ testNetName, DoopAI::Mode::SAMPLE_FILTERED });
			bots.push_back({ "", DoopAI::Mode::RANDOM });
			Evaluate(bots, 2048, std::thread::hardware_concurrency() / 2, false);
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

				auto generateData = [&](size_t threadNum, int numGames)
				{
					Shoop game(1366, 768);
					auto& section = game.Config().GetSection("learning");
					game.Config().GetSection("general").SetValue("LogGames", 1);
					game.Config().GetSection("gameplay").SetValue("numWinsRequired", numGames);
					game.Config().GetSection("gameplay").SetValue("maxNumGames", numGames);
					section.SetValue("LogPath", path + "/" + std::to_string(threadNum) + "_");
					section.SetValue("NetworkName0", neuralNetworkNames.back());
					section.SetValue("NetworkName1", neuralNetworkNames.back());
					if (i == 0)
					{
						section.SetValue("SelectMode0", static_cast<int>(DoopAI::Mode::RANDOM));
						section.SetValue("SelectMode1", static_cast<int>(DoopAI::Mode::RANDOM));
					}
					else
					{
						section.SetValue("SelectMode0", static_cast<int>(DoopAI::Mode::SAMPLE_FILTERED));
						section.SetValue("SelectMode1", static_cast<int>(DoopAI::Mode::SAMPLE_FILTERED));
					}
					const float explore = 1.f - (i / static_cast<float>(numEpochs)) * 0.5f;
					section.SetValue("ExploreRatio", explore);
					game.Run();
				};

				const size_t numThreads = std::max(1u, std::thread::hardware_concurrency() / 2);
				const int gamesPerThread = _numGamesPerEpoch / static_cast<int>(numThreads);
				std::vector<std::thread> threads;
				for (size_t t = 0; t < numThreads - 1; ++t)
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

		for (int i = 0; i < 8; ++i)
			bots.push_back({ "muchDataNet_" + std::to_string(i), DoopAI::Mode::SAMPLE });
		/*	bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::SAMPLE });
			bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::SAMPLE_FILTERED });
			bots.push_back({ "net_" + std::to_string(6), DoopAI::Mode::MAX });
			bots.push_back({ "net_" + std::to_string(3), DoopAI::Mode::SAMPLE });
			bots.push_back({ "net_" + std::to_string(4), DoopAI::Mode::SAMPLE });*/

		Evaluate(bots, 1024, std::thread::hardware_concurrency() / 2);

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
			//	std::cout << fmt::format("k {} i {} j {}\n", k, i, j);
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

	std::atomic<int> g_rngSeed = 123456;

	DoopAI::DoopAI(const std::string& _netName, Mode _mode, float _exploreRatio)
		: m_inputs(MakeInputSpace(numIntervals)),
		m_neuralNet(MLPOptions(GetSheepStateSize() * 2, hiddenSize, static_cast<int64_t>(m_inputs.size()))
			.layers(numLayers)),
		m_mode(_mode),
		m_exploreRatio(_exploreRatio),
		m_random(++g_rngSeed)
	{
		const std::string netName = _netName + ".pt";
		if (fs::exists(netName))
			torch::load(m_neuralNet, netName);
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
			x = torch::concat({ ToTensor(_self), ToTensor(_oth) });
			x = m_neuralNet->forward(x);

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
				x = torch::softmax(x, 0);
				torch::Tensor sortedIdx = torch::argsort(x, 0).slice(0, 0, x.numel() / 2);
				x.index_put_({ sortedIdx }, torch::zeros(sortedIdx.sizes(), x.options()));
				x /= x.sum();
				idx = SelectRandom(x);
				break;
			}
			case Mode::SAMPLE:
				x = torch::softmax(x, 0);
				idx = SelectRandom(x);
				break;
			default:
				break;
			}
			_inp.state = m_inputs[idx];

		}
		catch (c10::Error err)
		{
			std::cout << err.msg() << "\n";
			std::abort();
		}
		
	}
}