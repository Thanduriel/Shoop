#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/spriteomponent.hpp"
#include "playercontrollercomponent.hpp"

namespace Input {
	class InputInterface;
}

namespace Game {

	class RemoteControlComponent : public ProcessComponent
	{
	public:
		RemoteControlComponent(Actor& _actor, sf::Color _color, PlayerControllerComponent& _inputInterfac);
		void Process(float _deltaTime);

	private:
		float m_totalTime;
		sf::Color m_blinkColor;
		PlayerControllerComponent& m_playerController;
	};

	class RemoteControl : public Actor
	{
	public:
		RemoteControl(Math::Vec2 _position, 
			PlayerControllerComponent& _playerController,
			bool _mirrored = false,
			const sf::Color& _color = sf::Color(0xffffffff));

	private:
		SpriteComponent m_baseSprite;
		SpriteComponent m_flagSprite;
		SpriteComponent m_flagColorablePartSprite;
		SpriteComponent m_leftThumbSprite;
		SpriteComponent m_rightThumbSprite;
		SpriteComponent m_leftFingerSprite;
		SpriteComponent m_rightFingerSprite;
		SpriteComponent m_jumpbarSprite;
		RemoteControlComponent m_controlComponent;

		bool m_mirrored;

		friend class RemoteControlComponent;
	};
}