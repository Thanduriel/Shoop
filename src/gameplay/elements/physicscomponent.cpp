#include "physicscomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "Box2D/Box2D.h"
#include "utils/assert.hpp"
#include "SFML/Graphics.hpp"
#include "graphics/device.hpp"

namespace Game {

	using namespace Math;

	b2World* Details::PhysicsWorldWrapper::m_world;

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
		// when the scene is destroyed m_world is set to nullptr and all its objects
		// are destroyed at once.
		if(Details::PhysicsWorldWrapper::m_world)
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
		if (Details::PhysicsWorldWrapper::m_world)
			Details::PhysicsWorldWrapper::m_world->DestroyJoint(m_joint);
	}

	b2Joint& PhysicsJointComponent::Create(const b2JointDef& _def)
	{
		Assert(!m_joint, "A joint should only be created once per component.");
		m_joint = Details::PhysicsWorldWrapper::m_world->CreateJoint(&_def);

		return *m_joint;
	}



	// ****************************************************************** //
	PhysicsDebugComponent::PhysicsDebugComponent(Actor& _actor, const PhysicsBodyComponent& _component)
		: DrawComponent(_actor),
		m_target(_component)
	{

	}

	void PhysicsDebugComponent::Draw(sf::RenderWindow& _window)
	{
		const b2Body& body = m_target.Get();
		const b2Fixture& fixture = body.GetFixtureList()[0];
		const b2Shape& physShape = *fixture.GetShape();
		switch (fixture.GetShape()->m_type)
		{
		case b2Shape::e_circle:
		{
			sf::CircleShape shape(fixture.GetShape()->m_radius);
			shape.setPosition(Graphics::Device::ToScreenSpace(body.GetPosition()));
			shape.setOrigin(Vec2(shape.getRadius()));
			_window.draw(shape);
			break;
		}
		case b2Shape::e_polygon:
		{
			const b2PolygonShape& polyShape = static_cast<const b2PolygonShape&>(physShape);
			sf::ConvexShape shape(polyShape.m_count);
			for (int i = 0; i < polyShape.m_count; ++i)
				shape.setPoint(i, Vec2(polyShape.m_vertices[i]));
			shape.setPosition(Graphics::Device::ToScreenSpace(body.GetPosition()));
			shape.setOrigin(Vec2(polyShape.m_centroid));
			_window.draw(shape);
			break;
		}
		default:
			Assert(false, "This shape type is not supported for debug draw.");
		}
		
		
		
	}

}