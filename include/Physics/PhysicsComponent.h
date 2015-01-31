#ifndef _PHYSICS_COMPONENT_H_
#define _PHYSICS_COMPONENT_H_

#include <Box2D/Dynamics/b2Body.h>

struct PhysicsComponent
{
	static PhysicsComponent bodyToComponent(const b2Body* b)
	{
		static PhysicsComponent c;
		c.smoothedPosition = c.previousPosition = b->GetPosition();
		c.smoothedAngle = c.previousAngle = b->GetAngle();

		return c;
	}

	b2Vec2 previousPosition;
	float32 previousAngle;

	b2Vec2 smoothedPosition;
	float32 smoothedAngle;
};

#endif
