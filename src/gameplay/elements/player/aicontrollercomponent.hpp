#pragma once

#include "playercontrollercomponent.hpp"
#include "sheep.hpp"
#include "../../rules/gamerules.hpp"
#include "../../../input/inputmanager.hpp"
#include <functional>

namespace Game {

	// All relevant state information
	struct SheepState 
	{
		explicit SheepState(const Sheep& _sheep);

		Math::Vec2 position;
		Math::Vec2 velocity;
		float rotation;
		float angularVelocity;
		float wheelAngularVelocity;
		float jumpCharge;
	};

	using AIStep = void(const SheepState&, const SheepState&, Input::VirtualInputs&);

	class AIControllerComponent : public PlayerControllerComponent
	{
	public:
		// @param _tickRate Frequency at which the AI step function should be performed [ticks per s]
		AIControllerComponent(Sheep& _self, const Rules& _rules, 
			std::function<AIStep> _ai, float _tickRate = 0.f);

		void Process(float _deltaTime) override;
	private:

		void UpdatePlayerList();

		std::vector<const Sheep*> m_otherPlayers;
		const Rules& m_rules;
		Input::VirtualInputs m_inputs;
		std::function<AIStep> m_ai;
		float m_tickTime;
		float m_timePassed;
	};

	// Performs uniform random inputs.
	class RandomAI
	{
	public:
		void operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp);
	};
}