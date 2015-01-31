#include "Scene/Components/Physics/Body.h"
#include "Scene/GameObject.h"

#include "Scene/Components/Transform.h"

#include "Physics/PhysicsManager.h"

Body::Body() :
	m_radius(0.f),
	m_body(0),
	m_fixture(0),
	m_shape(0)
{
}

Body::~Body()
{
	onDestroy();
}

b2Body* Body::getBody()
{
	return m_body;
}

b2Shape* Body::getShape()
{
	return m_shape;
}

Body::VerticesData& Body::getVertices()
{
	return m_verts;
}

void Body::setVertices(Body::VerticesData& verts)
{
	if (&verts != &m_verts)
	{
		m_verts.resize(0);
		if (!verts.empty())
			m_verts.assign(verts.begin(), verts.end());
	}

	applyVertices();
}

void Body::applyVertices()
{
	onCreate();
}

float Body::getRadius()
{
	return m_shape ? m_shape->m_radius : m_radius;
}

void Body::setRadius(float value)
{
	m_radius = value;
	if (m_shape)
		m_shape->m_radius = value;
}

float Body::getDensity()
{
	return m_fixture ? m_fixture->GetDensity() : m_fixtureDef.density;
}

void Body::setDensity(float value)
{
	if (m_fixture)
		m_fixture->SetDensity(value);
	else
		m_fixtureDef.density = value;
}

float Body::getFriction()
{
	return m_fixture ? m_fixture->GetFriction() : m_fixtureDef.friction;
}

void Body::setFriction(float value)
{
	if (m_fixture)
		m_fixture->SetFriction(value);
	else
		m_fixtureDef.friction = value;
}

float Body::getRestitution()
{
	return m_fixture ? m_fixture->GetRestitution() : m_fixtureDef.restitution;
}

void Body::setRestitution(float value)
{
	if (m_fixture)
		m_fixture->SetRestitution(value);
	else
		m_fixtureDef.restitution = value;
}

b2Filter Body::getFilter()
{
	return m_fixture ? m_fixture->GetFilterData() : m_fixtureDef.filter;
}

void Body::setFilter(b2Filter filter)
{
	if (m_fixture)
		m_fixture->SetFilterData(filter);
	else
		m_fixtureDef.filter = filter;
}

b2BodyType Body::getBodyType()
{
	return m_body ? m_body->GetType() : m_bodyDef.type;
}

void Body::setBodyType(b2BodyType type)
{
	if (m_body)
		m_body->SetType(type);
	else
		m_bodyDef.type = type;
}

b2Vec2 Body::getLinearVelocity()
{
	return m_body ? m_body->GetLinearVelocity() : m_bodyDef.linearVelocity;
}

void Body::setLinearVelocity(b2Vec2 velocity)
{
	if (m_body)
		m_body->SetLinearVelocity(velocity);
	else
		m_bodyDef.linearVelocity = velocity;
}

float Body::getAngularVelocity()
{
	return m_body ? m_body->GetAngularVelocity() : m_bodyDef.angularVelocity;
}

void Body::setAngularVelocity(float velocity)
{
	if (m_body)
		m_body->SetAngularVelocity(velocity);
	else
		m_bodyDef.angularVelocity = velocity;
}

float Body::getLinearDamping()
{
	return m_body ? m_body->GetLinearDamping() : m_bodyDef.linearDamping;
}

void Body::setLinearDamping(float damping)
{
	if (m_body)
		m_body->SetLinearDamping(damping);
	else
		m_bodyDef.linearDamping = damping;
}

float Body::getAngularDamping()
{
	return m_body ? m_body->GetAngularDamping() : m_bodyDef.angularDamping;
}

void Body::setAngularDamping(float damping)
{
	if (m_body)
		m_body->SetAngularDamping(damping);
	else
		m_bodyDef.angularDamping = damping;
}

bool Body::isSleepingAllowed()
{
	return m_body ? m_body->IsSleepingAllowed() : m_bodyDef.allowSleep;
}

void Body::setSleepingAllowed(bool value)
{
	if (m_body)
		m_body->SetSleepingAllowed(value);
	else
		m_bodyDef.allowSleep = value;
}

bool Body::isFixedRotation()
{
	return m_body ? m_body->IsFixedRotation() : m_bodyDef.fixedRotation;
}

void Body::setFixedRotation(bool value)
{
	if (m_body)
		m_body->SetFixedRotation(value);
	else
		m_bodyDef.fixedRotation = value;
}

bool Body::isBullet()
{
	return m_body ? m_body->IsBullet() : m_bodyDef.bullet;
}

void Body::setBullet(bool value)
{
	if (m_body)
		m_body->SetBullet(value);
	else
		m_bodyDef.bullet = value;
}

float Body::getGravityScale()
{
	return m_body ? m_body->GetGravityScale() : m_bodyDef.gravityScale;
}

void Body::setGravityScale(float value)
{
	if (m_body)
		m_body->SetGravityScale(value);
	else
		m_bodyDef.gravityScale = value;
}

b2Vec2 Body::getPosition()
{
	return m_body ? m_body->GetPosition() : m_bodyDef.position;
}

float Body::getAngle()
{
	return m_body ? m_body->GetAngle() : m_bodyDef.angle;
}

void Body::onCreate()
{
	if (!getOwner() || getOwner()->isPrefab())
		return;

	onDestroy();
 	Transform* trans = getOwner()->getComponent<Transform>();
	if (trans)
	{
		sf::Vector2f pos = trans->getPosition();
		m_bodyDef.position = b2Vec2(pos.x, pos.y);
		m_bodyDef.angle = degToRad(trans->getRotation());
	}

	m_body = PhysicsManager::createBody(m_bodyDef);
	m_body->SetUserData(getOwner());
	m_shape = m_verts.size() ? (b2Shape*)(new b2PolygonShape()) : (b2Shape*)(new b2CircleShape());
	m_shape->m_radius = m_radius;
	if (m_shape->GetType() == b2Shape::e_polygon)
		((b2PolygonShape*)(m_shape))->Set(m_verts.data(), m_verts.size());
	m_fixtureDef.shape = m_shape;
	m_fixture = PhysicsManager::createFixture(m_body, m_fixtureDef);
	m_fixture->SetUserData(getOwner());
}

void Body::onDestroy()
{
	if (!getOwner() || getOwner()->isPrefab())
		return;

	if (m_fixture)
	{
		m_fixtureDef.shape = 0;
		m_fixtureDef.density = m_fixture->GetDensity();
		m_fixtureDef.friction = m_fixture->GetFriction();
		m_fixtureDef.restitution = m_fixture->GetRestitution();
		m_fixtureDef.filter = m_fixture->GetFilterData();
	}

	if (m_body)
	{
		m_body->DestroyFixture(m_fixture);
		m_bodyDef.type = m_body->GetType();
		m_bodyDef.linearVelocity = m_body->GetLinearVelocity();
		m_bodyDef.angularVelocity = m_body->GetAngularVelocity();
		m_bodyDef.linearDamping = m_body->GetLinearDamping();
		m_bodyDef.angularDamping = m_body->GetAngularDamping();
		m_bodyDef.allowSleep = m_body->IsSleepingAllowed();
		m_bodyDef.fixedRotation = m_body->IsFixedRotation();
		m_bodyDef.bullet = m_body->IsBullet();
		m_bodyDef.gravityScale = m_body->GetGravityScale();
		m_bodyDef.position = m_body->GetPosition();
		m_bodyDef.angle = m_body->GetAngle();
		PhysicsManager::destroyBody(m_body);

		Transform* trans = getOwner()->getComponent<Transform>();
		if (trans)
		{
			trans->setPosition(sf::Vector2f(m_bodyDef.position.x, m_bodyDef.position.y));
			trans->setRotation(radToDeg(m_bodyDef.angle));
		}
	}

	m_body = 0;
	m_fixture = 0;

	if (m_shape)
		m_radius = m_shape->m_radius;

	DELETE_OBJECT(m_shape);
}

void Body::onUpdate(const sf::Time& dt)
{
	Transform* trans = getOwner()->getComponent<Transform>();
	if (trans)
	{
		b2Vec2 pos = getPosition();
		trans->setPosition(sf::Vector2f(pos.x, pos.y));
		trans->setRotation(radToDeg(getAngle()));
	}
}

void Body::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	Component::onDuplicate(dest);
	if (PTR_TYPEID(dest) != typeid(Body))
		return;

	Body* c = (Body*)(dest);
	c->setRadius(getRadius());
	c->setVertices(getVertices());
	c->setDensity(getDensity());
	c->setFriction(getFriction());
	c->setRestitution(getRestitution());
	c->setFilter(getFilter());
	c->setBodyType(getBodyType());
	c->setLinearVelocity(getLinearVelocity());
	c->setAngularVelocity(getAngularVelocity());
	c->setLinearDamping(getLinearDamping());
	c->setAngularDamping(getAngularDamping());
	c->setSleepingAllowed(isSleepingAllowed());
	c->setFixedRotation(isFixedRotation());
	c->setBullet(isBullet());
	c->setGravityScale(getGravityScale());
}

template <class Archive>
void Body::save(Archive& ar, const unsigned int version) const
{
	ar & boost::serialization::base_object<const Component>(*this);

	ar & boost::serialization::make_nvp("bodyDef", m_bodyDef);
	ar & boost::serialization::make_nvp("fixtureDef", m_fixtureDef);
	ar & boost::serialization::make_nvp("verts", m_verts);
	ar & boost::serialization::make_nvp("radius", m_radius);
}

template <class Archive>
void Body::load(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Component>(*this);

	ar & boost::serialization::make_nvp("bodyDef", m_bodyDef);
	ar & boost::serialization::make_nvp("fixtureDef", m_fixtureDef);
	ar & boost::serialization::make_nvp("verts", m_verts);
	ar & boost::serialization::make_nvp("radius", m_radius);
}

DECLARE_BINARY_SAVE(Body)
DECLARE_BINARY_LOAD(Body)

BOOST_CLASS_EXPORT_IMPLEMENT(Body)
