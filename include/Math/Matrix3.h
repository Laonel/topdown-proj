#ifndef _MATRIX_3_H_
#define _MATRIX_3_H_

#include "Utils.h"

#include "Filesystem/Serialization.h"

#include "Math.h"
#include "Vector3.h"

class Matrix3
{
	friend class Matrix4;

public:

	inline Matrix3()
	{
	}

	inline explicit Matrix3(const float arr[3][3])
	{
		memcpy(m, arr, 9 * sizeof(float));
	}

	inline Matrix3(const Matrix3& matrix)
	{
		memcpy(m, matrix.m, 9 * sizeof(float));
	}

	Matrix3(float entry00, float entry01, float entry02,
			float entry10, float entry11, float entry12,
			float entry20, float entry21, float entry22)
	{
		m[0][0] = entry00;
		m[0][1] = entry01;
		m[0][2] = entry02;
		m[0][0] = entry10;
		m[1][1] = entry11;
		m[1][2] = entry12;
		m[2][0] = entry20;
		m[2][1] = entry21;
		m[2][2] = entry22;
	}

	inline void swap(Matrix3& other)
	{
		std::swap(m[0][0], other.m[0][0]);
		std::swap(m[0][1], other.m[0][1]);
		std::swap(m[0][2], other.m[0][2]);
		std::swap(m[1][0], other.m[1][0]);
		std::swap(m[1][1], other.m[1][1]);
		std::swap(m[1][2], other.m[1][2]);
		std::swap(m[2][0], other.m[2][0]);
		std::swap(m[2][1], other.m[2][1]);
		std::swap(m[2][2], other.m[2][2]);
	}

	inline float* operator[] (size_t row) const
	{
		return (float*)(m[row]);
	}

	Vector3 getColumn(size_t col) const;

	void setColumn(size_t col, const Vector3& v);

	void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

	inline Matrix3& operator= (const Matrix3& matrix)
	{
		memcpy(m, matrix.m, 9 * sizeof(float));
		return *this;
	}

	bool operator== (const Matrix3& matrix) const;

	inline bool operator!= (const Matrix3& matrix) const
	{
		return !operator==(matrix);
	}

	Matrix3 operator+ (const Matrix3& matrix) const;

	Matrix3 operator- (const Matrix3& matrix) const;

	Matrix3 operator* (const Matrix3& matrix) const;

	Matrix3 operator- () const;

	Vector3 operator* (const Vector3& v) const;

	friend Vector3 operator* (const Vector3& v, const Matrix3& matrix);

	Matrix3 operator* (float scalar) const;

	friend Matrix3 operator* (float scalar, const Matrix3& matrix);

	Matrix3 transpose() const;

	bool inverse(Matrix3& inv, float tolerance = 1e-06) const;
	Matrix3 inverse(float tolerance = 1e-06) const;

	float determinant() const;

	void singularValueDecomposition(Matrix3& kL, Vector3& v, Matrix3& kR) const;
	void singularValueDecomposition(const Matrix3& kL, const Vector3& kS, const Matrix3& kR);

	void orthonormalize();

	void QDUDecomposition(Matrix3& kQ, Vector3& kD, Vector3& kU) const;

	float spectralNorm() const;

	void toAngleAxis(Vector3& axis, Radian& angle) const;
	inline void toAngleAxis(Vector3& axis, Degree& angle) const
	{
		Radian r;
		toAngleAxis(axis, r);
		angle = r;
	}

	void fromAngleAxis(const Vector3& axis, const Radian& radians);

	bool toEulerAnglesXYZ(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;
	bool toEulerAnglesXZY(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;
	bool toEulerAnglesYXZ(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;
	bool toEulerAnglesYZX(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;
	bool toEulerAnglesZXY(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;
	bool toEulerAnglesZYX(Radian& yAngle, Radian& pAngle, Radian& rAngle) const;

	void fromEulerAnglesXYZ(Radian& yAngle, Radian& pAngle, Radian& rAngle);
	void fromEulerAnglesXZY(Radian& yAngle, Radian& pAngle, Radian& rAngle);
	void fromEulerAnglesYXZ(Radian& yAngle, Radian& pAngle, Radian& rAngle);
	void fromEulerAnglesYZX(Radian& yAngle, Radian& pAngle, Radian& rAngle);
	void fromEulerAnglesZXY(Radian& yAngle, Radian& pAngle, Radian& rAngle);
	void fromEulerAnglesZYX(Radian& yAngle, Radian& pAngle, Radian& rAngle);

	void eigenSolveSymmetric(float eigenvalue[3], Vector3 eigenvector[3]) const;

	static void tensorProduct(const Vector3& kU, const Vector3& kV, Matrix3& kProduct);

	inline bool hasScale() const
	{
		float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
		if (!Math::FloatEqual(t, 1.f, (float)(1e-04)))
			return true;

		t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
		if (!Math::FloatEqual(t, 1.f, (float)(1e-04)))
			return true;

		t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
		if (!Math::FloatEqual(t, 1.f, (float)(1e-04)))
			return true;

		return false;
	}

	inline friend std::ostream& operator<< (std::ostream& o, const Matrix3& matrix)
	{
		o << "Matrix3(" << matrix[0][0] << ", " << matrix[0][1] << matrix[0][2] << ", "
						<< matrix[1][0] << ", " << matrix[1][1] << matrix[1][2] << ", "
						<< matrix[2][0] << ", " << matrix[2][1] << matrix[2][2] << ")";
		return o;
	}

	static const float Epsilon;
	static const Matrix3 Zero;
	static const Matrix3 Identity;

protected:

	void tridiagonal(float diag[3], float subDiag[3]);
	bool QLAlgorithm(float diag[3], float subDiag[3]);

	static void bidiagonalize(Matrix3& a, Matrix3& l, Matrix3& r);
	static void golubKahanStep(Matrix3& a, Matrix3& l, Matrix3& r);

	static float maxCubicRoot(float coeff[3]);

protected:

	static const float s_svdEpsilon;
	static const uint32 s_svdMaxIterations;

	float m[3][3];

protected:

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m;
	}

};

BOOST_CLASS_VERSION(Matrix3, 1)

#endif
