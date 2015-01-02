#ifndef _VECTOR_2_H_
#define _VECTOR_2_H_

#include "../Utils.h"

#include "Filesystem/Serialization.h"

#include "Math.h"

class Vector2
{
public:

	float x, y;

public:

	inline Vector2()
	{
	}

	inline Vector2(const float fX, const float fY) :
		x(fX),
		y(fY)
	{
	}

	inline explicit Vector2(const float scaler) :
		x(scaler),
		y(scaler)
	{
	}

	inline explicit Vector2(const float fCoordinate[2]) :
		x(fCoordinate[0]),
		y(fCoordinate[1])
	{
	}

	inline explicit Vector2(const int fCoordinate[2]) :
		x((float)(fCoordinate[0])),
		y((float)(fCoordinate[1]))
	{
	}

	inline explicit Vector2(float* const r) :
		x(r[0]),
		y(r[1])
	{
	}

	inline void swap(Vector2& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
	}

	inline float operator[] (const size_t i) const
	{
		assert(i < 2);

		return *(&x + i);
	}

	inline float& operator[] (const size_t i)
	{
		assert(i < 2);

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

	inline Vector2& operator= (const Vector2& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	inline Vector2& operator= (const float scalar)
	{
		x = scalar;
		y = scalar;

		return *this;
	}

	inline bool operator== (const Vector2& v) const
	{
		return (x == v.x && y == v.y);
	}

	inline bool operator!= (const Vector2& v) const
	{
		return (x != v.x || y != v.y);
	}

	inline Vector2 operator+ (const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	inline Vector2 operator- (const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	inline Vector2 operator* (const float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	inline Vector2 operator* (const Vector2& v) const
	{
		return Vector2(x * v.x, y * v.y);
	}

	inline Vector2 operator/ (const float scalar) const
	{
		assert(scalar != 0.f);

		float inv = 1.f / scalar;

		return Vector2(x * inv, y * inv);
	}

	inline Vector2 operator/ (const Vector2& v) const
	{
		return Vector2(x / v.x, y / v.y);
	}

	inline const Vector2& operator+ () const
	{
		return *this;
	}

	inline Vector2 operator- () const
	{
		return Vector2(-x, -y);
	}

	inline friend Vector2 operator* (const float scalar, const Vector2& v)
	{
		return Vector2(scalar * v.x, scalar * v.y);
	}

	inline friend Vector2 operator/ (const float scalar, const Vector2& v)
	{
		return Vector2(scalar / v.x, scalar / v.y);
	}

	inline friend Vector2 operator+ (const Vector2& lhs, const float rhs)
	{
		return Vector2(lhs.x + rhs, lhs.y + rhs);
	}

	inline friend Vector2 operator+ (const float lhs, const Vector2& rhs)
	{
		return Vector2(lhs + rhs.x, lhs + rhs.y);
	}

	inline friend Vector2 operator- (const Vector2& lhs, const float rhs)
	{
		return Vector2(lhs.x - rhs, lhs.y - rhs);
	}

	inline friend Vector2 operator- (const float lhs, const Vector2& rhs)
	{
		return Vector2(lhs - rhs.x, lhs - rhs.y);
	}

	inline Vector2& operator+= (const Vector2& v)
	{
		x += v.x;
		y += v.y;

		return *this;
	}

	inline Vector2& operator+= (const float scaler)
	{
		x += scaler;
		y += scaler;

		return *this;
	}

	inline Vector2& operator-= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

	inline Vector2& operator-= (const float scaler)
	{
		x -= scaler;
		y -= scaler;

		return *this;
	}

	inline Vector2& operator*= (const float scaler)
	{
		x *= scaler;
		y *= scaler;

		return *this;
	}

	inline Vector2& operator*= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;

		return *this;
	}

	inline Vector2& operator/= (const float scaler)
	{
		assert(scaler != 0.f);

		float inv = 1.f / scaler;

		x *= inv;
		y *= inv;

		return *this;
	}

	inline Vector2& operator/= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;

		return *this;
	}

	inline float length() const
	{
		return sqrt(x * x + y * y);
	}

	inline float squaredLength() const
	{
		return x * x + y * y;
	}

	inline float distance(const Vector2& rhs) const
	{
		return (*this - rhs).length();
	}

	inline float squaredDistance(const Vector2& rhs) const
	{
		return (*this - rhs).squaredLength();
	}

	inline float dotProduct(const Vector2& v) const
	{
		return x * v.x + y * v.y;
	}

	inline float normalise()
	{
		float fLength = sqrt(x * x + y * y);

		if (fLength > float(0.f))
		{
			float invLength = 1.f / fLength;
			x *= invLength;
			y *= invLength;
		}

		return fLength;
	}

	inline Vector2 midPoint(const Vector2& v) const
	{
		return Vector2((x + v.x) * 0.5f, (y + v.y) * 0.5f);
	}

	inline bool operator< (const Vector2& rhs) const
	{
		if (x < rhs.x && y < rhs.y)
			return true;

		return false;
	}

	inline bool operator> (const Vector2& rhs) const
	{
		if (x > rhs.x && y > rhs.y)
			return true;

		return false;
	}

	inline void makeFloor(const Vector2& cmp)
	{
		if (cmp.x < x)
			x = cmp.x;
		if (cmp.y < y)
			y = cmp.y;
	}

	inline void makeCeil(const Vector2& cmp)
	{
		if (cmp.x > x)
			x = cmp.x;
		if (cmp.y > y)
			y = cmp.y;
	}

	inline Vector2 perpendicular() const
	{
		return Vector2(-y, x);
	}

	inline float crossProduct(const Vector2& v) const
	{
		return x * v.y - y * v.x;
	}

	inline Vector2 randomDeviant(float angle) const
	{
		angle *= (rand() / (float)(RAND_MAX)) * TWO_PI;
		float cosa = cos(angle);
		float sina = sin(angle);
		return Vector2(cosa * x - sina * y, sina * x + cosa * y);
	}

	inline bool isZeroLength() const
	{
		float sqlen = (x * x) + (y * y);
		return (sqlen < (1e-06 * 1e-06));
	}

	inline Vector2 normalisedCopy() const
	{
		Vector2 ret = *this;
		ret.normalise();
		return ret;
	}

	inline Vector2 reflect(const Vector2& normal) const
	{
		return Vector2(*this - (2 * this->dotProduct(normal) * normal));
	}

	inline bool isNaN() const
	{
		return x != x || y != y;
	}

	inline Radian angleBetween(const Vector2& other) const
	{
		float lenProduct = length() * other.length();

		if (lenProduct < 1e-6f)
			lenProduct = 1e-6f;

		float f = dotProduct(other) / lenProduct;

		f = clamp(f, (float)(-1.f), (float)(1.f));
		return Math::ACos(f);
	}

	inline Radian angleTo(const Vector2& other) const
	{
		Radian angle = angleBetween(other);

		if (crossProduct(other) < 0)
			angle = (Radian)(TWO_PI - angle.valueRadians());

		return angle;
	}

	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegativeUnitX;
	static const Vector2 NegativeUnitY;
	static const Vector2 UnitScale;

	inline friend std::ostream& operator<< (std::ostream& o, const Vector2& v)
	{
		o << "Vector2(" << v.x << ", " << v.y << ")";
		return o;
	}

private:

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & x & y;
	}

};

BOOST_CLASS_VERSION(Vector2, 1)

#endif
