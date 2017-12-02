#pragma once

#include "gamestate.hpp"
#include "gameplay/scene.hpp"

namespace Game {

	class GSPlay : public GameState
	{
	public:
		GSPlay();

		void Process(float _deltaTime) override;
		void Draw(sf::RenderWindow& _window) override;

	private:
		Scene m_scene;
	};
}