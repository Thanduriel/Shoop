#pragma once
#include "gamerules.hpp"

namespace Game {

	class Classic : public Rules
	{
	public:
		Classic(ControllerContainer& _controllers, int _numWins, float _waitTime = 2.f);

		void Process(float _deltaTime) override;
		bool IsOver() override;
		void Start() override;
		void Reset() override;
		std::string GetScore() override;
	private:
		void ResetMap();

		int m_numWinsRequired;
		std::vector<int> m_numWins;

		enum struct State {
			Wait,
			Running
		};
		State m_state = State::Running;
		float m_waitTime;
		float m_waitTimeLeft;
	};
}