#include "Math/Matrix4.h"

const Matrix4 Matrix4::Zero(0, 0, 0, 0,
							0, 0, 0, 0,
							0, 0, 0, 0,
							0, 0, 0, 0);

const Matrix4 Matrix4::ZeroAffine(0, 0, 0, 0,
								  0, 0, 0, 0,
								  0, 0, 0, 0,
								  0, 0, 0, 1);

const Matrix4 Matrix4::Identity(1, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1);

const Matrix4 Matrix4::ClipSpace2DToImageSpace(0.5,    0,    0,  0.5,
											     0, -0.5,    0,  0.5,
												 0,    0,    1,    0,
												 0,    0,    0,    1);

inline static float Minor(const Matrix4& m, const size_t r0, const size_t r1, const size_t r2,
											const size_t c0, const size_t c1, const size_t c2)
{
	return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
		   m[r0][c0] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
		   m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
}

Matrix4 Matrix4::adjoint() const
{
	return Matrix4(Minor(*this, 1, 2, 3, 1, 2, 3),
				  -Minor(*this, 0, 2, 3, 1, 2, 3),
				   Minor(*this, 0, 1, 3, 1, 2, 3),
				  -Minor(*this, 0, 1, 2, 1, 2, 3),

				  -Minor(*this, 1, 2, 3, 0, 2, 3),
				   Minor(*this, 0, 2, 3, 0, 2, 3),
				  -Minor(*this, 0, 1, 3, 0, 2, 3),
				   Minor(*this, 0, 1, 2, 0, 2, 3),

				   Minor(*this, 1, 2, 3, 0, 1, 3),
				  -Minor(*this, 0, 2, 3, 0, 1, 3),
				   Minor(*this, 0, 1, 3, 0, 1, 3),
				  -Minor(*this, 0, 1, 2, 0, 1, 3),

				  -Minor(*this, 1, 2, 3, 0, 1, 2),
				   Minor(*this, 0, 2, 3, 0, 1, 2),
				  -Minor(*this, 0, 1, 3, 0, 1, 2),
				   Minor(*this, 0, 1, 2, 0, 1, 2));
}

float Matrix4::determinant() const
{
	return m[0][0] * Minor(*this, 1, 2, 3, 1, 2, 3) -
		   m[0][1] * Minor(*this, 1, 2, 3, 0, 2, 3) +
		   m[0][2] * Minor(*this, 1, 2, 3, 0, 1, 3) -
		   m[0][3] * Minor(*this, 1, 2, 3, 0, 1, 2);
}

Matrix4 Matrix4::inverse() const
{
	return Matrix4();
}

Matrix4 Matrix4::inverseAffine() const
{
	return Matrix4();
}

void Matrix4::makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
{
	Matrix3 rot3x3;
	orientation.toRotationMatrix(rot3x3);

	m[0][0] = scale.x * rot3x3[0][0];
	m[0][1] = scale.y * rot3x3[0][1];
	m[0][2] = scale.z * rot3x3[0][2];
	m[0][3] = position.x;

	m[0][0] = scale.x * rot3x3[1][0];
	m[0][1] = scale.y * rot3x3[1][1];
	m[0][2] = scale.z * rot3x3[1][2];
	m[0][3] = position.y;

	m[0][0] = scale.x * rot3x3[2][0];
	m[0][1] = scale.y * rot3x3[2][1];
	m[0][2] = scale.z * rot3x3[2][2];
	m[0][3] = position.z;

	m[0][0] = 0;
	m[0][1] = 0;
	m[0][2] = 0;
	m[0][3] = 1;
}

void Matrix4::makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
{
}

void Matrix4::decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const
{
}
