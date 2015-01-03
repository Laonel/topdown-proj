#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/extended_type_info.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>

#include <Box2D/Dynamics/b2Fixture.h>

#include "Utils.h"
//#include "Defs.h"

#define DECLARE_BINARY_SAVE(T) template void T::save<eos::portable_oarchive>	\
	(eos::portable_oarchive& ar, const unsigned int version) const;

#define DECLARE_BINARY_LOAD(T) template void T::load<eos::portable_iarchive>	\
	(eos::portable_iarchive& ar, const unsigned int version);

#define DECLARE_BINARY_SERIALIZE(T)																				\
	template void T::serialize<eos::portable_oarchive>(eos::portable_oarchive& ar, const unsigned int version);	\
	template void T::serialize<eos::portable_iarchive>(eos::portable_iarchive& ar, const unsigned int version);

#define DECLARE_POLYMORPHIC_SAVE(T) template void T::save<boost::archive::polymorphic_oarchive>	\
	(boost::archive::polymorphic_oarchive&, const unsigned int version) const;

#define DECLARE_POLYMORPHIC_LOAD(T) template void T::load<boost::archive::polymorphic_iarchive>	\
	(boost::archive::polymorphic_iarchive&, const unsigned int version);

#define DECLARE_POLYMORPHIC_SERIALIZE(T)												\
	template void T::serialize<boost::archive::polymorphic_iarchive>					\
		(boost::archive::polymorphic_iarchive& ar, const unsigned int version);			\
	template void T::serialize<boost::archive::polymorphic_iarchive>					\
		(boost::archive::polymorphic_iarchive& ar, const unsigned int version);


namespace boost {
	namespace serialization {

// Serialization Vector2 float
template <class Archive>
inline void serialize(Archive& ar, sf::Vector2f& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
}

// Serialization Vector2 int
template <class Archive>
inline void serialize(Archive& ar, sf::Vector2i& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
}

// Serialization Vector2 unsigned int
template <class Archive>
inline void serialize(Archive& ar, sf::Vector2u& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
}

// Serialization Vector3 float
template <class Archive>
inline void serialize(Archive& ar, sf::Vector3f& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
	ar & BOOST_SERIALIZATION_NVP(v.z);
}

// Serialization Vector3 int
template <class Archive>
inline void serialize(Archive& ar, sf::Vector3i v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
	ar & BOOST_SERIALIZATION_NVP(v.z);
}

// Serialization Color
template <class Archive>
inline void serialize(Archive& ar, sf::Color& c, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(c.r);
	ar & BOOST_SERIALIZATION_NVP(c.g);
	ar & BOOST_SERIALIZATION_NVP(c.b);
	ar & BOOST_SERIALIZATION_NVP(c.a);
}

// Serialization Int Rect
template <class Archive>
inline void serialize(Archive& ar, sf::IntRect& r, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(r.left);
	ar & BOOST_SERIALIZATION_NVP(r.top);
	ar & BOOST_SERIALIZATION_NVP(r.width);
	ar & BOOST_SERIALIZATION_NVP(r.height);
}

// Serialization Float Rect
template <class Archive>
inline void serialize(Archive& ar, sf::FloatRect& r, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(r.left);
	ar & BOOST_SERIALIZATION_NVP(r.top);
	ar & BOOST_SERIALIZATION_NVP(r.width);
	ar & BOOST_SERIALIZATION_NVP(r.height);
}

// Serialization Vertex
template <class Archive>
inline void serialize(Archive& ar, sf::Vertex& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.position);
	ar & BOOST_SERIALIZATION_NVP(v.texCoords);
	ar & BOOST_SERIALIZATION_NVP(v.color);
}

template <class Archive>
inline void serialize(Archive& ar, b2Vec2& v, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(v.x);
	ar & BOOST_SERIALIZATION_NVP(v.y);
}

// Serialization Shape Base
template <class Archive>
inline void serialize(Archive& ar, b2Shape& s, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(s.m_type);
	ar & BOOST_SERIALIZATION_NVP(s.m_radius);
}

template <class Archive>
inline void serialize(Archive& ar, b2CircleShape& s, const unsigned int version)
{
	ar & boost::serialization::base_object<b2Shape>(s);

	ar & BOOST_SERIALIZATION_NVP(s.m_p);
}

template <class Archive>
inline void serialize(Archive& ar, b2ChainShape& s, const unsigned int version)
{
	ar & boost::serialization::base_object<b2Shape>(s);
}

template <class Archive>
inline void serialize(Archive& ar, b2EdgeShape& s, const unsigned int version)
{
	ar & boost::serialization::base_object<b2Shape>(s);
}

template <class Archive>
inline void serialize(Archive& ar, b2PolygonShape& s, const unsigned int version)
{
	ar & boost::serialization::base_object<b2Shape>(s);
}

// Serialization Collision Filter
template <class Archive>
inline void serialize(Archive& ar, b2Filter& f, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(f.categoryBits);
	ar & BOOST_SERIALIZATION_NVP(f.maskBits);
	ar & BOOST_SERIALIZATION_NVP(f.groupIndex);
}

template <class Archive>
inline void serialize(Archive& ar, b2BodyDef& b, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(b.active);
	ar & BOOST_SERIALIZATION_NVP(b.allowSleep);
	ar & BOOST_SERIALIZATION_NVP(b.angle);
	ar & BOOST_SERIALIZATION_NVP(b.angularDamping);
	ar & BOOST_SERIALIZATION_NVP(b.angularVelocity);
	ar & BOOST_SERIALIZATION_NVP(b.awake);
	ar & BOOST_SERIALIZATION_NVP(b.bullet);
	ar & BOOST_SERIALIZATION_NVP(b.fixedRotation);
	ar & BOOST_SERIALIZATION_NVP(b.gravityScale);
	ar & BOOST_SERIALIZATION_NVP(b.linearDamping);
	ar & BOOST_SERIALIZATION_NVP(b.linearVelocity);
	ar & BOOST_SERIALIZATION_NVP(b.position);
	ar & BOOST_SERIALIZATION_NVP(b.type);
}

// Serialization FixtureDef
template <class Archive>
inline void serialize(Archive& ar, b2FixtureDef& f, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(f.friction);
	ar & BOOST_SERIALIZATION_NVP(f.restitution);
	ar & BOOST_SERIALIZATION_NVP(f.density);
	ar & BOOST_SERIALIZATION_NVP(f.isSensor);
	ar & BOOST_SERIALIZATION_NVP(f.filter);
}

// Serialization Joints Defs
template <class Archive>
inline void serialize(Archive& ar, b2JointDef& j, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(j.collideConnected);
	ar & BOOST_SERIALIZATION_NVP(j.type);
}

template <class Archive>
inline void serialize(Archive& ar, b2RevoluteJointDef& j, const unsigned int version)
{
	ar & boost::serialization::base_object<b2JointDef>(j);

	ar & BOOST_SERIALIZATION_NVP(j.enableLimit);
	ar & BOOST_SERIALIZATION_NVP(j.enableMotor);
	ar & BOOST_SERIALIZATION_NVP(j.localAnchorA);
	ar & BOOST_SERIALIZATION_NVP(j.localAnchorB);
	ar & BOOST_SERIALIZATION_NVP(j.lowerAngle);
	ar & BOOST_SERIALIZATION_NVP(j.maxMotorTorque);
	ar & BOOST_SERIALIZATION_NVP(j.motorSpeed);
	ar & BOOST_SERIALIZATION_NVP(j.referenceAngle);
	ar & BOOST_SERIALIZATION_NVP(j.upperAngle);
}

template <class Archive>
inline void serialize(Archive& ar, b2DistanceJointDef& j, const unsigned int version)
{
	ar & boost::serialization::base_object<b2JointDef>(j);

	ar & BOOST_SERIALIZATION_NVP(j.localAnchorA);
	ar & BOOST_SERIALIZATION_NVP(j.localAnchorB);
	ar & BOOST_SERIALIZATION_NVP(j.length);
	ar & BOOST_SERIALIZATION_NVP(j.frequencyHz);
	ar & BOOST_SERIALIZATION_NVP(j.dampingRatio);
}

template <class Archive>
inline void serialize(Archive& ar, b2PrismaticJointDef& j, const unsigned int version)
{
	ar & boost::serialization::base_object<b2JointDef>(j);

	ar & BOOST_SERIALIZATION_NVP(j.localAnchorA);
	ar & BOOST_SERIALIZATION_NVP(j.localAnchorB);
	ar & BOOST_SERIALIZATION_NVP(j.localAxisA);
	ar & BOOST_SERIALIZATION_NVP(j.referenceAngle);
	ar & BOOST_SERIALIZATION_NVP(j.enableLimit);
	ar & BOOST_SERIALIZATION_NVP(j.enableMotor);
	ar & BOOST_SERIALIZATION_NVP(j.lowerTranslation);
	ar & BOOST_SERIALIZATION_NVP(j.upperTranslation);
	ar & BOOST_SERIALIZATION_NVP(j.motorSpeed);
	ar & BOOST_SERIALIZATION_NVP(j.maxMotorForce);
}

template <class Archive>
inline void serialize(Archive& ar, b2WheelJointDef& j, const unsigned int version)
{
	ar & boost::serialization::base_object<b2JointDef>(j);

	ar & BOOST_SERIALIZATION_NVP(j.localAnchorA);
	ar & BOOST_SERIALIZATION_NVP(j.localAnchorB);
	ar & BOOST_SERIALIZATION_NVP(j.localAxisA);
	ar & BOOST_SERIALIZATION_NVP(j.enableMotor);
	ar & BOOST_SERIALIZATION_NVP(j.maxMotorTorque);
	ar & BOOST_SERIALIZATION_NVP(j.motorSpeed);
	ar & BOOST_SERIALIZATION_NVP(j.frequencyHz);
	ar & BOOST_SERIALIZATION_NVP(j.dampingRatio);
}

template <class Archive>
inline void serialize(Archive& ar, b2MotorJointDef& j, const unsigned int version)
{
	ar & boost::serialization::base_object<b2JointDef>(j);

	ar & BOOST_SERIALIZATION_NVP(j.linearOffset);
	ar & BOOST_SERIALIZATION_NVP(j.angularOffset);
	ar & BOOST_SERIALIZATION_NVP(j.maxForce);
	ar & BOOST_SERIALIZATION_NVP(j.maxTorque);
	ar & BOOST_SERIALIZATION_NVP(j.correctionFactor);
}

// Serialization BoundingBox
template <class Archive>
inline void serialize(Archive& ar, b2AABB& f, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(f.lowerBound.x) & BOOST_SERIALIZATION_NVP(f.lowerBound.y);
	ar & BOOST_SERIALIZATION_NVP(f.upperBound.x) & BOOST_SERIALIZATION_NVP(f.upperBound.y);
}

} // namespace serialization
} // namespace boost

#endif
