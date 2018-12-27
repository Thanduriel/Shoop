#include "playercontrollercomponent.hpp"
#include "input/actions.hpp"
#include "input/inputmanager.hpp"
#include "math/vector.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "jumpcomponent.hpp"

namespace Game {

	PlayerControllerComponent::PlayerControllerComponent(Actor& _actor, Input::InputInterface& _inputInterface)
		: ControllerComponent(_actor),
		m_input(_inputInterface)
	{}

	void PlayerControllerComponent::Process(float _deltaTime)
	{
		using namespace Input;
		if (m_input.IsKeyPressed(Action::Jump))
			m_jump.Jump();

		using namespace Math;
		Vec2 up = m_actor.GetRotated(Vec2(0.f, 1.f));
		const float s = std::acos(std::abs(up.y)) * 1.5f;
		if (m_input.IsKeyPressed(Action::RotateCW))
			m_headBody.Get().ApplyTorque(s, true);
		if (m_input.IsKeyPressed(Action::RotateCCW))
			m_headBody.Get().ApplyTorque(-s, true);

		if (m_input.IsKeyPressed(Action::AccelerateCCW))
			m_wheelBody.Get().ApplyTorque(1.f, true);
		if (m_input.IsKeyPressed(Action::AccelerateCW))
			m_wheelBody.Get().ApplyTorque(-1.f, true);;


	}

}