#include "gamerules.hpp"
#include "gameplay/elements/player/sheep.hpp"

namespace Game {

	Rules::Rules(ControllerContainer& _controllers)
		: m_controllers(_controllers),
		m_map(nullptr)
	{
	}

	void Rules::SpawnPlayer(PlayerId _id, Math::Vec2 _position, bool _flip)
	{
		assert(m_map);

		Sheep& sheep = m_map->GetFactory().Create<Sheep>(_position);
		m_players[_id] = sheep.GetHandle();
		m_controllers[_id]->Attach(sheep);

		if (_flip) sheep.Flip(true);
	}

	bool Rules::IsDead(const Actor::ConstHandle& _handle)
	{
		return !_handle || _handle->IsDestroyed();
	}
}