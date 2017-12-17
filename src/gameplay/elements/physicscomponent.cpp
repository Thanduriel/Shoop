#include "physicscomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "Box2D/Box2D.h"
#include "utils/assert.hpp"
#include "SFML/Graphics.hpp"
#include "graphics/device.hpp"
#include "math/helpers.hpp"

namespace Game {

	using namespace Math;

	b2World* Details::PhysicsWorldWrapper::m_world;

	PhysicsBodyComponent::PhysicsBodyComponent(Actor& _actor, Math::Transformation* _overwriteTransform)
		: ProcessComponent(_actor, _overwriteTransform),
		m_body(nullptr),
		m_overwriteTransform(_overwriteTransform)
	{}

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
		// process is only called when initialized with a transformation
		m_overwriteTransform->SetPosition(m_body->GetWorldCenter());//
		m_overwriteTransform->SetRotation(m_body->GetAngle());
	}

	b2Body& PhysicsBodyComponent::Create(const b2BodyDef& _def, const b2FixtureDef& _fixtureDef)
	{
		Assert(!m_body, "A body should only be created once per component.");
		m_body = Details::PhysicsWorldWrapper::m_world->CreateBody(&_def);
		m_body->CreateFixture(&_fixtureDef);

		return *m_body;
	}

	b2Body& PhysicsBodyComponent::Create(const b2BodyDef& _def, const std::vector<b2FixtureDef*>& _fixtureDefs)
	{
		Assert(!m_body, "A body should only be created once per component.");
		m_body = Details::PhysicsWorldWrapper::m_world->CreateBody(&_def);
		for(b2FixtureDef* fixture : _fixtureDefs)
			m_body->CreateFixture(fixture);

		return *m_body;
	}

	// ****************************************************************** //
	PhysicsJointComponent::PhysicsJointComponent(Actor& _actor)
		: ProcessComponent(_actor, false),
		m_joint(nullptr)
	{}

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
		: DrawComponent(_actor, DrawingOrder::Debug),
		m_target(_component)
	{

	}

	void PhysicsDebugComponent::Draw(sf::RenderWindow& _window)
	{
		const b2Body& body = m_target.Get();
		const b2Fixture* fixture = body.GetFixtureList();

		do {
			const b2Shape& physShape = *fixture->GetShape();
			switch (fixture->GetShape()->m_type)
			{
			case b2Shape::e_circle:
			{
				const b2CircleShape& circleShape = static_cast<const b2CircleShape&>(physShape);
				sf::CircleShape shape(fixture->GetShape()->m_radius);
				shape.setPosition(Graphics::Device::ToScreenSpace(body.GetPosition() 
					+ b2Mul(body.GetTransform().q, circleShape.m_p)));
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
				shape.setRotation(-Math::ToDegree(body.GetTransform().q.GetAngle()));
				_window.draw(shape);
				break;
			}
			default:
				Assert(false, "This shape type is not supported for debug draw.");
			}
		} while (fixture = fixture->GetNext());
		
		
		
	}

}