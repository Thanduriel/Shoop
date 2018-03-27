#include "controllercomponent.hpp"
#include "jumpcomponent.hpp"

#include "gameplay/elements/physicscomponent.hpp"
//todo: use keyboardmanager instead
#include "SFML/Window.hpp"

namespace Game {

	ControllerComponent::ControllerComponent(Actor& _actor, JumpComponent& _jump, 
		PhysicsBodyComponent& _wheelBody)
		: ProcessComponent(_actor),
		m_jump(_jump),
		m_wheelBody(_wheelBody)
	{

	}

	void ControllerComponent::Process(float _deltaTime)
	{
		using Key = sf::Keyboard::Key;
		if (sf::Keyboard::isKeyPressed(Key::Space))
			m_jump.Jump();

		if (sf::Keyboard::isKeyPressed(Key::V))
			m_wheelBody.Get().ApplyTorque(1.f, true);
		if (sf::Keyboard::isKeyPressed(Key::I))
			m_wheelBody.Get().ApplyTorque(-1.f, true);;
	}
}