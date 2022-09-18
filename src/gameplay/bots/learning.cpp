#include "learning.hpp"

namespace Game {

	std::vector<Input::InputState> makeInputSpace(int _numIntervals)
	{
		constexpr size_t numAxis = static_cast<size_t>(Input::Axis::COUNT);
		constexpr size_t numActions = static_cast<size_t>(Input::Action::COUNT);

		size_t total = 1;
		for (size_t i = 0; i < numAxis; ++i)
			total *= _numIntervals;
		// actions are binary -> double the number
		total <<= numActions;

		std::vector<Input::InputState> inputSpace(1);
		inputSpace.reserve(total);

		std::vector<float> linAxisSpace(_numIntervals);
		for (int i = 0; i < _numIntervals; ++i)
		{
			linAxisSpace[i] = -1.f + 2.f * (i / static_cast<float>(_numIntervals - 1));
		}

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

		for (size_t ac = 0; ac < numActions; ++ac)
		{
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

	// Constructs a view based on the SheepState.
	torch::Tensor toTensor(const SheepState& _state)
	{
		constexpr int64_t NUM_VALUES = 8;
		static_assert(sizeof(SheepState) == sizeof(float) * NUM_VALUES);
		static const c10::TensorOptions options(c10::kFloat);

		return torch::from_blob(const_cast<SheepState*>(&_state), { NUM_VALUES }, options);
	}

	DoopAI::DoopAI(int _axisIntervals)
		: m_inputs(makeInputSpace(_axisIntervals))
	{}

	void DoopAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		torch::Tensor x = torch::concat({ toTensor(_self), toTensor(_oth) });
		x = m_neuralNet.forward(x);
		const int64_t idx = torch::argmax(x).item<int64_t>();
		_inp.state = m_inputs[idx];
		
	}
}