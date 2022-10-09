#include "aicontrollercomponent.hpp"
#include "../../../generators/random.hpp"
#include <fstream>
#include <ctime>
#include <chrono>
#include <iostream>

namespace Game {
	SheepState::SheepState(const Sheep& _sheep)
		: position(_sheep.GetBody().Get().GetWorldCenter()),
		velocity(_sheep.GetBody().Get().GetLinearVelocity()),
		rotation(_sheep.GetBody().Get().GetAngle()),
		angularVelocity(_sheep.GetBody().Get().GetAngularVelocity()),
		wheelAngularVelocity(_sheep.GetWheel().Get().GetAngularVelocity()),
		jumpCharge(_sheep.GetJump().GetChargeLevel())
	{
	}

	void GameLog::Save(const std::string& _path)
	{
		std::ofstream file(_path, std::ios::binary);

		file.write(reinterpret_cast<char*>(&outcome), sizeof(Outcome));

		const uint64_t numStates = states.size();
		file.write(reinterpret_cast<const char*>(&numStates), sizeof(uint64_t));

		const uint64_t stateSize = sizeof(State);
		file.write(reinterpret_cast<const char*>(&stateSize), sizeof(uint64_t));

		for (const State& state : states)
		{
			file.write(reinterpret_cast<char*>(states.data()), sizeof(State));
		}
	}

	void GameLog::Load(const std::string& _path)
	{
		std::ifstream file(_path, std::ios::binary);
		file.read(reinterpret_cast<char*>(&outcome), sizeof(Outcome));
		uint64_t numStates = 0;
		file.read(reinterpret_cast<char*>(&numStates), sizeof(uint64_t));

		uint64_t stateSize = 0;
		file.read(reinterpret_cast<char*>(&stateSize), sizeof(uint64_t));
		if (sizeof(State) != stateSize)
		{
			std::cerr << "[Error] State size does not match in game log.\"" << _path << "\"\n";
			std::abort();
		}
		states.resize(numStates);
		for (State& state : states)
			file.read(reinterpret_cast<char*>(&state), sizeof(State));
	}

	AIControllerComponent::AIControllerComponent(Sheep& _self, const Rules& _oth, std::function<AIStep> _ai, float _tickRate, bool _logGame)
		: PlayerControllerComponent(_self, m_inputs),
		m_rules(_oth),
		m_ai(std::move(_ai)),
		m_tickTime(_tickRate > 0.f ? 1.f / _tickRate : 0.f),
		m_timePassed(0.f),
		m_logGame(_logGame),
		m_totalGameCount(0)
	{}

	void AIControllerComponent::Process(float _deltaTime)
	{
		m_timePassed += _deltaTime;
		if (m_timePassed >= m_tickTime) 
		{
			m_timePassed = 0.f;
			UpdatePlayerList();

			if (!m_otherPlayers.empty())
			{
				const SheepState self(static_cast<Sheep&>(*m_actor));
				const SheepState oth(*m_otherPlayers[0]);
				m_ai(self, oth, m_inputs);

				if (m_logGame)
				{
					m_log.states.push_back({self, oth, m_inputs.state});
				}
			}

		}
		PlayerControllerComponent::Process(_deltaTime);
	}

	void AIControllerComponent::Reset(Outcome _outcome)
	{
		if (!m_logGame)
			return;

		m_log.outcome = _outcome;
		const std::string outcomeStr = std::to_string(static_cast<int>(_outcome));
		m_log.Save("gamelogs/" + std::to_string(m_totalGameCount) + "_" + outcomeStr + ".dat");

		++m_totalGameCount;
		m_log.states.clear();
	}

	void AIControllerComponent::UpdatePlayerList()
	{
		m_otherPlayers.clear();
		for (Actor::ConstHandle act : m_rules.GetPlayers()) 
		{
			if (act && &*act != m_actor)
				m_otherPlayers.push_back(&static_cast<const Sheep&>(*act)); 
		}
	}

	using namespace Input;
	void RandomAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		for(float& axis : _inp.state.axis)
			axis = Generators::g_random.Uniform(-1.f,1.f);
		for (bool& action : _inp.state.actions)
			action = Generators::g_random.Uniform(0, 1);
	}
}