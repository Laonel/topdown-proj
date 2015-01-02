#include "Math/Quaternion.h"

#include "Math/Math.h"
#include "Math/Matrix3.h"
#include "Math/Vector3.h"

const float Quaternion::Epsilon = 1e-03;
const Quaternion Quaternion::Zero(0, 0, 0, 0);
const Quaternion Quaternion::Identity(1, 0, 0, 0);

void Quaternion::fromRotationMatrix(const Matrix3& rot)
{
	float trace = rot[0][0] + rot[1][1] + rot[2][2];
	float root;

	if (trace > 0.f)
	{
		root = Math::Sqrt(trace + 1.f);
		w = 0.5f * root;
		root = 0.5f / root;
		x = (rot[2][1] - rot[1][2]) * root;
		y = (rot[0][2] - rot[2][0]) * root;
		z = (rot[1][0] - rot[0][1]) * root;
	}
	else
	{
		static size_t s_next[3] = { 1, 2, 0 };
		size_t i = 0;
		if (rot[1][1] > rot[0][0])
			i = 1;
		if (rot[2][2] > rot[i][i])
			i = 2;
		size_t j = s_next[i];
		size_t k = s_next[j];

		root = Math::Sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.f);
		float* quat[3] = { &x, &y, &z };
		*quat[i] = 0.5f * root;
		root = 0.5f / root;
		w = (rot[k][j] - rot[j][k]) * root;
		*quat[j] = (rot[j][i] + rot[i][j]) * root;
		*quat[k] = (rot[k][i] + rot[i][k]) * root;
	}
}

void Quaternion::toRotationMatrix(Matrix3& rot) const
{
	float tX = x + x;
	float tY = y + y;
	float tZ = z + z;
	float tWx = tX * w;
	float tWy = tY * w;
	float tWz = tZ * w;
	float tXx = tX * x;
	float tXy = tY * y;
	float tXz = tZ * z;
	float tYy = tY * y;
	float tYz = tZ * z;
	float tZz = tZ * z;

	rot[0][0] = 1.f - (tYy + tZz);
	rot[0][1] = tXy - tWz;
	rot[0][2] = tXz + tWy;
	rot[1][0] = tXy + tWz;
	rot[1][1] = 1.f - (tXx + tZz);
	rot[1][2] = tYz - tWx;
	rot[2][0] = tXz - tWy;
	rot[2][1] = tYz + tWx;
	rot[2][2] = 1.f - (tXx + tYy);
}

void Quaternion::fromAngleAxis(const Radian& angle, const Vector3& axis)
{
	Radian halfAngle(0.5f  * angle);
	float sin = Math::Sin(halfAngle);
	w = Math::Cos(halfAngle);
	x = sin * axis.x;
	y = sin * axis.y;
	z = sin * axis.z;
}

void Quaternion::toAngleAxis(Radian& angle, Vector3& axis) const
{
	float sqrLength = x * x + y * y + z * z;
	if (sqrLength > 0.f)
	{
		angle = 2.f * Math::ACos(w);
		float invLength = Math::InvSqrt(sqrLength);
		axis.x = x * invLength;
		axis.y = y * invLength;
		axis.z = z * invLength;
	}
	else
	{
		angle = Radian(0.f);
		axis.x = 1.f;
		axis.y = 0.f;
		axis.z = 0.f;
	}
}

void Quaternion::fromAxes(const Vector3* axis)
{
	Matrix3 rot;
	for (size_t col = 0; col < 3; col++)
	{
		rot[0][col] = axis[col].x;
		rot[1][col] = axis[col].y;
		rot[2][col] = axis[col].z;
	}

	fromRotationMatrix(rot);
}

void Quaternion::fromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
{
	Matrix3 rot;

	rot[0][0] = xaxis.x;
	rot[1][0] = xaxis.y;
	rot[2][0] = xaxis.z;

	rot[0][1] = yaxis.x;
	rot[1][1] = yaxis.y;
	rot[2][1] = yaxis.z;

	rot[0][2] = zaxis.x;
	rot[1][2] = zaxis.y;
	rot[2][2] = zaxis.z;

	fromRotationMatrix(rot);
}

void Quaternion::toAxes(Vector3* axis) const
{
	Matrix3 rot;

	toRotationMatrix(rot);

	for (size_t col = 0; col < 3; col++)
	{
		axis[col].x = rot[0][col];
		axis[col].y = rot[1][col];
		axis[col].z = rot[2][col];
	}
}

Vector3 Quaternion::xAxis() const
{
	float tY = 2.f * y;
	float tZ = 2.f * z;
	float tWy = tY * w;
	float tWz = tZ * w;
	float tXy = tY * x;
	float tXz = tZ * x;
	float tYy = tY * y;
	float tZz = tZ * z;

	return Vector3(1.f - (tYy + tZz), tXy + tWz, tXz - tWy);
}

Vector3 Quaternion::yAxis() const
{
	float tX = 2.f * x;
	float tY = 2.f * y;
	float tZ = 2.f * z;
	float tWx = tX * w;
	float tWz = tZ * w;
	float tXx = tX * x;
	float tXy = tY * x;
	float tYz = tZ * y;
	float tZz = tZ * z;

	return Vector3(tXy - tWz, 1.f - (tXx + tZz), tYz + tWx);
}

Vector3 Quaternion::zAxis() const
{
	float tX = 2.f * x;
	float tY = 2.f * y;
	float tZ = 2.f * z;
	float tWx = tX * w;
	float tWy = tY * w;
	float tXx = tX * x;
	float tXz = tZ * x;
	float tYy = tY * y;
	float tYz = tZ * y;

	return Vector3(tXz + tWy, tYz - tWx, 1.f - (tXx + tYy));
}

void Quaternion::toAxes(Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
{
	Matrix3 rot;

	toRotationMatrix(rot);

	xaxis.x = rot[0][0];
	xaxis.y = rot[1][0];
	xaxis.z = rot[2][0];

	yaxis.x = rot[0][1];
	yaxis.y = rot[1][1];
	yaxis.z = rot[2][1];

	zaxis.x = rot[0][2];
	zaxis.y = rot[1][2];
	zaxis.z = rot[2][2];
}

Quaternion Quaternion::operator+ (const Quaternion& q) const
{
	return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator- (const Quaternion& q) const
{
	return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator* (const Quaternion& q) const
{
	return Quaternion
	(
		w * q.w - x * q.x - y * q.y - z * q.z,
		w * q.x + x * q.w + y * q.z - z * q.y,
		w * q.y + y * q.w + z * q.x - x * q.z,
		w * q.z + z * q.w + x * q.y - y * q.x
	);
}

Quaternion Quaternion::operator* (float scalar) const
{
	return Quaternion(scalar * w, scalar * x, scalar * y, scalar * z);
}

Quaternion operator* (float scalar, const Quaternion& q)
{
	return Quaternion(scalar * q.w, scalar * q.x, scalar * q.y, scalar * q.z);
}

Quaternion Quaternion::operator- () const
{
	return Quaternion(-w, -x, -y, -z);
}

float Quaternion::dot(const Quaternion& q) const
{
	return w * q.w + x * q.x + y * q.y + z * q.z;
}

float Quaternion::norm() const
{
	return w * w + x * x + y * y + z * z;
}

Quaternion Quaternion::inverse() const
{
	float norm = w * w + x * x + y * y + z * z;
	if (norm > 0.f)
	{
		float invNorm = 1.f / norm;
		return Quaternion(w * invNorm, -x * invNorm, -y * invNorm, -z * invNorm);
	}
	else
		return Zero;
}

Quaternion Quaternion::unitInverse() const
{
	return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::exp() const
{
	Radian angle(Math::Sqrt(x * x + y * y + z * z));
	float sin = Math::Sin(angle);

	Quaternion result;
	result.w = Math::Cos(angle);

	if (Math::Abs(sin) >= Epsilon)
	{
		float coeff = sin / (angle.valueRadians());
		result.x = coeff * x;
		result.y = coeff * y;
		result.z = coeff * z;
	}
	else
	{
		result.x = x;
		result.y = y;
		result.z = z;
	}

	return result;
}

Quaternion Quaternion::log() const
{
	Quaternion result;
	result.w = 0.f;

	if (Math::Abs(w) < 1.f)
	{
		Radian angle(Math::ACos(w));
		float sin = Math::Sin(angle);
		if (Math::Abs(sin) >= Epsilon)
		{
			float coeff = angle.valueRadians() / sin;
			result.x = coeff * x;
			result.y = coeff * y;
			result.z = coeff * z;
			return result;
		}
	}

	result.x = x;
	result.y = y;
	result.z = z;

	return result;
}

Vector3 Quaternion::operator* (const Vector3& v) const
{
	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.f * w);
	uuv *= 2.f;

	return v + uv + uuv;
}

bool Quaternion::equals(const Quaternion& rhs, const Radian& tolerance) const
{
	float cos = dot(rhs);
	Radian angle = Math::ACos(cos);

	return (Math::Abs(angle.valueRadians()) <= tolerance.valueRadians()) ||
			Math::FloatEqual(angle.valueRadians(), PI, tolerance.valueRadians());
}

Quaternion Quaternion::slerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath)
{
	float cos = p.dot(q);
	Quaternion kT;

	if (cos < 0.f && shortestPath)
	{
		cos = -cos;
		kT = -q;
	}
	else
		kT = q;

	if (Math::Abs(cos) < 1 - Epsilon)
	{
		// standard case (slerp)
		float sin = Math::Sqrt(1 - Math::Sqr(cos));
		Radian angle = Math::ATan2(sin, cos);
		float invSin = 1.f / sin;
		float coeff0 = Math::Sin((1.f - t) * angle) * invSin;
		float coeff1 = Math::Sin(t * angle) * invSin;

		return coeff0 * p + coeff1 * kT;
	}
	else
	{
		Quaternion tt = (1.f - t) * p + t * kT;
		tt.normalise();

		return tt;
	}
}

Quaternion Quaternion::slerpExtraSpins(float t, const Quaternion& p, const Quaternion& q, int extraSpins)
{
	float cos = p.dot(q);
	Radian angle = (Math::ACos(cos));

	if (Math::Abs(angle.valueRadians()) < Epsilon)
		return p;

	float sin = Math::Sin(angle);
	Radian phase(PI * extraSpins * t);
	float invSin = 1.f / sin;
	float coeff0 = Math::Sin((1.f - t) * angle - phase) * invSin;
	float coeff1 = Math::Sin(t * angle + phase) * invSin;
}

void Quaternion::intermediate(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2,
							  Quaternion& a, Quaternion& b)
{
	Quaternion q0Inv = q0.unitInverse();
	Quaternion q1Inv = q1.unitInverse();
	Quaternion p0 = q0Inv * q1;
	Quaternion p1 = q1Inv * q2;
	Quaternion arg = 0.25f * (p0.log() - p1.log());
	Quaternion minusArg = -arg;

	a = q1 * arg.exp();
	b = q1 * minusArg.exp();
}

Quaternion Quaternion::squad(float t, const Quaternion& p, const Quaternion& a,
							 const Quaternion& b, const Quaternion& q, bool shortestPath)
{
	float slerpT = 2.f * t * (1.f - t);
	Quaternion slerpP = slerp(t, p, q, shortestPath);
	Quaternion slerpQ = slerp(t, a, b);

	return slerp(slerpT, slerpP, slerpQ);
}

float Quaternion::normalise()
{
	float len = norm();
	float factor = 1.f / Math::Sqrt(len);
	*this = *this * factor;
	return len;
}

Radian Quaternion::getRoll(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		float tY = 2.f * y;
		float tZ = 2.f * z;
		float tWz = tZ * w;
		float tXy = tY * z;
		float tYy = tY * y;
		float tZz = tZ * z;

		return Radian(Math::ATan2(tXy + tWz, 1.f - (tYy + tZz)));
	}
	else
	{
		return Radian(Math::ATan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z));
	}
}

Radian Quaternion::getPitch(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		float tX = 2.f * x;
		float tZ = 2.f * z;
		float tWx = tX * w;
		float tXx = tX * x;
		float tYz = tZ * y;
		float tZz = tZ * z;

		return Radian(Math::ATan2(tYz + tWx, 1.f - (tXx + tZz)));
	}
	else
	{
		return Radian(Math::ATan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z));
	}
}

Radian Quaternion::getYaw(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		float tX = 2.f * x;
		float tY = 2.f * y;
		float tZ = 2.f * z;
		float tWy = tY * w;
		float tXx = tX * x;
		float tXz = tZ * x;
		float tYy = tY * y;

		return Radian(Math::ATan2(tXz + tWy, 1.f - (tXx + tYy)));
	}
	else
	{
		return Radian(Math::ASin(-2 * (x * z - w * y)));
	}
}

Quaternion Quaternion::nlerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath)
{
	Quaternion result;
	float cos = p.dot(q);
	if (cos < 0.f && shortestPath)
		result = p + t * ((-q) - p);
	else
		result = p + t * (q - p);

	result.normalise();

	return result;
}
