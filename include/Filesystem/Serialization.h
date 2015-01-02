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

//#include <Box2D/Dynamics/b2Fixture.h>

#include "Utils.h"
//#include "Defs.h"
/*
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

*/
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

}	// namespace serialization
}	// namespace boost

#endif
