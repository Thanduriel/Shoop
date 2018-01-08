#pragma once

#include <vector>
#include <functional>
#include "math/transformation.hpp"
#include "gameplay/core/component.hpp"
#include "Box2D/Box2D.h"
/*
class b2World;
struct b2BodyDef;
struct b2JointDef;
struct b2FixtureDef;
class b2Body;
class b2Fixture;
class b2Joint;*/

namespace Game {

	// Wrapper for a b2 physics body.
	class PhysicsBodyComponent : public ProcessComponent
	{
	public:
		/* Creates an empty PhysicsBodyComponent that has to be initialized with Create().
		 * @param _overwriteTransform The given transformation is updated with the current 
		 *			orientation of this body on process.
		 */
		PhysicsBodyComponent(Actor& _actor, Math::Transformation* _overwriteTransform = nullptr);
		~PhysicsBodyComponent();

		void Process(float _deltaTime);

		// Creates the underlying body with the given definition.
		b2Body& Create(const b2BodyDef& _def, const b2FixtureDef& _fixtureDef);
		b2Body& Create(const b2BodyDef& _def, const std::vector<b2FixtureDef*>& _fixtureDefs);

		// Get access to the underlying body.
		const b2Body& Get() const { return *m_body; }
		b2Body& Get() { return *m_body; }

		typedef std::function<void(b2Fixture&, b2Fixture&)> ContactEvent;

		void SetOnContactBegin(ContactEvent&& _event) { m_onContactBegin = std::move(_event); }
		void SetOnContactEnd(ContactEvent&& _event) { m_onContactEnd = std::move(_event); }
	private:
		b2Body* m_body;
		Math::Transformation* m_overwriteTransform;
		ContactEvent m_onContactBegin;
		ContactEvent m_onContactEnd;

		friend class ContactListener;
	};

	// Wrapper for a b2 physics joint.
	// Uses the same initialization concept as PhysicsBodyComponent.
	class PhysicsJointComponent : public ProcessComponent
	{
	public:
		PhysicsJointComponent(Actor& _actor);
		~PhysicsJointComponent();

		void Process(float _deltaTime) {}

		b2Joint& Create(const b2JointDef& _body);

		const b2Joint& Get() const { return *m_joint; }
		b2Joint& Get() { return *m_joint; }
	private:
		b2Joint* m_joint;
	};

	// Structure that provides additional information to a body or fixture given as user data.
	struct PhysicsInfo
	{
		// Acquire the PhysicsInfo of a fixture.
		static PhysicsInfo& Get(const b2Fixture& _fixture) {
			return *static_cast<PhysicsInfo*>(_fixture.GetUserData());
		}
		enum Flags
		{
			IsLethal = 1
		};
		unsigned flags;
	};

	// Component that draws a shape representative to the bodies first shape.
	class PhysicsDebugComponent : public DrawComponent
	{
	public:
		PhysicsDebugComponent(Actor& _actor, const PhysicsBodyComponent& _component);

		void Draw(sf::RenderWindow& _window) override;

	private:
		const PhysicsBodyComponent& m_target;
	};

	// Helper to simplify debug usage.
	// Just use this type instead of a PhysicsBodyComponent
	class PhysicsBodyComponentD : public PhysicsBodyComponent
	{
	public:
		template<typename... Args>
		PhysicsBodyComponentD(Actor& _actor, Args&&... _args)
			: PhysicsBodyComponent(_actor, std::forward<Args>(_args)...),
			m_debugDraw(_actor, *this)
		{}
	private:
		PhysicsDebugComponent m_debugDraw;
	};

	// The contact event manager
	class ContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {}
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {}
	};

	class Scene;

	namespace Details {
		class PhysicsWorldWrapper {
		private:
			static b2World* m_world;

			// physics components can access the world for creation and destruction
			friend b2Body& PhysicsBodyComponent::Create(const b2BodyDef&, const b2FixtureDef&);
			friend b2Body& PhysicsBodyComponent::Create(const b2BodyDef&, const std::vector<b2FixtureDef*>&);
			friend PhysicsBodyComponent::~PhysicsBodyComponent();

			friend b2Joint& PhysicsJointComponent::Create(const b2JointDef&);
			friend PhysicsJointComponent::~PhysicsJointComponent();

			// the scene controls the world
			friend Game::Scene;
		};

	}
}