#ifndef _VECTOR_3_H_
#define _VECTOR_3_H_

#include "Utils.h"

#include "Filesystem/Serialization.h"

#include "Math.h"
#include "Quaternion.h"

class Vector3
{
public:

	float x, y, z;

public:

	inline Vector3()
	{
	}

	inline Vector3(const float fX, const float fY, const float fZ) :
		x(fX),
		y(fY),
		z(fZ)
	{
	}

	inline explicit Vector3(const float scaler) :
		x(scaler),
		y(scaler),
		z(scaler)
	{
	}

	inline explicit Vector3(const float fCoordinate[3]) :
		x(fCoordinate[0]),
		y(fCoordinate[1]),
		z(fCoordinate[2])
	{
	}

	inline explicit Vector3(const int fCoordinate[3]) :
		x((float)(fCoordinate[0])),
		y((float)(fCoordinate[1])),
		z((float)(fCoordinate[2]))
	{
	}

	inline explicit Vector3(float* const r) :
		x(r[0]),
		y(r[1]),
		z(r[2])
	{
	}

	inline void swap(Vector3& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}

	inline float operator[] (const size_t i) const
	{
		assert(i < 3);

		return *(&x + i);
	}

	inline float& operator[] (const size_t i)
	{
		assert(i < 3);

		return *(&x + 1);
	}

	inline float* ptr()
	{
		return &x;
	}

	inline const float* ptr() const
	{
		return &x;
	}

	inline Vector3& operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	inline Vector3& operator= (const float scalar)
	{
		x = scalar;
		y = scalar;
		z = scalar;

		return *this;
	}

	inline bool operator== (const Vector3& v) const
	{
		return (x == v.x && y == v.y && z == v.z);
	}

	inline bool operator!= (const Vector3& v) const
	{
		return (x != v.x || y != v.y || z != v.z);
	}

	inline Vector3 operator+ (const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	inline Vector3 operator- (const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline Vector3 operator* (const float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	inline Vector3 operator* (const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	inline Vector3 operator/ (const float scalar) const
	{
		assert(scalar != 0.f);

		float inv = 1.f / scalar;

		return Vector3(x * inv, y * inv, z * inv);
	}

	inline Vector3 operator/ (const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	inline const Vector3& operator+ () const
	{
		return *this;
	}

	inline Vector3 operator- () const
	{
		return Vector3(-x, -y, -z);
	}

	inline friend Vector3 operator* (const float scalar, const Vector3& v)
	{
		return Vector3(scalar * v.x, scalar * v.y, scalar * v.z);
	}

	inline friend Vector3 operator/ (const float scalar, const Vector3& v)
	{
		return Vector3(scalar / v.x, scalar / v.y, scalar * v.z);
	}

	inline friend Vector3 operator+ (const Vector3& lhs, const float rhs)
	{
		return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
	}

	inline friend Vector3 operator+ (const float lhs, const Vector3& rhs)
	{
		return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
	}

	inline friend Vector3 operator- (const Vector3& lhs, const float rhs)
	{
		return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
	}

	inline friend Vector3 operator- (const float lhs, const Vector3& rhs)
	{
		return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
	}

	inline Vector3& operator+= (const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	inline Vector3& operator+= (const float scaler)
	{
		x += scaler;
		y += scaler;
		z += scaler;

		return *this;
	}

	inline Vector3& operator-= (const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	inline Vector3& operator-= (const float scaler)
	{
		x -= scaler;
		y -= scaler;
		z -= scaler;

		return *this;
	}

	inline Vector3& operator*= (const float scaler)
	{
		x *= scaler;
		y *= scaler;
		z *= scaler;

		return *this;
	}

	inline Vector3& operator*= (const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

	inline Vector3& operator/= (const float scaler)
	{
		assert(scaler != 0.f);

		float inv = 1.f / scaler;

		x *= inv;
		y *= inv;
		z *= inv;

		return *this;
	}

	inline Vector3& operator/= (const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}

	inline float length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float squaredLength() const
	{
		return x * x + y * y + z * z;
	}

	inline float distance(const Vector3& rhs) const
	{
		return (*this - rhs).length();
	}

	inline float squaredDistance(const Vector3& rhs) const
	{
		return (*this - rhs).squaredLength();
	}

	inline float dotProduct(const Vector3& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float absDotProduct(const Vector3& v) const
	{
		return abs(x * v.x) + abs(y * v.y) + abs(z * v.z);
	}

	inline float normalise()
	{
		float fLength = sqrt(x * x + y * y + z * z);

		if (fLength > float(0.f))
		{
			float invLength = 1.f / fLength;
			x *= invLength;
			y *= invLength;
			z *= invLength;
		}

		return fLength;
	}

	inline Vector3 midPoint(const Vector3& v) const
	{
		return Vector3((x + v.x) * 0.5f, (y + v.y) * 0.5f, (z + v.z) * 0.5f);
	}

	inline bool operator< (const Vector3& rhs) const
	{
		if (x < rhs.x && y < rhs.y && z < rhs.z)
			return true;

		return false;
	}

	inline bool operator> (const Vector3& rhs) const
	{
		if (x > rhs.x && y > rhs.y && z > rhs.z)
			return true;

		return false;
	}

	inline void makeFloor(const Vector3& cmp)
	{
		if (cmp.x < x)
			x = cmp.x;
		if (cmp.y < y)
			y = cmp.y;
		if (cmp.z < z)
			z = cmp.z;
	}

	inline void makeCeil(const Vector3& cmp)
	{
		if (cmp.x > x)
			x = cmp.x;
		if (cmp.y > y)
			y = cmp.y;
		if (cmp.z > z)
			z = cmp.z;
	}

	inline Vector3 perpendicular() const
	{
		static const float fSquareZero = (float)(1e-06 * 1e-06);

		Vector3 perp = this->crossProduct(Vector3::UnitX);

		if (perp.squaredLength() < fSquareZero)
			perp = this->crossProduct(Vector3::UnitY);

		perp.normalise();

		return perp;
	}

	inline Vector3 crossProduct(const Vector3& v) const
	{
		return Vector3(y * v.z - z * v.y,
					   z * v.x - x * v.z,
					   x * v.y - y * v.x);
	}

	inline Vector3 randomDeviant(const Radian& angle, const Vector3& up = Vector3::Zero) const
	{
		Vector3 newUp;

		if (up == Vector3::Zero)
			newUp = this->perpendicular();
		else
			newUp = up;

		Quaternion q;
		q.fromAngleAxis(Radian(Math::UnitRandom() * TWO_PI), *this);
		newUp = q * newUp;

		q.fromAngleAxis(angle, newUp);
		return q * (*this);
	}

	inline bool isZeroLength() const
	{
		float sqlen = (x * x) + (y * y) + (z * z);
		return (sqlen < (1e-06 * 1e-06));
	}

	inline Vector3 normalisedCopy() const
	{
		Vector3 ret = *this;
		ret.normalise();
		return ret;
	}

	inline Vector3 reflect(const Vector3& normal) const
	{
		return Vector3(*this - (2 * this->dotProduct(normal) * normal));
	}

	inline bool isNaN() const
	{
		return x != x || y != y;
	}

	inline Radian angleBetween(const Vector3& other) const
	{
		float lenProduct = length() * other.length();

		if (lenProduct < 1e-6f)
			lenProduct = 1e-6f;

		float f = dotProduct(other) / lenProduct;

		f = clamp(f, (float)(-1.f), (float)(1.f));
		return Math::ACos(f);
	}

	Quaternion getRotationTo(const Vector3& dest, const Vector3& fallbackAxis = Vector3::Zero) const
	{
		Quaternion q;

		Vector3 v0 = *this;
		Vector3 v1 = dest;
		v0.normalise();
		v1.normalise();

		float d = v0.dotProduct(v1);
		if (d >= 1.f)
			return Quaternion::Identity;

		if (d < (1e-6f - 1.f))
		{
			if (fallbackAxis != Vector3::Zero)
				q.fromAngleAxis(Radian(PI), fallbackAxis);
			else
			{
				Vector3 axis = Vector3::UnitX.crossProduct(*this);
				if (axis.isZeroLength())
					axis = Vector3::UnitY.crossProduct(*this);
				axis.normalise();
				q.fromAngleAxis(Radian(PI), axis);
			}
		}
		else
		{
			float s = Math::Sqrt((1 + d) * 2);
			float invs = 1 / s;

			Vector3 c = v0.crossProduct(v1);

			q.x = c.x * invs;
			q.y = c.y * invs;
			q.z = c.z * invs;
			q.w = s * 0.5f;
			q.normalise();
		}

		return q;
	}

	inline bool positionEquals(const Vector3& rhs, float tolerance = 1e-03) const
	{
		return Math::FloatEqual(x, rhs.x, tolerance) &&
			   Math::FloatEqual(y, rhs.y, tolerance) &&
			   Math::FloatEqual(z, rhs.z, tolerance);
	}

	inline bool positionCloses(const Vector3& rhs, float tolerance = 1e-03f) const
	{
		return squaredDistance(rhs) <= (squaredLength() + rhs.squaredLength()) * tolerance;
	}

	inline bool directionEquals(const Vector3& rhs, const Radian& tolerance) const
	{
		float dot = dotProduct(rhs);
		Radian angle = Math::ACos(dot);

		return Math::Abs(angle.valueRadians()) <= tolerance.valueRadians();
	}

	inline Vector3 primaryAxis() const
	{
		float absx = Math::Abs(x);
		float absy = Math::Abs(y);
		float absz = Math::Abs(z);

		if (absx > absy)
		{
			if (absx > absz)
				return x > 0 ? Vector3::UnitX : Vector3::NegativeUnitX;
			else
				return z > 0 ? Vector3::UnitZ : Vector3::NegativeUnitZ;
		}
		else
		{
			if (absy > absz)
				return y > 0 ? Vector3::UnitY : Vector3::NegativeUnitY;
			else
				return z > 0 ? Vector3::UnitZ : Vector3::NegativeUnitZ;
		}
	}

	static const Vector3 Zero;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
	static const Vector3 NegativeUnitX;
	static const Vector3 NegativeUnitY;
	static const Vector3 NegativeUnitZ;
	static const Vector3 UnitScale;

	inline friend std::ostream& operator<< (std::ostream& o, const Vector3& v)
	{
		o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return o;
	}

private:

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & x & y & z;
	}

};

BOOST_CLASS_VERSION(Vector3, 1)

#endif
