#ifndef _MATH_H_
#define _MATH_H_

#include "Utils.h"

class Degree;
class Vector2;

class Radian
{
	float m_rad;

public:

	explicit Radian(float r = 0) :
		m_rad(r)
	{
	}

	Radian(const Degree& d);

	Radian& operator= (const float& f) { m_rad = f; return *this; }
	Radian& operator= (const Radian& r) { m_rad = r.m_rad; return *this; }
	Radian& operator= (const Degree& d);

	float valueDegrees() const;
	float valueRadians() const { return m_rad; }
	float valueAngleUnits() const;

	const Radian& operator+ () const { return *this; }
	Radian operator + ( const Radian& r ) const { return Radian ( m_rad + r.m_rad ); }
	Radian operator + ( const Degree& d ) const;
	Radian& operator += ( const Radian& r ) { m_rad += r.m_rad; return *this; }
	Radian& operator += ( const Degree& d );
	Radian operator - () const { return Radian(-m_rad); }
	Radian operator - ( const Radian& r ) const { return Radian ( m_rad - r.m_rad ); }
	Radian operator - ( const Degree& d ) const;
	Radian& operator -= ( const Radian& r ) { m_rad -= r.m_rad; return *this; }
	Radian& operator -= ( const Degree& d );
	Radian operator * ( float f ) const { return Radian ( m_rad * f ); }
    Radian operator * ( const Radian& f ) const { return Radian ( m_rad * f.m_rad ); }
	Radian& operator *= ( float f ) { m_rad *= f; return *this; }
	Radian operator / ( float f ) const { return Radian ( m_rad / f ); }
	Radian& operator /= ( float f ) { m_rad /= f; return *this; }

	bool operator <  ( const Radian& r ) const { return m_rad <  r.m_rad; }
	bool operator <= ( const Radian& r ) const { return m_rad <= r.m_rad; }
	bool operator == ( const Radian& r ) const { return m_rad == r.m_rad; }
	bool operator != ( const Radian& r ) const { return m_rad != r.m_rad; }
	bool operator >= ( const Radian& r ) const { return m_rad >= r.m_rad; }
	bool operator >  ( const Radian& r ) const { return m_rad >  r.m_rad; }

	inline friend std::ostream& operator<< (std::ostream& o, const Radian& v)
	{
		o << "Radian(" << v.valueRadians() << ")";
		return o;
	}

};

class Degree
{
	float m_deg;

public:

	explicit Degree ( float d=0 ) : m_deg(d) {}
	Degree ( const Radian& r ) : m_deg(r.valueDegrees()) {}

	Degree& operator = ( const float& f ) { m_deg = f; return *this; }
	Degree& operator = ( const Degree& d ) { m_deg = d.m_deg; return *this; }
	Degree& operator = ( const Radian& r ) { m_deg = r.valueDegrees(); return *this; }

	float valueDegrees() const { return m_deg; }
	float valueRadians() const; // see bottom of this file
	float valueAngleUnits() const;

	const Degree& operator + () const { return *this; }
	Degree operator + ( const Degree& d ) const { return Degree ( m_deg + d.m_deg ); }
	Degree operator + ( const Radian& r ) const { return Degree ( m_deg + r.valueDegrees() ); }
	Degree& operator += ( const Degree& d ) { m_deg += d.m_deg; return *this; }
	Degree& operator += ( const Radian& r ) { m_deg += r.valueDegrees(); return *this; }
	Degree operator - () const { return Degree(-m_deg); }
	Degree operator - ( const Degree& d ) const { return Degree ( m_deg - d.m_deg ); }
	Degree operator - ( const Radian& r ) const { return Degree ( m_deg - r.valueDegrees() ); }
	Degree& operator -= ( const Degree& d ) { m_deg -= d.m_deg; return *this; }
	Degree& operator -= ( const Radian& r ) { m_deg -= r.valueDegrees(); return *this; }
	Degree operator * ( float f ) const { return Degree ( m_deg * f ); }
    Degree operator * ( const Degree& f ) const { return Degree ( m_deg * f.m_deg ); }
	Degree& operator *= ( float f ) { m_deg *= f; return *this; }
	Degree operator / ( float f ) const { return Degree ( m_deg / f ); }
	Degree& operator /= ( float f ) { m_deg /= f; return *this; }

	bool operator <  ( const Degree& d ) const { return m_deg <  d.m_deg; }
	bool operator <= ( const Degree& d ) const { return m_deg <= d.m_deg; }
	bool operator == ( const Degree& d ) const { return m_deg == d.m_deg; }
	bool operator != ( const Degree& d ) const { return m_deg != d.m_deg; }
	bool operator >= ( const Degree& d ) const { return m_deg >= d.m_deg; }
	bool operator >  ( const Degree& d ) const { return m_deg >  d.m_deg; }

	inline friend std::ostream& operator<< (std::ostream& o, const Degree& v)
	{
		o << "Radian(" << v.valueDegrees() << ")";
		return o;
	}

};

class Angle
{
	float m_angle;

public:

	explicit Angle(float angle) : m_angle(angle) { }
	operator Radian() const;
	operator Degree() const;

};

inline Radian::Radian(const Degree& d) :
	m_rad(d.valueRadians())
{
}

inline Radian& Radian::operator = (const Degree& d)
{
	m_rad = d.valueRadians();
	return *this;
}

inline Radian Radian::operator + (const Degree& d) const
{
	return Radian(m_rad + d.valueRadians());
}

inline Radian& Radian::operator += (const Degree& d)
{
	m_rad += d.valueRadians();
	return *this;
}

inline Radian Radian::operator - ( const Degree& d ) const
{
	return Radian (m_rad - d.valueRadians());
}

inline Radian& Radian::operator -= ( const Degree& d )
{
	m_rad -= d.valueRadians();
	return *this;
}

class Math
{
public:

	enum AngleUnit
	{
		UnitDegree,
		UnitRadian
	};

protected:

	static AngleUnit s_angleUnit;

	static int s_trigTableSize;

	static float s_trigTableFactor;
	static float* s_sinTable;
	static float* s_tanTable;

	void buildTrigTables();

	static float SinTable(float fValue);
	static float TanTable(float fValue);

public:

	Math(uint32 trigTableSize = 4096);

	~Math();

	static inline int IAbs(int iValue) { return (iValue >= 0 ? iValue : -iValue); }
	static inline int ICeil (float fValue) { return int(ceil(fValue)); }
	static inline int IFloor (float fValue) { return int(floor(fValue)); }
    static int ISign (int iValue);

	static inline float Abs (float fValue) { return float(fabs(fValue)); }

	static inline Degree Abs (const Degree& dValue) { return Degree(fabs(dValue.valueDegrees())); }

	static inline Radian Abs (const Radian& rValue) { return Radian(fabs(rValue.valueRadians())); }

	static Radian ACos (float fValue);

	static Radian ASin (float fValue);

	static inline Radian ATan (float fValue) { return Radian(atan(fValue)); }

	static inline Radian ATan2 (float fY, float fX) { return Radian(atan2(fY,fX)); }

	static inline float Ceil (float fValue) { return float(ceil(fValue)); }
	static inline bool isNaN(float f)
	{
		return f != f;
	}

	static inline float Cos (const Radian& fValue, bool useTables = false)
	{
		return (!useTables) ? float(cos(fValue.valueRadians())) : SinTable(fValue.valueRadians() + HALF_PI);
	}

	static inline float Cos (float fValue, bool useTables = false)
	{
		return (!useTables) ? float(cos(fValue)) : SinTable(fValue + HALF_PI);
	}

	static inline float Exp (float fValue) { return float(exp(fValue)); }

	static inline float Floor (float fValue) { return float(floor(fValue)); }

	static inline float Log (float fValue) { return float(log(fValue)); }

	static const float LOG2;

	static inline float Log2 (float fValue) { return float(log(fValue)/LOG2); }

	static inline float LogN (float base, float fValue) { return float(log(fValue)/log(base)); }

	static inline float Pow (float fBase, float fExponent) { return float(pow(fBase,fExponent)); }

	static float Sign (float fValue);
	static inline Radian Sign ( const Radian& rValue )
	{
		return Radian(Sign(rValue.valueRadians()));
	}

	static inline Degree Sign ( const Degree& dValue )
	{
		return Degree(Sign(dValue.valueDegrees()));
	}

	static inline float Sin (const Radian& fValue, bool useTables = false)
	{
		return (!useTables) ? float(sin(fValue.valueRadians())) : SinTable(fValue.valueRadians());
	}

	static inline float Sin (float fValue, bool useTables = false)
	{
	return (!useTables) ? float(sin(fValue)) : SinTable(fValue);
	}

	static inline float Sqr (float fValue) { return fValue*fValue; }

	static inline float Sqrt (float fValue) { return float(sqrt(fValue)); }

	static inline Radian Sqrt (const Radian& fValue) { return Radian(sqrt(fValue.valueRadians())); }

	static inline Degree Sqrt (const Degree& fValue) { return Degree(sqrt(fValue.valueDegrees())); }

	static float InvSqrt (float fValue);

	static float UnitRandom ();

	static float RangeRandom (float fLow, float fHigh);

	static float SymmetricRandom ();

	static inline float Tan (const Radian& fValue, bool useTables = false)
	{
		return (!useTables) ? float(tan(fValue.valueRadians())) : TanTable(fValue.valueRadians());
	}

	static inline float Tan (float fValue, bool useTables = false)
	{
		return (!useTables) ? float(tan(fValue)) : TanTable(fValue);
	}

	static inline float DegreesToRadians(float degrees) { return degrees * fDeg2Rad; }
	static inline float RadiansToDegrees(float radians) { return radians * fRad2Deg; }

	static AngleUnit getAngleUnit();
    static void setAngleUnit(AngleUnit unit);

	static float AngleUnitsToRadians(float units);

	static float RadiansToAngleUnits(float radians);

	static float AngleUnitsToDegrees(float units);

	static float DegreesToAngleUnits(float degrees);

    static bool pointInTri2D(const Vector2& p, const Vector2& a,
							 const Vector2& b, const Vector2& c);

	static bool FloatEqual(float a, float b,
					      float tolerance = std::numeric_limits<float>::epsilon());

	static const float POS_INFINITY;
	static const float NEG_INFINITY;
	static const float fDeg2Rad;
	static const float fRad2Deg;

};

inline float Radian::valueDegrees() const
{
	return Math::RadiansToDegrees (m_rad);
}

inline float Radian::valueAngleUnits() const
{
	return Math::RadiansToAngleUnits (m_rad);
}

inline float Degree::valueRadians() const
{
	return Math::DegreesToRadians (m_deg);
}

inline float Degree::valueAngleUnits() const
{
	return Math::DegreesToAngleUnits (m_deg);
}

inline Angle::operator Radian() const
{
	return Radian(Math::AngleUnitsToRadians(m_angle));
}

inline Angle::operator Degree() const
{
	return Degree(Math::AngleUnitsToDegrees(m_angle));
}

inline Radian operator * (float a, const Radian& b)
{
	return Radian (a * b.valueRadians());
}

inline Radian operator / (float a, const Radian& b)
{
	return Radian (a / b.valueRadians());
}

inline Degree operator * (float a, const Degree& b)
{
	return Degree (a * b.valueDegrees());
}

inline Degree operator / (float a, const Degree& b)
{
	return Degree (a / b.valueDegrees());
}

#endif
