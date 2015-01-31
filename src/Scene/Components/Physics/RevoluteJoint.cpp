#include "Scene/Components/Physics/RevoluteJoint.h"
#include "Scene/Components/Physics/Body.h"

#include "Scene/GameObject.h"

#include "Physics/PhysicsManager.h"

RevoluteJoint::RevoluteJoint() :
	m_joint(0)
{
}

RevoluteJoint::~RevoluteJoint()
{
	onDestroy();
}

std::string RevoluteJoint::getBindingA()
{
	return m_bindingA;
}

void RevoluteJoint::setBindingA(std::string str)
{
	m_bindingA = str;
}

std::string RevoluteJoint::getBindingB()
{
	return m_bindingB;
}

void RevoluteJoint::setBindingB(std::string str)
{
	m_bindingB = str;
}

bool RevoluteJoint::getCollideConnected()
{
	return m_jointDef.collideConnected;
}

void RevoluteJoint::setCollideConnected(bool value)
{
	m_jointDef.collideConnected = value;
}

float RevoluteJoint::getReferenceAngle()
{
	return m_jointDef.referenceAngle;
}

void RevoluteJoint::setReferenceAngle(float angle)
{
	m_jointDef.referenceAngle = angle;
}

b2Vec2 RevoluteJoint::getLocalAnchorA()
{
	return m_jointDef.localAnchorA;
}

void RevoluteJoint::setLocalAnchorA(b2Vec2 anchor)
{
	m_jointDef.localAnchorA = anchor;
}

b2Vec2 RevoluteJoint::getLocalAnchorB()
{
	return m_jointDef.localAnchorB;
}

void RevoluteJoint::setLocalAnchorB(b2Vec2 anchor)
{
	m_jointDef.localAnchorB = anchor;
}

bool RevoluteJoint::isLimitEnabled()
{
	return m_joint ? m_joint->IsLimitEnabled() : m_jointDef.enableLimit;
}

void RevoluteJoint::setLimitEnabled(bool value)
{
	if (m_joint)
		m_joint->EnableLimit(value);
	else
		m_jointDef.enableLimit = value;
}

float RevoluteJoint::getLowerLimit()
{
	return m_joint ? m_joint->GetLowerLimit() : m_jointDef.lowerAngle;
}

void RevoluteJoint::setLowerLimit(float limit)
{
	if (m_joint)
		m_joint->SetLimits(limit, m_joint->GetUpperLimit());
	else
		m_jointDef.lowerAngle = limit;
}

float RevoluteJoint::getUpperLimit()
{
	return m_joint ? m_joint->GetUpperLimit() : m_jointDef.upperAngle;
}

void RevoluteJoint::setUpperLimit(float limit)
{
	if (m_joint)
		m_joint->SetLimits(m_joint->GetLowerLimit(), limit);
	else
		m_jointDef.upperAngle = limit;
}

bool RevoluteJoint::isMotorEnabled()
{
	return m_joint ? m_joint->IsMotorEnabled() : m_jointDef.enableMotor;
}

void RevoluteJoint::setMotorEnabled(bool value)
{
	if (m_joint)
		m_joint->EnableMotor(value);
	else
		m_jointDef.enableMotor = value;
}

float RevoluteJoint::getMotorSpeed()
{
	return m_joint ? m_joint->GetMotorSpeed() : m_jointDef.motorSpeed;
}

void RevoluteJoint::setMotorSpeed(float speed)
{
	if (m_joint)
		m_joint->SetMotorSpeed(speed);
	else
		m_jointDef.motorSpeed = speed;
}

float RevoluteJoint::getMotorTorgue()
{
	return m_joint ? m_joint->GetMaxMotorTorque() : m_jointDef.maxMotorTorque;
}

void RevoluteJoint::setMotorTorgue(float torgue)
{
	if (m_joint)
		m_joint->SetMaxMotorTorque(torgue);
	else
		m_jointDef.maxMotorTorque = torgue;
}

void RevoluteJoint::onCreate()
{
	if (!getOwner() || getOwner()->isPrefab())
		return;

	onDestroy();

	if (m_bindingA.empty() || m_bindingB.empty())
		return;

	GameObject* a = getOwner()->findChildren(m_bindingA);
	if (!a)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "cannot find gameobject " << m_bindingA << " for binding A in: "
			<< getOwner()->getId() << std::endl;
		return;
	}

	Body* ba = a->getComponent<Body>();
	if (!ba)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "gameobject " << m_bindingA << "does not have Body component" << std::endl;
		return;
	}

	GameObject* b = getOwner()->findChildren(m_bindingB);
	if (!b)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "cannot find gameobject " << m_bindingB << " for binding B in: "
			<< getOwner()->getId() << std::endl;
		return;
	}

	Body* bb = b->getComponent<Body>();
	if (!bb)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "gameobject " << m_bindingB << "does not have Body component" << std::endl;
		return;
	}

	if (ba == bb)
	{
		IF_PRINT_WARNING(SCENE_DEBUG) << "gameobjects binding A and B are the same" << std::endl;
		return;
	}

	m_jointDef.bodyA = ba->getBody();
	m_jointDef.bodyB = bb->getBody();
	m_joint = (b2RevoluteJoint*)(PhysicsManager::createJoint(m_jointDef));
	m_joint->SetUserData(getOwner());
}

void RevoluteJoint::onDestroy()
{
	if (!getOwner() || getOwner()->isPrefab())
		return;

	if (m_joint)
	{
		m_jointDef.enableLimit = m_joint->IsLimitEnabled();
		m_jointDef.lowerAngle = m_joint->GetLowerLimit();
		m_jointDef.upperAngle = m_joint->GetUpperLimit();
		m_jointDef.enableMotor = m_joint->IsMotorEnabled();
		m_jointDef.motorSpeed = m_joint->GetMotorSpeed();
		m_jointDef.maxMotorTorque = m_joint->GetMaxMotorTorque();

		PhysicsManager::destroyJoint(m_joint);
	}

	m_joint = 0;
}

void RevoluteJoint::onDuplicate(Component* dest)
{
	if (!dest)
		return;

	Component::onDuplicate(dest);
	if (PTR_TYPEID(dest) != typeid(RevoluteJoint))
		return;

	RevoluteJoint* c = (RevoluteJoint*)(dest);
	c->setBindingA(getBindingA());
	c->setBindingB(getBindingB());
	c->setCollideConnected(getCollideConnected());
	c->setReferenceAngle(getReferenceAngle());
	c->setLocalAnchorA(getLocalAnchorA());
	c->setLocalAnchorB(getLocalAnchorB());
	c->setLimitEnabled(isLimitEnabled());
	c->setLowerLimit(getLowerLimit());
	c->setUpperLimit(getUpperLimit());
	c->setMotorEnabled(isMotorEnabled());
	c->setMotorSpeed(getMotorSpeed());
	c->setMotorTorgue(getMotorTorgue());
}

template <class Archive>
void RevoluteJoint::serialize(Archive& ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Component>(*this);

	ar & boost::serialization::make_nvp("bindingA", m_bindingA);
	ar & boost::serialization::make_nvp("bindingB", m_bindingB);

	ar & boost::serialization::make_nvp("jointDef", m_jointDef);
}

DECLARE_BINARY_SERIALIZE(RevoluteJoint)

BOOST_CLASS_EXPORT_IMPLEMENT(RevoluteJoint)
