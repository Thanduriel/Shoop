#include "gsplay.hpp"
#include "utils/config.hpp"
#include "gameplay/rules/classic.hpp"
#include "gameplay/maps/map.hpp"
#include "spdlog/spdlog.h"

// test related
#include "gameplay/elements/spriteomponent.hpp"
#include "resources.hpp"
#include "gameplay/elements/player/sheep.hpp"
#include "gameplay/elements/player/remotecontrol.hpp"
#include "gameplay/elements/wall.hpp"
#include "graphics/device.hpp"
#include "gameplay/elements/ground.hpp"
#include "gameplay/elements/physicalparticle.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/factory.hpp"
#include "input/keyboardinputmanager.hpp"
#include "SFML/Graphics.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "gameplay/elements/textcomponent.hpp"
#include "input/gamepadinputmanager.hpp"
#include "gameplay/elements/player/aicontrollercomponent.hpp"
#include "gameplay/bots/learning.hpp"

namespace Game {

	using namespace Math;

	GSPlay::GSPlay(const Utils::Config& _config)
		: m_rules(new Classic(m_controllers, _config.GetSection("gameplay").GetValue<int>("numWinsRequired"), 
			3.f, 30.f,
			_config.GetSection("gameplay").GetValue<int>("maxNumGames"))),
		m_autoReset(_config.GetSection("general").GetValue<bool>("AutoReset"))
	{
		spdlog::info("Creating main state");

		Map* map = new Map(m_scene);
		m_scene.Add(*map);
		m_rules->SetMap(*map);

		if (sf::Joystick::isConnected(0))
			m_input1.reset(new Input::GamePadInputInterface(_config.GetSection("gamepad1"), 0));
		else
			m_input1.reset(new Input::KeyBoardInputInterface(_config.GetSection("keyboard2")));
		if (sf::Joystick::isConnected(1))
			m_input2.reset(new Input::GamePadInputInterface(_config.GetSection("gamepad1"), 1));
		else
			m_input2.reset(new Input::KeyBoardInputInterface(_config.GetSection("keyboard1")));

		const sf::Color playerRed(206, 0, 0); 
		const sf::Color playerGreen(12, 183, 0);

		Sheep* sheep1 = new Sheep(Vec2(), playerRed);
		Sheep* sheep2 = new Sheep(Vec2(), playerGreen);
		const Utils::ConfigSection& gamplaySettings = _config.GetSection("gameplay");
		const bool autoCharge = gamplaySettings.GetValue<int>("autoChargeJump");

		const Utils::ConfigSection& learnSettings = _config.GetSection("learning");
		int controllerCount = 0;
		auto makeController = [&](Sheep& sheep, Input::InputInterface& inp, bool isAI)
		{
			const std::string controllerId = std::to_string(controllerCount);
			++controllerCount;
			if (isAI)
			{
				const std::string netNameKey = "NetworkName" + controllerId;
				const bool logGames = _config.GetSection("general").GetValue<int>("LogGames");
				return std::unique_ptr<PlayerControllerComponent>(
					new AIControllerComponent(sheep, *m_rules, 
						DoopAI(learnSettings.GetValue<std::string>(netNameKey), DoopAI::Mode::SAMPLE, learnSettings.GetValue<float>("ExploreRatio")),
						4.f, controllerId,
						logGames,
						logGames ? learnSettings.GetValue<std::string>("LogPath") : ""));
			}
			else
			{
				return std::unique_ptr<PlayerControllerComponent>(
					new PlayerControllerComponent(sheep, inp, autoCharge));
			}
		};

		m_controller1 = makeController(*sheep1, *m_input1, true);
		m_controller2 = makeController(*sheep2, *m_input2, true);
		m_controllers.push_back(m_controller1.get());
		m_controllers.push_back(m_controller2.get());
		// register after to also register the controllers
		m_scene.Add(*sheep1);
		m_scene.Add(*sheep2);

		const Math::Vec2 worldSize = Graphics::Device::GetSizeWorldSpace();

		RemoteControl* remoteControl1 = new RemoteControl(worldSize * Vec2(0.125f, -0.02f), *m_controller1, false, playerRed);
		RemoteControl* remoteControl2 = new RemoteControl(worldSize * Vec2(1.f-0.125f, -0.02f), *m_controller2, true, playerGreen);
		m_scene.Add(*remoteControl1);
		m_scene.Add(*remoteControl2);

		auto scoreActor = new SingleComponentActor<TextComponent>(worldSize * Vec2(0.5f, 0.75f),
			Resources::Load<sf::Font>("Anaktoria"), 64);
		m_scene.Add(*scoreActor);
		m_scoreScreen = &scoreActor->GetComponent();

		m_rules->Start();
	}

	GSPlay::~GSPlay()
	{}

	void GSPlay::Process(float _deltaTime)
	{
		m_scene.CleanUp();
		m_scene.RegisterActors();

		m_scene.Process(_deltaTime);
		m_rules->Process(_deltaTime);

		if (m_rules->IsOver())
		{
			if (m_autoReset)
			{
				m_rules->Reset();
				spdlog::info("Reseting rules.");
			}
			else
			{
				Finish();
				if (Classic* rules = dynamic_cast<Classic*>(m_rules.get()))
				{
					auto& results = rules->GetResults();
					spdlog::info("{} {} {}", results[0], results[1], results[2] - results[0] - results[1]);
				}
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
			m_rules->Reset();
	}

	void GSPlay::Draw(sf::RenderWindow& _window)
	{
		Graphics::Device::SetView(true);
		m_scene.Draw(_window);

		m_scoreScreen->SetText(m_rules->GetScore());
	}
}