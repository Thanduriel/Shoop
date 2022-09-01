#include "aicontrollercomponent.hpp"
#include "../../../generators/random.hpp"

namespace Game {
	SheepState::SheepState(const Sheep& _sheep)
		: position(_sheep.GetBody().Get().GetWorldCenter()),
		velocity(_sheep.GetBody().Get().GetLinearVelocity()),
		rotation(_sheep.GetBody().Get().GetAngle()),
		angularVelocity(_sheep.GetBody().Get().GetAngularVelocity()),
		wheelAngularVelocity(_sheep.GetWheel().Get().GetAngularVelocity()),
		jumpCharge(_sheep.GetJump().GetChargeLevel())
	{
	}

	AIControllerComponent::AIControllerComponent(Sheep& _self, const Rules& _oth, std::function<AIStep> _ai, float _tickRate)
		: PlayerControllerComponent(_self, m_inputs),
		m_rules(_oth),
		m_ai(std::move(_ai)),
		m_tickTime(_tickRate > 0.f ? 1.f / _tickRate : 0.f),
		m_timePassed(0.f)
	{}

	void AIControllerComponent::Process(float _deltaTime)
	{
		m_timePassed += _deltaTime;
		if (m_timePassed >= m_tickTime) 
		{
			m_timePassed = 0.f;
			UpdatePlayerList();

			if (!m_otherPlayers.empty())
				m_ai(SheepState(static_cast<Sheep&>(*m_actor)), SheepState(*m_otherPlayers[0]), m_inputs);
		}
		PlayerControllerComponent::Process(_deltaTime);
	}

	void AIControllerComponent::UpdatePlayerList()
	{
		m_otherPlayers.clear();
		for (Actor::ConstHandle act : m_rules.GetPlayers()) 
		{
			if (act && &*act != m_actor)
				m_otherPlayers.push_back(&static_cast<const Sheep&>(*act)); 
		}
	}

	using namespace Input;
	void RandomAI::operator()(const SheepState& _self, const SheepState& _oth, Input::VirtualInputs& _inp)
	{
		_inp.axis[static_cast<size_t>(Axis::Accelerate)] = Generators::g_random.Uniform(-1.f,1.f);
		_inp.axis[static_cast<size_t>(Axis::Rotate)] = Generators::g_random.Uniform(-1.f, 1.f);
		_inp.actions[static_cast<size_t>(Action::Jump)] = Generators::g_random.Uniform(0, 1);
	}
}