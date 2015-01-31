#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Utils.h"

#include <Box2D/Box2D.h>
#include <SFML/System.hpp>

#include "Physics/PhysicsComponent.h"

class PhysicsManager
{
public:

	static bool init();

	static void shutdown();

	static b2Body* createBody(const b2BodyDef& def);
	static b2Joint* createJoint(const b2JointDef& def);
	static b2Fixture* createFixture(b2Body* body, const b2FixtureDef& def);

	static void destroyBody(b2Body* body);
	static void destroyJoint(b2Joint* joint);
	static void destroyFixture(b2Body* body, b2Fixture* fixture);

	static sf::Vector2f getGravity();

	static void setGravity(sf::Vector2f gravity);

	static const float getTimestep();

	static int getVelIterations();

	static int getPosIterations();

	static void update(const sf::Time& dt);

private:

	static void singleStep(float dt);

	static void smoothStates();
	static void resetSmoothStates();

private:

	static b2World* s_world;
	
	static sf::Vector2f s_gravity;

	static int s_velocityIterations;
	static int s_positionIterations;

	static const float s_timeStep;

	static double s_fixedTimestepAccumulator;
	static double s_fixedTimestepAccumulatorRatio;


};

#endif
