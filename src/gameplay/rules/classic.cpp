#include "classic.hpp"
#include <algorithm>

namespace Game {

	Classic::Classic(ControllerContainer& _controllers, int _numWins)
		: Rules(_controllers),
		m_numWinsRequired(_numWins)
	{

	}

	void Classic::Process(float _deltaTime)
	{
		if (std::any_of(m_players.begin(), m_players.end(), Rules::IsDead))
		{
			// register win
			for (size_t i = 0; i < m_players.size(); ++i)
			{
				const Actor::Handle& player = m_players[i];
				if (!Rules::IsDead(player))
					++m_numWins[i];
			}

			ResetMap();
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
		for (PlayerControllerComponent* controller : m_controllers)
			controller->GetActor().Destroy();
		ResetMap();
	}

	void Classic::ResetMap()
	{
		for (size_t i = 0; i < m_players.size(); ++i)
		{
			if (!IsDead(m_players[i])) m_players[i]->Destroy();
			SpawnPlayer(static_cast<int>(i), m_map->GetSpawnPositions()[i]);
		}
	}
}