#include "classic.hpp"
#include <algorithm>

namespace Game {

	Classic::Classic(ControllerContainer& _controllers, int _numWins, float _waitTime, 
		float _timeOut, int _maxNumGames)
		: Rules(_controllers),
		m_numWinsRequired(_numWins),
		m_maxNumGames(_maxNumGames),
		m_waitTime(_waitTime),
		m_waitTimeLeft(_timeOut),
		m_timeOut(_timeOut)
	{

	}

	void Classic::Process(float _deltaTime)
	{
		m_waitTimeLeft -= _deltaTime;

		switch (m_state)
		{
		case State::Running:
			if (std::any_of(m_players.begin(), m_players.end(), Rules::IsDead)
				|| m_waitTimeLeft <= 0.f)
			{
				++m_numWins.back();
				// register win
				int winner = -1;
				for (size_t i = 0; i < m_players.size(); ++i)
				{
					const Actor::Handle& player = m_players[i];
					if (!Rules::IsDead(player)) 
					{
						++m_numWins[i];
						winner = static_cast<int>(i);
					}
				}

				m_state = State::Wait;
				m_waitTimeLeft = m_waitTime;

				m_map->ShowWinnerFlags(winner, m_waitTime);
				for (int i = 0; i < static_cast<int>(m_players.size()); ++i)
					m_controllers[i]->Reset(winner == i ? Outcome::Win : (winner < 0 ? Outcome::Draw : Outcome::Loss));
			}
			break;
		case State::Wait:
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
		for (size_t i = 0; i < m_numWins.size()-1; ++i)
		{
			if (m_numWins[i] >= m_numWinsRequired)
				return true;
		}

		return m_numWins.back() >= m_maxNumGames;
	}

	void Classic::Start()
	{
		m_players.resize(m_controllers.size());
		m_numWins.resize(m_controllers.size() + 1, 0);
		for (ControllerComponent* controller : m_controllers)
			controller->GetActor().Destroy();
		ResetMap();
	}

	void Classic::Reset()
	{
		for (int& i : m_numWins)
			i = 0;
		ResetMap();
	}

	std::string Classic::GetScore()
	{
		return std::to_string(m_numWins[0]) + " : " + std::to_string(m_numWins[1]);
	}

	std::vector<int> Classic::GetResults() const
	{
		auto res = m_numWins; 
		res[2] = res[2] - res[1] - res[0]; 
		return res;
	}

	void Classic::ResetMap()
	{
		m_waitTimeLeft = m_timeOut;
		for (size_t i = 0; i < m_players.size(); ++i)
		{
			if (m_players[i]) m_players[i]->Destroy();
			SpawnPlayer(static_cast<int>(i), m_map->GetSpawnPositions()[i], i%2);
		}
	}
}