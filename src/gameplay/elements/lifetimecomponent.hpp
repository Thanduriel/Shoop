#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	class LifetimeComponent : ProcessComponent
	{
	public:
		LifetimeComponent(Actor& _actor, float _maxTime);

		void Process(float _deltaTime) override;

		float GetTimeLeft() const { return m_timeLeft; }
		float GetTimeMax() const { return m_timeMax; }
	private:
		const float m_timeMax;
		float m_timeLeft;
	};
}