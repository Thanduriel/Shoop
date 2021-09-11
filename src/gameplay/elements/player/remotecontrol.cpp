#include "remotecontrol.hpp"
#include "resources.hpp"
#include "input/input.hpp"
#include "sheep.hpp"
#include <iostream>
namespace Game {

	const std::array<float, 4> HORIZONTAL_THRESHOLD = { -0.8f, -0.2f, 0.2f, 0.8f };
	const std::array < std::string, 5> HORIZONTAL_THUMB_LEFT_SPRITES =
	{ {
		{"leftThumb_leftmost"},
		{"leftThumb_left"},
		{"leftThumb_mid"},
		{"leftThumb_right"},
		{"leftThumb_rightmost"}
	} };
	const std::array < std::string, 5> HORIZONTAL_THUMB_RIGHT_SPRITES =
	{ {
		{"rightThumb_leftmost"},
		{"rightThumb_left"},
		{"rightThumb_mid"},
		{"rightThumb_right"},
		{"rightThumb_rightmost"}
	} };

	const std::array < std::string, 2> FINGER_LEFT_SPRITES =
	{ {
		{"leftFinger_up"},
		{"leftFinger_down"}
	} };
	const std::array < std::string, 5> FINGER_RIGHT_SPRITES =
	{ {
		{"rightFinger_up"},
		{"rightFinger_down"}
	} };

	const std::array<float, 5> JUMP_THRESHOLD = { 0.05f, 0.15f, 0.3f, 0.5f, 0.8f };

	const std::array < std::string, 6> JUMP_BAR_SPRITES =
	{ {
		{"Jumpbar1"},
		{"Jumpbar2"},
		{"Jumpbar3"},
		{"Jumpbar4"},
		{"Jumpbar5"},
		{"Jumpbar6"}
	} };

	RemoteControlComponent::RemoteControlComponent(Actor& _actor, 
		sf::Color _color,
		PlayerControllerComponent& _playerController)
		: ProcessComponent(_actor),
		m_totalTime(0.f),
		m_blinkColor(_color),
		m_playerController(_playerController)
	{}

	void RemoteControlComponent::Process(float _deltaTime)
	{
		// blinking
		m_totalTime += _deltaTime;
		const float blinkTime = 3.f;
		m_totalTime = std::fmod(m_totalTime, blinkTime);

		const sf::Color fromColor(255, 255, 255);
		const sf::Color toColor(m_blinkColor);

		// Set Finger/Thumb-Color
		float t;
		if (m_totalTime < blinkTime * 0.5F)
		{
			t = m_totalTime / (blinkTime * 0.5F);
		}
		else
		{
			t = 1.f - (m_totalTime / (blinkTime * 0.5F) - 1.F);
		}
		t *= 0.5f;
		const sf::Color color(static_cast<sf::Uint8>((1-t) * fromColor.r + t * toColor.r),
			static_cast<sf::Uint8>((1 - t) * fromColor.g + t * toColor.g),
			static_cast<sf::Uint8>((1 - t) * fromColor.b + t * toColor.b));

		RemoteControl& act = static_cast<RemoteControl&>(GetActor());
		act.m_leftFingerSprite.GetSprite().setColor(color);
		act.m_rightFingerSprite.GetSprite().setColor(color);
		act.m_leftThumbSprite.GetSprite().setColor(color);
		act.m_rightThumbSprite.GetSprite().setColor(color);

		// set thumb sprites
		using namespace Input;
		InputInterface& inputs = m_playerController.GetInputs();
		const float rightStick = inputs.GetAxis(Axis::Rotate);
		auto it = std::upper_bound(HORIZONTAL_THRESHOLD.begin(), HORIZONTAL_THRESHOLD.end(), rightStick);
		size_t ind = std::distance(HORIZONTAL_THRESHOLD.begin(), it);
		if(act.m_mirrored)
			act.m_leftThumbSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + HORIZONTAL_THUMB_LEFT_SPRITES[4-ind]));
		else
			act.m_rightThumbSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + HORIZONTAL_THUMB_RIGHT_SPRITES[ind]));
	
		const float leftStick = inputs.GetAxis(Axis::Accelerate);
		it = std::upper_bound(HORIZONTAL_THRESHOLD.begin(), HORIZONTAL_THRESHOLD.end(), leftStick);
		ind = std::distance(HORIZONTAL_THRESHOLD.begin(), it);
		if (act.m_mirrored)
			act.m_rightThumbSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + HORIZONTAL_THUMB_RIGHT_SPRITES[4-ind]));
		else
			act.m_leftThumbSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + HORIZONTAL_THUMB_LEFT_SPRITES[ind]));

		// set finger sprites
		if (inputs.IsKeyPressed(Action::Jump) || inputs.IsKeyPressed(Action::JumpAlt))
		{
			act.m_rightFingerSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + FINGER_RIGHT_SPRITES[1]));
			act.m_leftFingerSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + FINGER_LEFT_SPRITES[1]));
		}
		else
		{
			act.m_rightFingerSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + FINGER_RIGHT_SPRITES[0]));
			act.m_leftFingerSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + FINGER_LEFT_SPRITES[0]));
		}

		JumpComponent& jump = static_cast<Sheep&>(m_playerController.GetActor()).GetJump();
		auto itt = std::upper_bound(JUMP_THRESHOLD.begin(), JUMP_THRESHOLD.end(), jump.GetChargeLevel());
		ind = std::distance(JUMP_THRESHOLD.begin(), itt);
		act.m_jumpbarSprite.GetSprite().setTexture(Resources::Load<sf::Texture>("Hands/" + JUMP_BAR_SPRITES[ind]));
	}

	const float size = 6.f;
	const Math::Vec2 origin = Math::Vec2(0.5f, 1.f);

	RemoteControl::RemoteControl(Math::Vec2 _position,
		PlayerControllerComponent& _playerController,
		bool _mirrored,
		const sf::Color& _color)
		: Actor(_position),
		m_baseSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/Base"), size, origin, DrawComponent::DrawingOrder::Front),
		m_flagSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/Flag"), size, origin, DrawComponent::DrawingOrder::Front),
		m_flagColorablePartSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/FlagWhitePart"), size, origin, DrawComponent::DrawingOrder::Front),
		m_leftThumbSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/leftThumb_left"), size, origin, DrawComponent::DrawingOrder::Front),
		m_rightThumbSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/rightThumb_left"), size, origin, DrawComponent::DrawingOrder::Front),
		m_leftFingerSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/leftFinger_up"), size, origin, DrawComponent::DrawingOrder::Front),
		m_rightFingerSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/rightFinger_up"), size, origin, DrawComponent::DrawingOrder::Front),
		m_jumpbarSprite(THISACTOR, Resources::Load<sf::Texture>("Hands/Jumpbar1"), size, origin, DrawComponent::DrawingOrder::Front),
		m_controlComponent(THISACTOR, _color, _playerController),
		m_mirrored(_mirrored)
	{

		m_flagColorablePartSprite.GetSprite().setColor(_color);
		if (m_mirrored)
		{
			m_baseSprite.FlipX(true);
			m_flagSprite.FlipX(true);
			m_flagColorablePartSprite.FlipX(true);
			m_leftThumbSprite.FlipX(true);
			m_rightThumbSprite.FlipX(true);
			m_leftFingerSprite.FlipX(true);
			m_rightFingerSprite.FlipX(true);
			m_jumpbarSprite.FlipX(true);
		}
	}
}