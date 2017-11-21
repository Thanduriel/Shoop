#pragma once

#define THISACTOR *static_cast<Actor*>(this)

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
		ProcessComponent(Actor& _actor) : Component(_actor, Type::Process) {}

		virtual void Process(float _deltaTime) = 0;

		void SetActive(bool _active) { m_isActive = _active; }
		bool IsActive() const { return m_isActive; }
	private:
		bool m_isActive;
	};

	class DrawComponent : public Component
	{
	public:
		DrawComponent(Actor& _actor) : Component(_actor, Type::Draw) {}

		virtual void Draw() = 0;
	};


}