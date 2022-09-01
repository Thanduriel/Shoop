#include "playercontrollercomponent.hpp"
#include "input/actions.hpp"
#include "input/inputmanager.hpp"
#include "math/vector.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "jumpcomponent.hpp"
#include "sheep.hpp"

namespace Game {

	PlayerControllerComponent::PlayerControllerComponent(Actor& _actor, 
		Input::InputInterface& _inputInterface,
		bool _autoCharge)
		: ControllerComponent(_actor),
		m_input(_inputInterface), 
		m_autoCharge(_autoCharge)
	{}

	void PlayerControllerComponent::Process(float _deltaTime)
	{
		using namespace Input;

		Sheep& player = static_cast<Sheep&>(*m_actor);
		if (player.IsDead()) return;

		JumpComponent& jump = player.GetJump();
		if (m_autoCharge)
		{
			if (m_input.IsKeyPressed(Action::Jump) || m_input.IsKeyPressed(Action::JumpAlt))
			{
				if (jump.GetChargeLevel() > 0.25)
					jump.Jump();
			}
		}
		else 
		{
			if (m_input.IsKeyPressed(Action::Jump) || m_input.IsKeyPressed(Action::JumpAlt))
			{
				jump.SetCharging(true);
			}
			else
			{
				jump.SetCharging(false);
				if (jump.GetChargeLevel() > 0.05)
					jump.Jump();
			}
		}

		using namespace Math;
		// stronger rotation when parallel to the ground to enable standing up
		// todo: better adaptive torque
		const Vec2 up = m_actor->GetRotated(Vec2(0.f, 1.f));
		const float s = std::acos(std::abs(up.y)) * 0.4f;

		const float rot = m_input.GetAxis(Axis::Rotate);
		if(std::abs(rot) > 0.2f) player.GetBody().Get().ApplyTorque(rot * -(0.4f + s), true);

		const float acc = m_input.GetAxis(Axis::Accelerate);
		if (std::abs(acc) > 0.2f) player.GetWheel().Get().ApplyTorque(-acc, true);
	}

}