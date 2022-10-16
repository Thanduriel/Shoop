#pragma once
#include "gamerules.hpp"
#include <limits>

namespace Game {

	class Classic : public Rules
	{
	public:
		Classic(ControllerContainer& _controllers, int _numWins, float _waitTime = 2.f,
			float _timeOut = std::numeric_limits<float>::max(),
			int _maxNumGames = std::numeric_limits<int>::max());

		void Process(float _deltaTime) override;
		bool IsOver() override;
		void Start() override;
		void Reset() override;
		std::string GetScore() override;

		const std::vector<int> GetResults() const { return m_numWins; }
	private:
		void ResetMap();

		int m_numWinsRequired;
		int m_maxNumGames;
		std::vector<int> m_numWins;

		enum struct State {
			Wait,
			Running
		};
		State m_state = State::Running;
		float m_waitTime;
		float m_waitTimeLeft;
		float m_timeOut;
	};
}