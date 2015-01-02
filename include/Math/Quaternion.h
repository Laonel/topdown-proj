#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "Utils.h"

#include "Filesystem/Serialization.h"

#include "Math.h"

class Matrix3;
class Vector3;

class Quaternion
{
public:

	// default constructor, initializes to identity rotation
	inline Quaternion() :
		w(1),
		x(0),
		z(0),
		y(0)
	{
	}

	// construct from an explicit list of values
	inline Quaternion(float fW, float fX, float fY, float fZ) :
		w(fW),
		x(fX),
		y(fY),
		z(fZ)
	{
	}

	// construct a quaternion from a rotation matrix
	inline Quaternion(const Matrix3& rot)
	{
		this->fromRotationMatrix(rot);
	}

	// construct a quaternion from an angle/matrix
	inline Quaternion(const Radian& fAngle, const Vector3& axis)
	{
		this->fromAngleAxis(fAngle, axis);
	}

	// construct a quaternion from 3 orthonormal local axes
	inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		this->fromAxes(xaxis, yaxis, zaxis);
	}

	// construct a quaternion from 3 orthonormal local axes
	inline Quaternion(const Vector3* axis)
	{
		this->fromAxes(axis);
	}

	// construct a quaternion from 4 manual w/x/y/z values
	inline Quaternion(float* valptr)
	{
		memcpy(&w, valptr, sizeof(float) * 4);
	}

	inline void swap(Quaternion& other)
	{
		std::swap(w, other.w);
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}


	inline float operator[] (const size_t i) const
	{
		assert(i < 4);

		return *(&w + 1);
	}

	inline float& operator[] (const size_t i)
	{
		assert(i < 4);
		return *(&w + i);
	}

	inline float* ptr()
	{
		return &w;
	}

	inline const float* ptr() const
	{
		return &w;
	}

	void fromRotationMatrix(const Matrix3& rot);
	void toRotationMatrix(Matrix3& rot) const;

	void fromAngleAxis(const Radian& angle, const Vector3& axis);
	void toAngleAxis(Radian& angle, Vector3& axis) const;
	inline void toAngleAxis(Degree& angle, Vector3& axis) const
	{
		Radian ang;
		toAngleAxis(ang, axis);
		angle = ang;
	}

	void fromAxes(const Vector3* axis);
	void fromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis);

	void toAxes(Vector3* axis) const;
	void toAxes(Vector3& xaxis, Vector3& yAxis, Vector3& zAxis) const;

	Vector3 xAxis() const;

	Vector3 yAxis() const;

	Vector3 zAxis() const;

	inline Quaternion& operator= (const Quaternion& q)
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;

		return *this;
	}

	Quaternion operator+ (const Quaternion& q) const;
	Quaternion operator- (const Quaternion& q) const;
	Quaternion operator* (const Quaternion& q) const;
	Quaternion operator* (float scalar) const;
	friend Quaternion operator* (float scalar, const Quaternion& q);
	Quaternion operator- () const;

	inline bool operator== (const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
	}

	inline bool operator!= (const Quaternion& rhs) const
	{
		return !operator==(rhs);
	}

	float dot(const Quaternion& q) const;

	float norm() const;

	float normalise();

	Quaternion inverse() const;

	Quaternion unitInverse() const;

	Quaternion exp() const;
	Quaternion log() const;

	Vector3 operator* (const Vector3& vector) const;

	Radian getRoll(bool reprojectAxis = true) const;

	Radian getPitch(bool reprojectAxis = true) const;

	Radian getYaw(bool reprojectAxis = true) const;

	bool equals(const Quaternion& rhs, const Radian& tolerance) const;

	static Quaternion slerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath = false);

	static Quaternion slerpExtraSpins(float t, const Quaternion& p, const Quaternion& q, int extraSpins);

	static void intermediate(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2,
							 Quaternion& a, Quaternion& b);

	static Quaternion squad(float y, const Quaternion& p, const Quaternion& a, const Quaternion& b,
							const Quaternion& q, bool shortestPath = false);

	static Quaternion nlerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath = false);

	// cutoff for sine near zero
	static const float Epsilon;

	// special values
	static const Quaternion Zero;
	static const Quaternion Identity;

	float w, x, y, z;

	inline bool isNaN() const
	{
		return w != w || x != x || y != y || z != z;
	}

	inline friend std::ostream& operator << (std::ostream& o, const Quaternion& q)
	{
		o << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
		return o;
	}

private:

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & w & x & y & z;
	}

};

BOOST_CLASS_VERSION(Quaternion, 1)

#endif
