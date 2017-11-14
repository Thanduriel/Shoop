#pragma once

#include "gamestate.hpp"

namespace Game {

	class GSPlay : public GameState
	{
	public:
		void Process(float _deltaTime) override;
		void Draw() override;
	};
}