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

		Component(Actor& _actor, Type _type, bool _shouldRegister = true);
		virtual ~Component();
		
		// A component always requires an owning Actor.
		// Reasonable copy constructors take an Actor&.
		Component() = delete;
		Component(Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&) = delete;
		Component& operator=(Component&&) = delete;
		
		Actor& GetActor() const { return *m_actor; }
		// move this component to another actor
		void Attach(Actor& _actor);
		Type GetType() const { return m_type; }
		bool ShouldRegister() const { return m_shouldRegister; }
	protected:
		mutable Actor* m_actor;
		const Type m_type;

	private:
		bool m_shouldRegister;

		friend class Actor;
	};

	class ProcessComponent : public Component
	{
	public:
		ProcessComponent(Actor& _actor, bool _canTick = true) 
			: Component(_actor, Type::Process, _canTick), m_isActive(true) 
		{
		}

		virtual void Process(float _deltaTime) = 0;

		void SetActive(bool _active) { m_isActive = _active; }
		bool IsActive() const { return m_isActive; }

	protected:

	private:
		bool m_isActive;
	};

	class DrawComponent : public Component
	{
	public:
		enum struct DrawingOrder
		{
			Background,
			Middle,
			Front,
			Debug,
			COUNT
		};

		DrawComponent(Actor& _actor, DrawingOrder _order = DrawingOrder::Middle) : Component(_actor, Type::Draw), m_drawingOrder(_order) {}

		virtual void Draw(sf::RenderWindow&) = 0;

		DrawingOrder GetDrawingOrder() const { return m_drawingOrder; }
	private:
		const DrawingOrder m_drawingOrder;
	};


}