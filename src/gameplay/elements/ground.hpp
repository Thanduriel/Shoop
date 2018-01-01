#pragma once

#include "gameplay/core/actor.hpp"
#include "physicscomponent.hpp"
#include "generators/curve.hpp"

namespace Game {

	class GroundPlane : public Actor
	{
	public:
		// A physical ground line created from the given _curve.
		GroundPlane(const Generators::Curve& _curve);
	private:
		PhysicsBodyComponentD m_body;
	};
}