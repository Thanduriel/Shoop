#include "classic.hpp"
#include <algorithm>

namespace Game {

	Classic::Classic(ControllerContainer& _controllers, int _numWins, float _waitTime)
		: Rules(_controllers),
		m_numWinsRequired(_numWins),
		m_waitTime(_waitTime),
		m_waitTimeLeft(0.f)
	{

	}

	void Classic::Process(float _deltaTime)
	{
		switch (m_state)
		{
		case State::Running:
			if (std::any_of(m_players.begin(), m_players.end(), Rules::IsDead))
			{
				// register win
				for (size_t i = 0; i < m_players.size(); ++i)
				{
					const Actor::Handle& player = m_players[i];
					if (!Rules::IsDead(player))
						++m_numWins[i];
				}

				m_state = State::Wait;
				m_waitTimeLeft = m_waitTime;
			}
			break;
		case State::Wait:
			m_waitTimeLeft -= _deltaTime;
			if (m_waitTimeLeft < 0.f)
			{
				ResetMap();
				m_state = State::Running;
			}
			break;
		}
	}

	bool Classic::IsOver()
	{
		for (size_t i = 0; i < m_numWins.size(); ++i)
		{
			if (m_numWins[i] >= m_numWinsRequired)
				return true;
		}

		return false;
	}

	void Classic::Start()
	{
		m_players.resize(2);
		m_numWins.resize(2);
		for (PlayerControllerComponent* controller : m_controllers)
			controller->GetActor().Destroy();
		ResetMap();
	}

	std::string Classic::GetScore()
	{
		return std::to_string(m_numWins[0]) + " : " + std::to_string(m_numWins[1]);
	}

	void Classic::ResetMap()
	{
		for (size_t i = 0; i < m_players.size(); ++i)
		{
			if (!IsDead(m_players[i])) m_players[i]->Destroy();
			SpawnPlayer(static_cast<int>(i), m_map->GetSpawnPositions()[i], i%2);
		}
	}
}