#pragma once
#include "gamerules.hpp"

namespace Game {

	class Classic : public Rules
	{
	public:
		Classic(ControllerContainer& _controllers, int _numWins);

		void Process(float _deltaTime) override;
		bool IsOver() override;
		void Start() override;
		std::string GetScore() override;
	private:
		void ResetMap();

		int m_numWinsRequired;
		std::vector<int> m_numWins;
	};
}