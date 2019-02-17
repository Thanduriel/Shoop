#include "playercontrollercomponent.hpp"
#include "input/actions.hpp"
#include "input/inputmanager.hpp"
#include "math/vector.hpp"
#include "gameplay/elements/physicscomponent.hpp"
#include "jumpcomponent.hpp"
#include "sheep.hpp"

namespace Game {

	PlayerControllerComponent::PlayerControllerComponent(Actor& _actor, Input::InputInterface& _inputInterface)
		: ControllerComponent(_actor),
		m_input(_inputInterface)
	{}

	void PlayerControllerComponent::Process(float _deltaTime)
	{
		using namespace Input;

		Sheep& player = static_cast<Sheep&>(*m_actor);

		if (m_input.IsKeyPressed(Action::Jump))
			player.GetJump().Jump();

		using namespace Math;
		const Vec2 up = m_actor->GetRotated(Vec2(0.f, 1.f));
		const float s = std::acos(std::abs(up.y)) * 1.5f;
		if (m_input.IsKeyPressed(Action::RotateCW))
			player.GetBody().Get().ApplyTorque(s, true);
		if (m_input.IsKeyPressed(Action::RotateCCW))
			player.GetBody().Get().ApplyTorque(-s, true);

		if (m_input.IsKeyPressed(Action::AccelerateCCW))
			player.GetWheel().Get().ApplyTorque(1.f, true);
		if (m_input.IsKeyPressed(Action::AccelerateCW))
			player.GetWheel().Get().ApplyTorque(-1.f, true);;


	}

}