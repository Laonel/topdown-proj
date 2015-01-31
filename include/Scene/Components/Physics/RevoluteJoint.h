#ifndef _REVOLUTE_JOINT_H_
#define _REVOLUTE_JOINT_H_

#include "Utils.h"

#include "Scene/Component.h"

#include <Box2D/Box2D.h>

class RevoluteJoint : public Component
{
public:

	inline static Component* onBuildComponent() { return new RevoluteJoint(); }

	RevoluteJoint();

	virtual ~RevoluteJoint();

	inline b2RevoluteJoint* getJoint() { return m_joint; }

	inline b2RevoluteJointDef getJointDef() { return m_jointDef; }

	std::string getBindingA();

	void setBindingA(std::string str);

	std::string getBindingB();

	void setBindingB(std::string str);

	bool getCollideConnected();

	void setCollideConnected(bool value);

	float getReferenceAngle();

	void setReferenceAngle(float angle);

	b2Vec2 getLocalAnchorA();

	void setLocalAnchorA(b2Vec2 anchor);

	b2Vec2 getLocalAnchorB();

	void setLocalAnchorB(b2Vec2 anchor);

	bool isLimitEnabled();

	void setLimitEnabled(bool value);

	float getLowerLimit();

	void setLowerLimit(float limit);

	float getUpperLimit();

	void setUpperLimit(float limit);

	bool isMotorEnabled();

	void setMotorEnabled(bool value);

	float getMotorSpeed();

	void setMotorSpeed(float speed);

	float getMotorTorgue();

	void setMotorTorgue(float torgue);

protected:

	virtual void onCreate();
	virtual void onDestroy();
	virtual void onDuplicate(Component* dest);

private:

	std::string m_bindingA;
	std::string m_bindingB;

	b2RevoluteJoint* m_joint;
	b2RevoluteJointDef m_jointDef;

private:

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int version);

};

BOOST_CLASS_VERSION(RevoluteJoint, 1)

BOOST_CLASS_EXPORT_KEY(RevoluteJoint)

#endif
