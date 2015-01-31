#include "Physics/PhysicsManager.h"

b2World* PhysicsManager::s_world = 0;
	
sf::Vector2f PhysicsManager::s_gravity = sf::Vector2f(0, 0);

int PhysicsManager::s_velocityIterations = 8;
int PhysicsManager::s_positionIterations = 3;

const float PhysicsManager::s_timeStep = 60.f;

double PhysicsManager::s_fixedTimestepAccumulator = 0.f;
double PhysicsManager::s_fixedTimestepAccumulatorRatio = 0.f;

bool PhysicsManager::init()
{
	s_world = new b2World(b2Vec2(s_gravity.x, s_gravity.y));
	s_world->SetAutoClearForces(false);

	return true;
}

void PhysicsManager::shutdown()
{
	if (s_world)
		delete s_world;
}

b2Body* PhysicsManager::createBody(const b2BodyDef& def)
{
	return s_world->CreateBody(&def);
}

b2Joint* PhysicsManager::createJoint(const b2JointDef& def)
{
	return s_world->CreateJoint(&def);
}

b2Fixture* PhysicsManager::createFixture(b2Body* body, const b2FixtureDef& def)
{
	return body->CreateFixture(&def);
}

void PhysicsManager::destroyBody(b2Body* body)
{
	s_world->DestroyBody(body);
}

void PhysicsManager::destroyJoint(b2Joint* joint)
{
	s_world->DestroyJoint(joint);
}

void PhysicsManager::destroyFixture(b2Body* body, b2Fixture* fixture)
{
	body->DestroyFixture(fixture);
}

sf::Vector2f PhysicsManager::getGravity()
{
	return s_gravity;
}

void PhysicsManager::setGravity(sf::Vector2f gravity)
{
	s_gravity = gravity;
	s_world->SetGravity(b2Vec2(gravity.x, gravity.y));
}

const float PhysicsManager::getTimestep()
{
	return s_timeStep;
}

int PhysicsManager::getVelIterations()
{
	return s_velocityIterations;
}

int PhysicsManager::getPosIterations()
{
	return s_positionIterations;
}

void PhysicsManager::update(const sf::Time& dt)
{
	const int maxSteps = 5;

	s_fixedTimestepAccumulator += dt.asSeconds();
	const int steps = static_cast<int>(std::floor(s_fixedTimestepAccumulator / s_timeStep));

	if (steps > 0)
		s_fixedTimestepAccumulator -= steps * s_timeStep;

	assert("Accumulator must have a value lesser than the fixed time step" &&
		   s_fixedTimestepAccumulator < s_timeStep + FLT_EPSILON);

	s_fixedTimestepAccumulatorRatio = s_fixedTimestepAccumulator / s_timeStep;

	const int stepsClamped = std::min(steps, maxSteps);
	for (int i = 0; i < stepsClamped; ++i)
	{
		resetSmoothStates();
		singleStep(s_timeStep);
	}

	s_world->ClearForces();

	smoothStates();
}

void PhysicsManager::singleStep(float dt)
{
	s_world->Step(dt, s_velocityIterations, s_positionIterations);
}

void PhysicsManager::smoothStates()
{
	const float oneMinusRatio = 1.f - s_fixedTimestepAccumulatorRatio;

	for (b2Body* b = s_world->GetBodyList(); b != NULL; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody)
			continue;

		PhysicsComponent c = PhysicsComponent::bodyToComponent(b);
		c.smoothedPosition = s_fixedTimestepAccumulatorRatio * b->GetPosition() + oneMinusRatio * c.previousPosition;
		c.smoothedAngle = s_fixedTimestepAccumulatorRatio * b->GetAngle() + oneMinusRatio * c.previousAngle;
	}
}

void PhysicsManager::resetSmoothStates()
{
	for (b2Body* b = s_world->GetBodyList(); b != NULL; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody)
			continue;
	}
}
