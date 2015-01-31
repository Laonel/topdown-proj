#ifndef _BODY_H_
#define _BODY_H_

#include "Utils.h"

#include "Scene/Component.h"

#include <Box2D/Box2D.h>

class Body : public Component
{
public:

	typedef std::vector<b2Vec2> VerticesData;

	inline static Component* onBuildComponent() { return new Body(); }

	Body();

	virtual ~Body();

	b2Body* getBody();

	b2Shape* getShape();

	VerticesData& getVertices();

	void setVertices(VerticesData& verts);

	void applyVertices();

	float getRadius();

	void setRadius(float value);

	float getDensity();

	void setDensity(float value);

	float getFriction();

	void setFriction(float value);

	float getRestitution();

	void setRestitution(float value);

	b2Filter getFilter();

	void setFilter(b2Filter filter);

	b2BodyType getBodyType();

	void setBodyType(b2BodyType type);

	b2Vec2 getLinearVelocity();

	void setLinearVelocity(b2Vec2 velocity);

	float getAngularVelocity();

	void setAngularVelocity(float velocity);

	float getLinearDamping();

	void setLinearDamping(float damping);

	float getAngularDamping();

	void setAngularDamping(float damping);

	bool isSleepingAllowed();

	void setSleepingAllowed(bool value);

	bool isFixedRotation();

	void setFixedRotation(bool value);

	bool isBullet();

	void setBullet(bool value);

	float getGravityScale();

	void setGravityScale(float value);

	b2Vec2 getPosition();

	float getAngle();

protected:

	virtual void onCreate();
	virtual void onDestroy();
	virtual void onUpdate(const sf::Time& dt);
	virtual void onDuplicate(Component* dest);

private:

	b2BodyDef m_bodyDef;
	b2FixtureDef m_fixtureDef;
	VerticesData m_verts;
	b2Body* m_body;
	b2Fixture* m_fixture;
	b2Shape* m_shape;

	float m_radius;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const;

	template <class Archive>
	void load(Archive& ar, const unsigned int version);

	BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(Body, 1)

BOOST_CLASS_EXPORT_KEY(Body)

#endif
