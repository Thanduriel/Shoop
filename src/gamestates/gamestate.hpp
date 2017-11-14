#pragma once

#include <memory>

namespace Game
{
	class GameState
	{
	public:

		virtual ~GameState() {}
		virtual void Process(float _deltaTime) = 0;
		virtual void Draw() = 0;

		// called when the state is on top of the stack after state changes
		virtual void OnBegin() {}

		bool IsFinished() const { return m_isFinished; }
		void Finish() { m_isFinished = true; }
		std::unique_ptr<GameState> GetNewState() { return std::move(m_newState); }
	private:
		bool m_isFinished;
		std::unique_ptr<GameState> m_newState;

		friend class Shoop; // only the main loop needs to access state change requests
	};
}