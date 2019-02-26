#pragma once
#include "gameplay/elements/player/playercontrollercomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/maps/map.hpp"
#include <vector>

namespace Game {

	class Rules
	{
	public:
		Rules(ControllerContainer& _controllers);
		virtual ~Rules() {}

		void SetMap(Map& _map) { m_map = &_map; }

		virtual void Process(float _deltaTime) {}
		virtual bool IsOver() = 0;
		virtual void Start() {}
		virtual std::string GetScore() { return "not implemented"; };

	protected:
		void SpawnPlayer(PlayerId _id, Math::Vec2 _position);
		static bool IsDead(const Actor::ConstHandle& _handle);

		std::vector<Actor::Handle> m_players;
		ControllerContainer& m_controllers;
		Map* m_map;
		
	};

}