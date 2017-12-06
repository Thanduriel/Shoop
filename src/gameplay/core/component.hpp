#pragma once

#define THISACTOR *static_cast<Actor*>(this)

namespace sf {
	class RenderWindow;
}

namespace Game {

	class Actor;

	class Component
	{
	public:
		enum struct Type
		{
			Draw,
			Process,
			Factory
		};

		Component(Actor& _actor, Type _type);
		~Component();
		
		// A component always requires an owning Actor.
		// Reasonable copy constructors take an Actor&.
		Component() = delete;
		Component(Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&) = delete;
		Component& operator=(Component&&) = delete;

		Actor& GetActor() const { return m_actor; }
		Type GetType() const { return m_type; }
	protected:
		Actor& m_actor;
		const Type m_type;
	};

	class ProcessComponent : public Component
	{
	public:
		ProcessComponent(Actor& _actor, bool _canTick = true) : Component(_actor, Type::Process),m_canTick(_canTick) {}

		virtual void Process(float _deltaTime) = 0;

		void SetActive(bool _active) { m_isActive = _active; }
		bool IsActive() const { return m_isActive; }

		bool CanTick() const { return m_canTick; }
	protected:
		// when set to false at time of register, this Components Process is never called.
		const bool m_canTick;
	private:
		bool m_isActive;
	};

	class DrawComponent : public Component
	{
	public:
		DrawComponent(Actor& _actor) : Component(_actor, Type::Draw) {}

		virtual void Draw(sf::RenderWindow&) = 0;
	};


}