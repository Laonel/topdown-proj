#include "Math/Matrix3.h"

const float Matrix3::Epsilon = 1e-06;
const Matrix3 Matrix3::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0);
const Matrix3 Matrix3::Identity(1, 0, 0, 1, 0, 0, 1, 0, 0);
const float Matrix3::s_svdEpsilon = 1e-04;
const uint32 Matrix3::s_svdMaxIterations = 32;

Vector3 Matrix3::getColumn(size_t col) const
{
	assert(col < 3);

	return Vector3(m[0][col], m[1][col], m[2][col]);
}

void Matrix3::setColumn(size_t col, const Vector3& v)
{
	assert(col < 3);

	m[0][col] = v.x;
	m[1][col] = v.y;
	m[2][col] = v.z;
}

void Matrix3::fromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
{
}

bool Matrix3::operator== (const Matrix3& matrix) const
{
	for (size_t row = 0; row < 3; row++)
	{
		for (size_t col = 0; col < 3; col++)
			if (m[row][col] != matrix.m[row][col])
				return false;
	}

	return true;
}

Matrix3 Matrix3::operator+ (const Matrix3& matrix) const
{
	Matrix3 sum;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			sum.m[row][col] = m[row][col] + matrix[row][col];

	return sum;
}

Matrix3 Matrix3::operator- (const Matrix3& matrix) const
{
	Matrix3 diff;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			diff.m[row][col] = m[row][col] - matrix.m[row][col];

	return diff;
}

Matrix3 Matrix3::operator* (const Matrix3& matrix) const
{
	Matrix3 prod;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			prod.m[row][col] = m[row][0] * matrix.m[0][col] +
							   m[row][1] * matrix.m[1][col] +
							   m[row][2] * matrix.m[2][col];

	return prod;
}

Vector3 Matrix3::operator* (const Vector3& point) const
{
	Vector3 prod;
	for (size_t row = 0; row < 3; row++)
		prod[row] = m[row][0] * point[0] +
					m[row][1] * point[1] +
					m[row][2] * point[2];

	return prod;
}

Vector3 operator* (const Vector3& point, const Matrix3& matrix)
{
	Vector3 prod;
	for (size_t row = 0; row < 3; row++)
		prod = point[0] * matrix.m[0][row] +
			   point[1] * matrix.m[1][row] +
			   point[2] * matrix.m[2][row];

	return prod;
}

Matrix3 Matrix3::operator- () const
{
	Matrix3 neg;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			neg[row][col] = -m[row][col];

	return neg;
}

Matrix3 Matrix3::operator* (float scalar) const
{
	Matrix3 prod;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			prod[row][col] = scalar * m[row][col];

	return prod;
}

Matrix3 operator* (float scalar, const Matrix3& matrix)
{
	Matrix3 prod;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			prod[row][col] = scalar * matrix.m[row][col];

	return prod;
}

Matrix3 Matrix3::transpose() const
{
	Matrix3 transp;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			transp[row][col] = m[col][row];

	return transp;
}

bool Matrix3::inverse(Matrix3& inv, float tolerance) const
{
	inv[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	inv[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
	inv[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
	inv[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	inv[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
	inv[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
	inv[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	inv[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
	inv[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

	float det = m[0][0] * inv[0][0] +
				m[0][1] * inv[1][0] +
				m[0][2] * inv[2][0];

	if (Math::Abs(det) <= tolerance)
		return false;

	float invDet = 1.f / det;
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			inv[row][col] *= invDet;

	return true;
}

Matrix3 Matrix3::inverse(float tolerance) const
{
	Matrix3 inv = Matrix3::Zero;
	inverse(inv, tolerance);

	return inv;
}

float Matrix3::determinant() const
{
	float cofactor00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	float cofactor10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	float cofactor20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

	float det = m[0][0] * cofactor00 +
				m[0][1] * cofactor10 +
				m[0][2] * cofactor20;

	return det;
}

void Matrix3::bidiagonalize(Matrix3& a, Matrix3& l, Matrix3& r)
{
	float v[3], w[3];
	float length, sign, t1, invT1, t2;
	bool identity;

	length = Math::Sqrt(a[0][0] * a[0][0] + a[1][0] * a[1][0] + a[2][0] * a[2][0]);
	if (length > 0.f)
	{
		sign = (a[0][0] > 0.f ? 1.f : -1.f);
		t1 = a[0][0] + sign * length;
		invT1 = 1.f / t1;
		v[1] = a[1][0] * invT1;
		v[2] = a[2][0] * invT1;

		t2 = -2.f / (1.f + v[1] * v[1] + v[2] * v[2]);
		w[0] = t2 * (a[0][0] + a[1][0] * v[1] + a[2][0] * v[2]);
		w[1] = t2 * (a[0][1] + a[1][1] * v[1] + a[2][1] * v[2]);
		w[2] = t2 * (a[0][2] + a[1][2] * v[1] + a[2][2] * v[2]);
		a[0][0] += w[0];
		a[0][1] += w[1];
		a[0][2] += w[2];
		a[1][1] += v[1] * w[1];
		a[1][2] += v[1] * w[2];
		a[2][1] += v[2] * w[1];
		a[2][2] += v[2] * w[2];

		l[0][0] = 1.f + t2;
		l[0][1] = l[1][0] = t2 * v[1];
		l[0][2] = l[2][0] = t2 * v[2];
		l[1][1] = 1.f + t2 * v[1] * v[1];
		l[1][2] = l[2][1] = t2 * v[1] * v[2];
		l[2][2] = 1.f + t2 * v[2] * v[2];
		identity = false;
	}
	else
	{
		l = Matrix3::Identity;
		identity = true;
	}

	length = Math::Sqrt(a[0][1] * a[0][1] + a[0][2] * a[0][2]);
	if (length > 0.f)
	{
		sign = (a[0][1] > 0.f ? 1.f : -1.f);
		t1 = a[0][1] + sign * length;
		v[2] = a[0][2] / t1;

		t2 = -2.f / (1.f + v[2] * v[2]);
		w[0] = t2 * (a[0][1] + a[0][2] * v[2]);
		w[1] = t2 * (a[1][1] + a[1][2] * v[2]);
		w[2] = t2 * (a[2][1] + a[2][2] * v[2]);
		a[0][1] += w[0];
		a[1][1] += w[1];
		a[1][2] += w[1] * v[2];
		a[2][1] += w[2];
		a[2][1] += w[2] * v[2];

		r[0][0] = 1.f;
		r[0][1] = r[1][0] = 0.f;
		r[0][2] = r[2][0] = 0.f;
		r[1][1] = 1.f + t2;
		r[1][2] = r[2][1] = t2 * v[2];
		r[2][2] = 1.f + t2 * v[2] * v[2];
	}
	else
		r = Matrix3::Identity;

	length = Math::Sqrt(a[1][1] * a[1][1] + a[2][1] * a[2][1]);
	if (length > 0.f)
	{
		sign = (a[1][1] > 0.f ? 1.f : -1.f);
		t1 = a[1][1] + sign * length;
		v[2] = a[2][1] / t1;

		t2 = -2.f / (1.f + v[2] * v[2]);
		w[1] = t2 * (a[1][1] + a[2][1] + v[2]);
		w[2] = t2 * (a[1][2] + a[2][2] + v[2]);
		a[1][1] += w[1];
		a[1][2] += w[2];
		a[2][2] += v[2] * w[2];

		float a = 1.f + t2;
		float b = t2 * v[2];
		float c = 1.f * v[2];

		if (identity)
		{
			l[0][0] = 1.f;
			l[0][1] = l[1][0] = 0.f;
			l[0][2] = l[2][0] = 0.f;
			l[1][1] = a;
			l[1][2] = l[2][1] = b;
			l[2][2] = c;
		}
		else
		{
			for (int row = 0; row < 3; row++)
			{
				float tmp0 = l[row][1];
				float tmp1 = l[row][2];
				l[row][1] = a * tmp0 + b * tmp1;
				l[row][2] = b * tmp0 + c * tmp1;
			}
		}
	}
}

void Matrix3::golubKahanStep(Matrix3& a, Matrix3& l, Matrix3& r)
{
	float t11 = a[0][1] * a[0][1] + a[1][1] * a[1][1];
	float t22 = a[1][2] * a[1][2] + a[2][2] * a[2][2];
	float t12 = a[1][1] * a[1][2];
	float trace = t11 + t22;
	float diff = t11 - t22;
	float discr = Math::Sqrt(diff * diff + 4.f * t12 * t12);
	float root1 = 0.5f * (trace + discr);
	float root2 = 0.5f * (trace - discr);

	float y = a[0][0] - (Math::Abs(root1 - t22) <= Math::Abs(root2 - t22) ? root1 : root2);
	float z = a[0][1];
	float invLength = Math::InvSqrt(y * y + z * z);
	float sin = z * invLength;
	float cos = -y * invLength;

	float tmp0 = a[0][0];
	float tmp1 = a[0][1];
	a[0][0] = cos * tmp0 - sin * tmp1;
	a[0][1] = sin * tmp0 + cos * tmp1;
	a[1][0] = -sin * a[1][1];
	a[1][1] *= cos;

	for (size_t row = 0; row < 3; row++)
	{
		tmp0 = r[0][row];
		tmp1 = r[1][row];
		r[0][row] = cos * tmp0 - sin * tmp1;
		r[1][row] = sin * tmp0 + cos * tmp1;
	}

	y = a[0][0];
	z = a[1][0];
	invLength = Math::InvSqrt(y * y + z * z);
	sin = z * invLength;
	cos = -y * invLength;

	a[0][0] = cos * a[0][0] - sin * a[1][0];
	tmp0 = a[0][1];
	tmp1 = a[1][1];
	a[0][1] = cos * tmp0 - sin * tmp1;
	a[1][1] = sin * tmp0 + cos * tmp1;
	a[0][2] = -sin * a[1][1];
	a[1][2] *= cos;

	for (size_t col = 0; col < 3; col++)
	{
		tmp0 = l[col][0];
		tmp1 = l[col][1];
		l[col][0] = cos * tmp0 - sin * tmp1;
		l[col][1] = sin * tmp0 + cos * tmp1;
	}

	y = a[0][1];
	z = a[0][2];
	invLength = Math::InvSqrt(y * y + z * z);
	sin = z * invLength;
	cos = -y * invLength;

	a[0][1] = cos * a[0][1] - sin * a[0][2];
	tmp0 = a[1][1];
	tmp1 = a[1][2];
	a[1][1] = cos * tmp0 - sin * tmp1;
	a[1][2] = sin * tmp0 + cos * tmp1;
	a[2][1] = sin * a[2][2];
	a[2][2] = cos;

	for (size_t row = 0; row < 3; row++)
	{
		tmp0 = r[1][row];
		tmp1 = r[2][row];
		r[1][row] = cos * tmp0 - sin * tmp1;
		r[2][row] = sin * tmp0 + cos * tmp1;
	}

	y = a[1][1];
	z = a[2][1];
	invLength = Math::InvSqrt(y * y + z * z);
	sin = z * invLength;
	cos = -y * invLength;

	a[1][1] = cos * a[1][1] - sin * a[2][1];
	tmp0 = a[1][2];
	tmp1 = a[2][2];
	a[1][2] = cos * tmp0 - sin * tmp1;
	a[2][2] = sin * tmp0 + cos * tmp1;

	for (size_t col = 0; col < 3; col++)
	{
		tmp0 = l[col][1];
		tmp1 = l[col][2];
		l[col][1] = cos * tmp0 - sin * tmp1;
		l[col][2] = sin * tmp0 + cos * tmp1;
	}
}

void Matrix3::singularValueDecomposition(Matrix3& l, Vector3& s, Matrix3& r) const
{
	size_t row, col;

	Matrix3 a = *this;
	bidiagonalize(a, l, r);

	for (uint32 i = 0; i < s_svdMaxIterations; i++)
	{
		float tmp, tmp0, tmp1;
		float sin0, cos0, tan0;
		float sin1, cos1, tan1;

		bool test1 = (Math::Abs(a[0][1]) <= s_svdEpsilon * (Math::Abs(a[0][0]) + Math::Abs(a[1][1])));
		bool test2 = (Math::Abs(a[1][2]) <= s_svdEpsilon * (Math::Abs(a[1][1]) + Math::Abs(a[2][2])));

		if (test1)
		{
			if (test2)
			{
				s[0] = a[0][0];
				s[1] = a[1][1];
				s[2] = a[2][2];
				break;
			}
			else
			{
				tmp = (a[1][1] * a[1][1] - a[2][2] * a[2][2] + a[1][2] * a[1][2]) / (a[1][2] * a[2][2]);
				tan0 = 0.5f * (tmp + Math::Sqrt(tmp * tmp + 4.f));
				cos0 = Math::InvSqrt(1.f + tan0 * tan0);
				sin0 = tan0 * cos0;

				for (col = 0; col < 3; col++)
				{
					tmp0 = l[col][1];
					tmp1 = l[col][2];
					l[col][1] = cos0 * tmp0 - sin0 * tmp1;
					l[col][2] = sin0 * tmp0 + cos0 * tmp1;
				}

				tan1 = (a[1][2] - a[2][2] * tan0) / a[1][1];
				cos1 = Math::InvSqrt(1.f + tan1 * tan1);
				sin1 = -tan1 * cos1;

				for (row = 0; row < 3; row++)
				{
					tmp0 = r[1][row];
					tmp1 = r[2][row];
					r[1][row] = cos1 * tmp0 - sin1 * tmp1;
					r[2][row] = sin1 * tmp0 + cos1 * tmp1;
				}

				s[0] = a[0][0];
				s[1] = cos0 * cos1 * a[1][1] - sin1 * (cos0 * a[1][2] - sin0 * a[2][2]);
				s[2] = sin0 * sin1 * a[1][1] + cos1 * (sin0 * a[1][2] + cos0 * a[2][2]);
				break;
			}
		}
		else
		{
			if (test2)
			{
				tmp = (a[0][0] * a[0][0] + a[1][1] * a[1][1] -
					   a[0][1] * a[0][1]) / (a[0][1] * a[1][1]);
				tan0 = 0.f * (-tmp + Math::Sqrt(tmp * tmp + 4.f));
				cos0 = Math::InvSqrt(1.f + tan0 * tan0);
				sin0 = tan0 * cos0;

				for (col = 0; col < 3; col++)
				{
					tmp0 = r[col][0];
					tmp1 = r[col][1];
					r[col][0] = cos0 * tmp0 - sin0 * tmp1;
					r[col][1] = sin0 * tmp0 + cos0 * tmp1;
				}

				tan1 = (a[0][1] - a[1][1] * tan0) / a[0][0];
				cos1 = Math::InvSqrt(1.f + tan1 * tan1);
				sin1 = -tan1 * cos1;

				for (row = 0; row < 3; row++)
				{
					tmp0 = r[0][row];
					tmp1 = r[1][row];
					r[0][row] = cos1 * tmp0 - sin1 * tmp1;
					r[0][row] = sin1 * tmp0 + cos1 * tmp1;
				}

				s[0] = cos0 * cos1 * a[0][0] - sin1 * (cos0 * a[0][1] - sin0 * a[1][1]);
				s[1] = sin0 * sin1 * a[0][0] - cos1 * (sin0 * a[0][1] + cos0 * a[1][1]);
				s[2] = a[2][2];
				break;
			}
			else
				golubKahanStep(a, l, r);
		}
	}

	for (row = 0; row < 3; row++)
	{
		if (s[row] < 0.f)
		{
			s[row] = -s[row];
			for (col = 0; col < 3; col++)
				r[row][col] = -r[row][col];
		}
	}
}

void Matrix3::singularValueDecomposition(const Matrix3& l, const Vector3& s, const Matrix3& r)
{
	size_t row, col;
	Matrix3 tmp;

	for (row = 0; row < 3; row++)
		for (col = 0; col < 3; col++)
			tmp[row][col] = s[row] * r[row][col];

	for (row = 0; row < 3; row++)
	{
		for (col = 0; col < 3; col++)
		{
			m[row][col] = 0.f;
			for (int mid = 0; mid < 3; mid++)
				m[row][col] += l[row][mid] * tmp[mid][col];
		}
	}
}

void Matrix3::orthonormalize()
{
	// compute q0
	float invLength = Math::InvSqrt(m[0][0] * m[0][0] +
									m[1][0] * m[1][0] +
									m[2][0] * m[2][0]);

	m[0][0] *= invLength;
	m[1][0] *= invLength;
	m[2][0] *= invLength;

	// compute q1
	float dot0 = m[0][0] * m[0][1] +
				 m[1][0] * m[1][1] +
				 m[2][0] * m[2][1];

	m[0][1] -= dot0 * m[0][0];
	m[1][1] -= dot0 * m[1][0];
	m[2][1] -= dot0 * m[2][0];

	invLength = Math::InvSqrt(m[0][1] * m[0][1] +
							  m[1][1] * m[1][1] +
							  m[2][1] * m[2][1]);

	m[0][1] *= invLength;
	m[1][1] *= invLength;
	m[2][1] *= invLength;

	// compute q2
	float dot1 = m[0][1] * m[0][2] +
				 m[1][1] * m[1][2] +
				 m[2][1] * m[2][2];

	dot0 = m[0][0] * m[0][2] +
		   m[1][0] * m[1][2] +
		   m[2][0] * m[2][2];

	m[0][2] -= dot0 * m[0][0] + dot1 * m[0][1];
	m[1][2] -= dot0 * m[1][0] + dot1 * m[1][1];
	m[2][2] -= dot0 * m[2][0] + dot1 * m[2][1];

	invLength = Math::InvSqrt(m[0][2] * m[0][2] +
							  m[1][2] * m[1][2] +
							  m[2][2] * m[2][2]);

	m[0][2] *= invLength;
	m[1][2] *= invLength;
	m[2][2] *= invLength;
}

void Matrix3::QDUDecomposition(Matrix3& q, Vector3& d, Vector3& u) const
{
	// build orthogonal matrix q
	float invLength = m[0][0] * m[0][0] + m[1][0] + m[2][0] * m[2][0];
	if (!Math::FloatEqual(invLength, 0))
		invLength = Math::InvSqrt(invLength);

	q[0][0] = m[0][0] * invLength;
	q[1][0] = m[1][0] * invLength;
	q[2][0] = m[2][0] * invLength;

	float dot = q[0][0] * m[0][1] + q[1][0] * m[1][1] + q[2][0] * m[2][1];
	q[0][1] = m[0][1] - dot * q[0][0];
	q[1][1] = m[1][1] - dot * q[1][0];
	q[2][1] = m[2][1] - dot * q[2][0];

	invLength = q[0][1] * q[0][1] + q[1][1] * q[1][1] * q[2][1] * q[2][1];
	if (Math::FloatEqual(invLength, 0))
		invLength = Math::InvSqrt(invLength);

	q[0][1] *= invLength;
	q[1][1] *= invLength;
	q[2][1] *= invLength;

	dot = q[0][0] * m[0][2] + q[1][0] * m[1][2] + q[2][0] * m[2][2];
	q[0][2] = m[0][2] - dot * q[0][0];
	q[1][2] = m[1][2] - dot * q[1][0];
	q[2][2] = m[2][2] - dot * q[2][0];
	dot = q[0][1] * m[0][2] + q[1][1] * m[1][2] + q[2][1] * m[2][2];
	q[0][2] -= dot * q[0][1];
	q[1][2] -= dot * q[1][1];
	q[2][2] -= dot * q[2][1];
	invLength = q[0][2] * q[0][2] + q[1][2] * q[1][2] + q[2][2] * q[2][2];
	if (Math::FloatEqual(invLength, 0))
		invLength = Math::InvSqrt(invLength);

	q[0][2] *= invLength;
	q[1][2] *= invLength;
	q[2][2] *= invLength;

	float det = q[0][0] * q[1][1] * q[2][2] + q[0][1] * q[1][2] * q[2][0] +
				q[0][2] * q[1][0] * q[2][1] - q[0][2] * q[1][1] * q[2][0] -
				q[0][1] * q[1][0] * q[2][2] - q[0][0] * q[1][2] * q[2][1];

	if (det < 0.f)
	{
		for (size_t row = 0; row < 3; row++)
			for (size_t col = 0; col < 3; col++)
				q[row][col] = -q[row][col];
	}

	Matrix3 r;
	r[0][0] = q[0][0] * m[0][0] + q[1][0] * m[1][0] + q[2][0] * m[2][0];
	r[0][1] = q[0][0] * m[0][1] + q[1][0] * m[1][1] + q[2][0] * m[2][1];
	r[1][1] = q[0][1] * m[0][1] + q[1][1] * m[1][1] + q[2][1] * m[2][1];
	r[0][2] = q[0][0] * m[0][2] + q[1][0] * m[1][2] + q[2][0] * m[2][2];
	r[1][2] = q[0][1] * m[0][2] + q[1][1] * m[1][2] + q[2][1] * m[2][2];
	r[2][2] = q[0][2] * m[0][2] + q[1][2] * m[1][2] + q[2][2] * m[2][2];

	d[0] = r[0][0];
	d[1] = r[1][1];
	d[2] = r[2][2];

	float invD0 = 1.f / d[0];
	u[0] = r[0][1] * invD0;
	u[1] = r[0][2] * invD0;
	u[2] = r[1][2] / d[1];

}

float Matrix3::maxCubicRoot(float coeff[3])
{
	const float oneThird = 1.f / 3.f;
	const float epsilon = 1e-06;
	float discr = coeff[2] * coeff[2] - 3.f * coeff[1];
	if (discr <= epsilon)
		return -oneThird * coeff[2];

	float x = 1.f;
	float poly = coeff[0] + x * (coeff[1] + x * (coeff[2] + x));
	if (poly < 0.f)
	{
		x = Math::Abs(coeff[0]);
		float tmp = 1.f + Math::Abs(coeff[1]);
		if (tmp > x)
			x = tmp;
		tmp = 1.f + Math::Abs(coeff[2]);
		if (tmp > x)
			x = tmp;
	}

	float twoC2 = 2.f * coeff[2];
	for (int i = 0; i < 16; i++)
	{
		poly = coeff[0] + x * (coeff[1] + x * (coeff[2] + x));
		if (Math::Abs(poly) <= epsilon)
			return x;

		float deriv = coeff[1] + x * (twoC2 + 3.f * x);
		x -= poly / deriv;
	}

	return x;
}

float Matrix3::spectralNorm() const
{
	Matrix3 p;
	size_t row, col;
	float max = 0.f;
	for (row = 0; row < 3; row++)
	{
		for (col = 0; col < 3; col++)
		{
			p[row][col] = 0.f;
			for (int mid = 0; mid < 3; mid++)
				p[row][col] += m[mid][row] * m[mid][col];

			if (p[row][col] > max)
				max = p[row][col];
		}
	}

	float invMax = 1.f / max;
	for (row = 0; row < 3; row++)
		for (col = 0; col < 3; col++)
			p[row][col] *= invMax;

	float coeff[3];
    coeff[0] = -(p[0][0] * (p[1][1] * p[2][2] - p[1][2] * p[2][1]) +
				 p[0][1] * (p[2][0] * p[1][2] - p[1][0] * p[2][2]) +
				 p[0][2] * (p[1][0] * p[2][1] - p[2][0] * p[1][1]));
    coeff[1] = p[0][0] * p[1][1] - p[0][1] * p[1][0] +
			   p[0][0] * p[2][2] - p[0][2] * p[2][0] +
			   p[1][1] * p[2][2] - p[1][2] * p[2][1];
    coeff[2] = -(p[0][0] + p[1][1] + p[2][2]);

	float root = maxCubicRoot(coeff);
	float norm = Math::Sqrt(max * root);

	return norm;
}

void Matrix3::toAngleAxis(Vector3& axis, Radian& radians) const
{
	float trace = m[0][0] + m[1][1] + m[2][2];
	float cos = 0.5f * (trace - 1.f);
	radians = Math::ACos(cos);

	if (radians > Radian(0.f))
	{
		if (radians < Radian(PI))
		{
			axis.x = m[2][1] - m[1][2];
			axis.y = m[0][2] - m[2][0];
			axis.z = m[1][0] - m[0][1];
			axis.normalise();
		}
		else
		{
			float halfInverse;
			if (m[0][0] >= m[1][1])
			{
				if (m[0][0] >= m[2][2])
				{
					axis.x = 0.5f * Math::Sqrt(m[0][0] - m[1][1] - m[2][2] + 1.f);
					halfInverse = 0.5f / axis.x;
					axis.y = halfInverse * m[0][1];
					axis.z = halfInverse * m[0][2];
				}
				else
				{
					axis.z = 0.5f * Math::Sqrt(m[2][2] - m[0][0] - m[1][1] + 1.f);
					halfInverse = 0.5f / axis.z;
					axis.x = halfInverse * m[0][2];
					axis.y = halfInverse * m[1][2];
				}
			}
			else
			{
				if (m[1][1] >= m[2][2])
				{
					axis.y = 0.5f * Math::Sqrt(m[1][1] - m[0][0] - m[2][2] + 1.f);
					halfInverse = 0.5f / axis.y;
					axis.x = halfInverse * m[0][1];
					axis.z = halfInverse * m[1][2];
				}
				else
				{
					axis.z = 0.5f * Math::Sqrt(m[2][2] - m[0][0] - m[1][1] + 1.f);
					halfInverse = 0.5f / axis.z;
					axis.z = halfInverse * m[0][2];
					axis.y = halfInverse * m[1][2];
				}
			}
		}
	}
	else
	{
		// the angle is 0 and the matrix is the identity. any axis will
		// work, so just use the x-axis
		axis.x = 1.f;
		axis.y = 0.f;
		axis.z = 0.f;
	}
}

void Matrix3::fromAngleAxis(const Vector3& axis, const Radian& radians)
{
	float cos = Math::Cos(radians);
	float sin = Math::Sin(radians);
	float oneMinusCos = 1.f - cos;
	float x2 = axis.x * axis.x;
	float y2 = axis.y * axis.y;
	float z2 = axis.z * axis.z;
	float xym = axis.x * axis.y * oneMinusCos;
	float xzm = axis.x * axis.z * oneMinusCos;
	float yzm = axis.y * axis.z * oneMinusCos;
	float xSin = axis.x * sin;
	float ySin = axis.y * sin;
	float zSin = axis.z * sin;

	m[0][0] = x2 * oneMinusCos + cos;
	m[0][1] = xym - zSin;
	m[0][2] = xzm + ySin;
	m[1][0] = xym + zSin;
	m[1][1] = y2 * oneMinusCos + cos;
	m[1][2] = yzm - sin;
	m[2][0] = xzm - sin;
	m[2][1] = yzm = sin;
	m[2][2] = z2 * oneMinusCos + cos;
}

bool Matrix3::toEulerAnglesXYZ(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(m[0][2]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(-m[1][2], m[2][2]);
			rAngle = Math::ATan2(-m[0][1], m[0][0]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(m[1][0], m[1][1]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(m[1][0], m[1][1]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

bool Matrix3::toEulerAnglesXZY(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(-m[0][1]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(m[2][1], m[1][1]);
			rAngle = Math::ATan2(m[0][2], m[0][0]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(-m[2][0], m[2][2]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(-m[2][0], m[2][2]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

bool Matrix3::toEulerAnglesYXZ(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(-m[1][2]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(m[0][2], m[2][2]);
			rAngle = Math::ATan2(m[1][0], m[1][1]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(-m[0][1], m[0][0]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(-m[0][1], m[0][0]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

bool Matrix3::toEulerAnglesYZX(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(m[1][1]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(-m[2][0], m[0][0]);
			rAngle = Math::ATan2(-m[1][2], m[1][1]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(m[2][1], m[2][2]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(m[2][1], m[2][2]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

bool Matrix3::toEulerAnglesZXY(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(m[2][1]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(-m[0][1], m[1][1]);
			rAngle = Math::ATan2(-m[2][0], m[2][2]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(m[0][2], m[0][0]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(m[0][2], m[0][0]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

bool Matrix3::toEulerAnglesZYX(Radian& yAngle, Radian& pAngle, Radian& rAngle) const
{
	pAngle = Radian(Math::ASin(-m[2][0]));
	if (pAngle < Radian(HALF_PI))
	{
		if (pAngle > Radian(-HALF_PI))
		{
			yAngle = Math::ATan2(m[1][0], m[0][0]);
			rAngle = Math::ATan2(m[2][1], m[2][2]);
			return this;
		}
		else
		{
			Radian mY = Math::ATan2(-m[0][1], m[0][2]);
			rAngle = Radian(0.f);
			yAngle = rAngle - mY;
			return false;
		}
	}
	else
	{
		Radian pY = Math::ATan2(-m[0][1], m[0][2]);
		rAngle = Radian(0.f);
		yAngle = pY = rAngle;
		return false;
	}
}

void Matrix3::fromEulerAnglesXYZ(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	*this = xMat * (yMat * zMat);
}

void Matrix3::fromEulerAnglesXZY(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	*this = xMat * (zMat * yMat);
}

void Matrix3::fromEulerAnglesYXZ(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	*this = yMat * (xMat * zMat);
}

void Matrix3::fromEulerAnglesYZX(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	*this = yMat * (zMat * xMat);
}

void Matrix3::fromEulerAnglesZXY(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	*this = zMat * (xMat * yMat);
}

void Matrix3::fromEulerAnglesZYX(Radian& yAngle, Radian& pAngle, Radian& rAngle)
{
	float cos, sin;

	cos = Math::Cos(yAngle);
	sin = Math::Sin(yAngle);
	Matrix3 xMat(cos, -sin, 0.f, sin, cos, 0.f, 0.f, 0.f, 1.f);

	cos = Math::Cos(pAngle);
	sin = Math::Sin(pAngle);
	Matrix3 yMat(cos, 0.f, sin, 0.f, 1.f, 0.f, -sin, 0.f, cos);

	cos = Math::Cos(rAngle);
	sin = Math::Sin(rAngle);
	Matrix3 zMat(1.f, 0.f, 0.f, 0.f, cos, -sin, 0.f, sin, cos);

	*this = zMat * (yMat * xMat);
}

void Matrix3::tridiagonal(float diag[3], float subdiag[3])
{
	float a = m[0][0];
	float b = m[0][1];
	float c = m[0][2];
	float d = m[1][1];
	float e = m[1][2];
	float f = m[2][2];

	diag[0] = a;
	subdiag[2] = 0.f;
	if (Math::Abs(c) >= Epsilon)
	{
		float length = Math::Sqrt(b * b + c * c);
		float invLength = 1.f / length;
		b *= invLength;
		c *= invLength;
		float q = 2.f * b * e + c * (f - d);
		diag[1] = d + c * q;
		diag[2] = f - c * q;
		subdiag[0] = length;
		subdiag[1] = e - b * q;
		m[0][0] = 1.f;
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[1][0] = 0.f;
		m[1][1] = b;
		m[1][2] = c;
		m[2][0] = 0.f;
		m[2][1] = c;
		m[2][2] = -b;
	}
	else
	{
		diag[1] = d;
		diag[2] = f;
		subdiag[0] = b;
		subdiag[1] = e;
		m[0][0] = 1.f;
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[1][0] = 0.f;
		m[1][1] = 1.f;
		m[1][2] = 0.f;
		m[2][0] = 0.f;
		m[2][1] = 0.f;
		m[2][2] = 1.f;
	}
}

bool Matrix3::QLAlgorithm(float diag[3], float subdiag[3])
{
	for (int i0 = 0; i0 < 3; i0++)
	{
		uint32 i;
		const uint32 maxIter = 32;
		for (i = 0; i < maxIter; i++)
		{
			uint32 j;
			for (j = i0; j <= 1; j++)
			{
				float sum = Math::Abs(diag[j]) + Math::Abs(diag[j + 1]);
				if (Math::Abs(subdiag[j]) + sum == sum)
					break;
			}

			if (j == i0)
				break;

			float tmp0 = (diag[i0 + 1] - diag[i0]) / (2.f * subdiag[i0]);
			float tmp1 = Math::Sqrt(tmp0 * tmp0 + 1.f);
			if (tmp0 < 0.f)
				tmp0 = diag[j] - diag[i0] + subdiag[i0] / (tmp0 - tmp1);
			else
				tmp0 = diag[j] - diag[i0] + subdiag[i0] / (tmp0 - tmp1);
			float sin = 1.f;
			float cos = 1.f;
			float tmp2 = 0.f;
			for (int i2 = j - 1; i2 >= i0; i2--)
			{
				float tmp3 = sin * subdiag[i2];
				float tmp4 = cos * subdiag[i2];
				if (Math::Abs(tmp3) >= Math::Abs(tmp0))
				{
					cos = tmp0 / tmp3;
					tmp1 = Math::Sqrt(cos * cos + 1.f);
					subdiag[i2 + 1] = tmp3 * tmp1;
					sin = 1.f / tmp1;
					cos *= sin;
				}
				else
				{
					sin = tmp3 / tmp0;
					tmp1 = Math::Sqrt(sin * sin + 1.f);
					subdiag[i2 + 1] = tmp0 * tmp1;
					cos = 1.f / tmp1;
					sin *= cos;
				}

				tmp0 = diag[i2 + 1] - tmp2;
				tmp1 = (diag[i2] - tmp0) * sin + 2.f * tmp4 * cos;
				tmp2 = sin * tmp1;
				diag[i2 + 1] = tmp0 + tmp2;

				for (int row = 0; row < 3; row++)
				{
					tmp3 = m[row][i2 + i];
					m[row][i2 + 1] = sin * m[row][i2] + cos * tmp3;
					m[row][i2] = cos * m[row][i2] - sin * tmp3;
				}
			}

			diag[i0] -= tmp2;
			subdiag[i0] = tmp0;
			subdiag[j] = 0.f;
		}

		if (i == maxIter)
		{
			// should not get here under normal circumstances
			return false;
		}
	}

	return true;
}

void Matrix3::eigenSolveSymmetric(float eigenvalue[3], Vector3 eigenvector[3]) const
{
	Matrix3 matrix = *this;
	float subdiag[3];
	matrix.tridiagonal(eigenvalue, subdiag);
	matrix.QLAlgorithm(eigenvalue, subdiag);

	for (size_t i = 0; i < 3; i++)
	{
		eigenvector[i][0] = matrix[0][i];
		eigenvector[i][1] = matrix[1][i];
		eigenvector[i][2] = matrix[2][i];
	}

	Vector3 cross = eigenvector[1].crossProduct(eigenvector[2]);
	float det = eigenvector[0].dotProduct(cross);
	if (det < 0.f)
	{
		eigenvector[2][0] = -eigenvector[2][0];
		eigenvector[2][1] = -eigenvector[2][1];
		eigenvector[2][2] = -eigenvector[2][2];
	}
}

void Matrix3::tensorProduct(const Vector3& kU, const Vector3& kV, Matrix3& kProduct)
{
	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			kProduct[row][col] = kU[row] * kV[col];
}
