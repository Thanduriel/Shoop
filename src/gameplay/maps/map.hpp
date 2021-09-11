#pragma once

#include "math/vector.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factory.hpp"
#include <vector>

namespace Game {

	class Scene;

	class Map : public Actor
	{
	public:
		Map(Scene& _scene);
		
		const std::vector<Math::Vec2>& GetSpawnPositions() const { return m_spawnPositions; }

		FactoryComponent& GetFactory() { return m_factory; }

		// -1 gray, 0 red, 1 green
		void ShowWinnerFlags(int _color, float _duration);
	protected:
		enum Border {
			Left = 0x1,
			Right = 0x2,
			Top = 0x4,
			Bot = 0x8
		};
		using BorderFlags = unsigned;

		void MakeBorders(Scene& _scene, BorderFlags _borders = Left | Right | Top | Bot);

		std::vector<Math::Vec2> m_spawnPositions;
		Math::Vec2 m_winnerFlagPos;

		FactoryComponent m_factory;
	};
}