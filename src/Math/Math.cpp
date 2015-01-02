#include "Math/Math.h"

#include "Math/Vector2.h"

const float Math::POS_INFINITY = std::numeric_limits<float>::infinity();
const float Math::NEG_INFINITY = -std::numeric_limits<float>::infinity();
const float Math::fDeg2Rad = PI / float(180.0);
const float Math::fRad2Deg = float(180.0) / PI;
const float Math::LOG2 = log(float(2.0));

int Math::s_trigTableSize;
Math::AngleUnit Math::s_angleUnit;

float Math::s_trigTableFactor;
float *Math::s_sinTable = NULL;
float *Math::s_tanTable = NULL;

Math::Math( unsigned int trigTableSize )
{
	s_angleUnit = UnitDegree;
	s_trigTableSize = trigTableSize;
	s_trigTableFactor = s_trigTableSize / TWO_PI;

	s_sinTable = new float[s_trigTableSize];
	s_tanTable = new float[s_trigTableSize];

	buildTrigTables();
}

Math::~Math()
{
	DELETE_ARRAY(s_sinTable);
	DELETE_ARRAY(s_tanTable);
}

void Math::buildTrigTables()
{
	// Build trig lookup tables
	// Could get away with building only PI sized Sin table but simpler this
	// way. Who cares, it'll ony use an extra 8k of memory anyway and I like
	// simplicity.
	float angle;
	for (int i = 0; i < s_trigTableSize; ++i)
	{
		angle = TWO_PI * i / s_trigTableSize;
		s_sinTable[i] = sin(angle);
		s_tanTable[i] = tan(angle);
	}
}

float Math::SinTable (float fValue)
{
	// Convert range to index values, wrap if required
	int idx;
	if (fValue >= 0)
	{
		idx = int(fValue * s_trigTableFactor) % s_trigTableSize;
	}
	else
	{
		idx = s_trigTableSize - (int(-fValue * s_trigTableFactor) % s_trigTableSize) - 1;
	}

	return s_sinTable[idx];
}

float Math::TanTable (float fValue)
{
	// Convert range to index values, wrap if required
	int idx = int(fValue *= s_trigTableFactor) % s_trigTableSize;
	return s_tanTable[idx];
}

int Math::ISign (int iValue)
{
	return (iValue > 0 ? +1 : (iValue < 0 ? -1 : 0));
}

Radian Math::ACos(float fValue)
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return Radian(acos(fValue));
		else
			return Radian(0.0);
	}
	else
	{
		return Radian(PI);
	}
}

Radian Math::ASin(float fValue)
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return Radian(asin(fValue));
		else
			return Radian(HALF_PI);
	}
	else
	{
		return Radian(-HALF_PI);
	}
}

float Math::Sign(float fValue)
{
	if ( fValue > 0.0 )
		return 1.0;

	if ( fValue < 0.0 )
		return -1.0;

	return 0.0;
}

float Math::InvSqrt(float fValue)
{
	return float(1.f / sqrt(fValue));
}

float Math::UnitRandom ()
{
	return rand() / RAND_MAX;
}

float Math::RangeRandom (float fLow, float fHigh)
{
	return (fHigh-fLow)*UnitRandom() + fLow;
}

float Math::SymmetricRandom ()
{
	return 2.0f * UnitRandom() - 1.0f;
}

void Math::setAngleUnit(Math::AngleUnit unit)
{
	s_angleUnit = unit;
}

Math::AngleUnit Math::getAngleUnit(void)
{
	return s_angleUnit;
}

float Math::AngleUnitsToRadians(float angleunits)
{
	if (s_angleUnit == UnitDegree)
		return angleunits * fDeg2Rad;
	else
		return angleunits;
}

float Math::RadiansToAngleUnits(float radians)
{
	if (s_angleUnit == UnitDegree)
		return radians * fRad2Deg;
	else
		return radians;
}

float Math::AngleUnitsToDegrees(float angleunits)
{
	if (s_angleUnit == UnitRadian)
		return angleunits * fRad2Deg;
	else
		return angleunits;
}

float Math::DegreesToAngleUnits(float degrees)
{
	if (s_angleUnit == UnitRadian)
		return degrees * fDeg2Rad;
	else
		return degrees;
}

bool Math::pointInTri2D(const Vector2& p, const Vector2& a,
						const Vector2& b, const Vector2& c)
   {
	// Winding must be consistent from all edges for point to be inside
	Vector2 v1, v2;
	float dot[3];
	bool zeroDot[3];

	v1 = b - a;
	v2 = p - a;

	// Note we don't care about normalisation here since sign is all we need
	// It means we don't have to worry about magnitude of cross products either
	dot[0] = v1.crossProduct(v2);
	zeroDot[0] = Math::FloatEqual(dot[0], 0.0f, 1e-3);

	v1 = c - b;
	v2 = p - b;

	dot[1] = v1.crossProduct(v2);
	zeroDot[1] = Math::FloatEqual(dot[1], 0.0f, 1e-3);

	// Compare signs (ignore colinear / coincident points)
	if(!zeroDot[0] && !zeroDot[1]
	&& Math::Sign(dot[0]) != Math::Sign(dot[1]))
	{
		return false;
	}

	v1 = a - c;
	v2 = p - c;

	dot[2] = v1.crossProduct(v2);
	zeroDot[2] = Math::FloatEqual(dot[2], 0.0f, 1e-3);
	// Compare signs (ignore colinear / coincident points)
	if((!zeroDot[0] && !zeroDot[2]
		&& Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
		(!zeroDot[1] && !zeroDot[2]
		&& Math::Sign(dot[1]) != Math::Sign(dot[2])))
	{
		return false;
	}


	return true;
}

bool Math::FloatEqual(float a, float b, float tolerance)
{
	if (fabs(b-a) <= tolerance)
		return true;
	else
		return false;
}
