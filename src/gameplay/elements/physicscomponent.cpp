#include "physicscomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "Box2D/Box2D.h"
#include "utils/assert.hpp"

namespace Game {

	PhysicsBodyComponent::PhysicsBodyComponent(Actor& _actor, bool _isPrimary)
		: ProcessComponent(_actor, _isPrimary),
		m_body(nullptr)
	{}

	PhysicsBodyComponent::PhysicsBodyComponent(Actor& _actor, const b2BodyDef& _def, bool _isPrimary)
		: ProcessComponent(_actor, _isPrimary)
	{
	//	Create(_def);
	}

	PhysicsBodyComponent::~PhysicsBodyComponent()
	{
		Assert(m_body, "An uninitialized body component is being destroyed.");
		Details::PhysicsWorldWrapper::m_world->DestroyBody(m_body);
	}

	void PhysicsBodyComponent::Process(float _deltaTime)
	{
		m_actor.SetPosition(m_body->GetPosition());
		m_actor.SetRotation(m_body->GetAngle());
	}

	b2Body& PhysicsBodyComponent::Create(const b2BodyDef& _def, const b2FixtureDef& _fixtureDef)
	{
		Assert(!m_body, "A body should only be created once per component.");
		m_body = Details::PhysicsWorldWrapper::m_world->CreateBody(&_def);
		m_body->CreateFixture(&_fixtureDef);

		return *m_body;
	}

	// ****************************************************************** //
	PhysicsJointComponent::PhysicsJointComponent(Actor& _actor)
		: ProcessComponent(_actor, false),
		m_joint(nullptr)
	{}

	PhysicsJointComponent::PhysicsJointComponent(Actor& _actor, const b2JointDef& _def)
		: ProcessComponent(_actor, false)
	{
		Create(_def);
	}

	PhysicsJointComponent::~PhysicsJointComponent()
	{
		Assert(m_joint, "An uninitialized joint component is being destroyed.");
		Details::PhysicsWorldWrapper::m_world->DestroyJoint(m_joint);
	}

	b2Joint& PhysicsJointComponent::Create(const b2JointDef& _def)
	{
		Assert(!m_joint, "A joint should only be created once per component.");
		m_joint = Details::PhysicsWorldWrapper::m_world->CreateJoint(&_def);

		return *m_joint;
	}

	b2World* Details::PhysicsWorldWrapper::m_world;
}