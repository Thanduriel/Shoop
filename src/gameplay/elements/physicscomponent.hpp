#pragma once

#include "gameplay/core/component.hpp"

class b2World;
struct b2BodyDef;
struct b2JointDef;
struct b2FixtureDef;
class b2Body;
class b2Joint;

namespace Game {
	// Wrapper for a b2 physics body.
	class PhysicsBodyComponent : public ProcessComponent
	{
	public:
		/* Creates an empty PhysicsBodyComponent that has to be initialized with Create().
		 * @param _isPrimary The primary physics body of an Actor determines its orientation and position.
		 */
		PhysicsBodyComponent(Actor& _actor, bool _isPrimary = false);
		PhysicsBodyComponent(Actor& _actor, const b2BodyDef& _def, bool _isPrimary = false);
		~PhysicsBodyComponent();

		void Process(float _deltaTime);

		// Creates the underlying body with the given definition.
		// This should only be done once and only if the simple constructor was used.
		b2Body& Create(const b2BodyDef& _def, const b2FixtureDef& _fixtureDef);

		// Get access to the underlying body.
		const b2Body& Get() const { return *m_body; }
		b2Body& Get() { return *m_body; }
	private:
		b2Body* m_body;
	};

	// Wrapper for a b2 physics joint.
	// Uses the same initialization concept as PhysicsBodyComponent.
	class PhysicsJointComponent : public ProcessComponent
	{
	public:
		PhysicsJointComponent(Actor& _actor);
		PhysicsJointComponent(Actor& _actor, const b2JointDef& _def);
		~PhysicsJointComponent();

		void Process(float _deltaTime) {}

		b2Joint& Create(const b2JointDef& _body);

		const b2Joint& Get() const { return *m_joint; }
		b2Joint& Get() { return *m_joint; }
	private:
		b2Joint* m_joint;
	};


	class Scene;

	namespace Details {
		class PhysicsWorldWrapper {
		private:
			static b2World* m_world;

			// physics components can access the world for creation and destruction
			friend b2Body& PhysicsBodyComponent::Create(const b2BodyDef&, const b2FixtureDef&);
			friend PhysicsBodyComponent::~PhysicsBodyComponent();

			friend b2Joint& PhysicsJointComponent::Create(const b2JointDef&);
			friend PhysicsJointComponent::~PhysicsJointComponent();

			// the scene controls the world
			friend Game::Scene;
		};
	}
}