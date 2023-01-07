//#pragma once
//
//#define _USE_MATH_DEFINES 
//#include <math.h>
//#pragma warning(push)
//#pragma warning(disable:4201) // anonymous struct warning
//
//struct Vector3
//{
//    float x, y, z;
//};
//
//union Vector4
//{
//    struct {
//        float x, y, z, w;
//    };
//    struct {
//        Vector3 xyz;
//    };
//};
//#pragma warning(pop)
//
//struct Vector3x3
//{
//    // We internally do 3x4 for HLSL-friendly alignment
//    float m[3][4];
//};
//
//union Matrix4
//{
//    float m[4][4];
//    Vector4 cols[4];
//
//    inline Vector4 row(int i) { // Returns i-th row of matrix
//        return { m[0][i], m[1][i], m[2][i], m[3][i] };
//    }
//};
//
//inline float degreesToRadians(float degs) {
//    return degs * ((float)M_PI / 180.0f);
//}
//
//inline float length(Vector3 v) {
//    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
//}
//
//inline float length(Vector4 v) {
//    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
//}
//
//inline float dot(Vector4 a, Vector4 b) {
//    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
//}
//
//inline Vector3 operator* (Vector3 v, float f) {
//    return { v.x * f, v.y * f, v.z * f };
//}
//
//inline Vector4 operator* (Vector4 v, float f) {
//    return { v.x * f, v.y * f, v.z * f, v.w * f };
//}
//
//inline Vector3 normalise(Vector3 v) {
//    return v * (1.f / length(v));
//}
//
//inline Vector4 normalise(Vector4 v) {
//    return v * (1.f / length(v));
//}
//
//inline Vector3 cross(Vector3 a, Vector3 b) {
//    return {
//        a.y * b.z - a.z * b.y,
//        a.z * b.x - a.x * b.z,
//        a.x * b.y - a.y * b.x
//    };
//}
//
//inline Vector3 operator+= (Vector3& lhs, Vector3 rhs) {
//    lhs.x += rhs.x;
//    lhs.y += rhs.y;
//    lhs.z += rhs.z;
//    return lhs;
//}
//
//inline Vector3 operator-= (Vector3& lhs, Vector3 rhs) {
//    lhs.x -= rhs.x;
//    lhs.y -= rhs.y;
//    lhs.z -= rhs.z;
//    return lhs;
//}
//
//inline Vector3 operator- (Vector3 v) {
//    return { -v.x, -v.y, -v.z };
//}
//
//inline Matrix4 scaleMat(float s) {
//    return {
//        s, 0, 0, 0,
//        0, s, 0, 0,
//        0, 0, s, 0,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 scaleMat(Vector3 s) {
//    return {
//        s.x, 0, 0, 0,
//        0, s.y, 0, 0,
//        0, 0, s.z, 0,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 rotateXMat(float rad) {
//    float sinTheta = sinf(rad);
//    float cosTheta = cosf(rad);
//    return {
//        1, 0, 0, 0,
//        0, cosTheta, -sinTheta, 0,
//        0, sinTheta, cosTheta, 0,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 rotateYMat(float rad) {
//    float sinTheta = sinf(rad);
//    float cosTheta = cosf(rad);
//    return {
//        cosTheta, 0, sinTheta, 0,
//        0, 1, 0, 0,
//        -sinTheta, 0, cosTheta, 0,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 rotateZMat(float rad) {
//    float sinTheta = sinf(rad);
//    float cosTheta = cosf(rad);
//    return {
//        cosTheta, -sinTheta, 0, 0,
//        sinTheta, cosTheta, 0, 0,
//        0, 0, 1, 0,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 translationMat(Vector3 trans) {
//    return {
//        1, 0, 0, trans.x,
//        0, 1, 0, trans.y,
//        0, 0, 1, trans.z,
//        0, 0, 0, 1
//    };
//}
//
//inline Matrix4 makePerspectiveMat(float aspectRatio, float fovYRadians, float zNear, float zFar)
//{
//    // float yScale = 1 / tanf(0.5f * fovYRadians); 
//    // NOTE: 1/tan(X) = tan(90degs - X), so we can avoid a divide
//    // float yScale = tanf((0.5f * M_PI) - (0.5f * fovYRadians));
//    float yScale = tanf(0.5f * ((float)M_PI - fovYRadians));
//    float xScale = yScale / aspectRatio;
//    float zRangeInverse = 1.f / (zNear - zFar);
//    float zScale = zFar * zRangeInverse;
//    float zTranslation = zFar * zNear * zRangeInverse;
//
//    Matrix4 result = {
//        xScale, 0, 0, 0,
//        0, yScale, 0, 0,
//        0, 0, zScale, zTranslation,
//        0, 0, -1, 0
//    };
//    return result;
//}
//
//inline Matrix4 operator* (Matrix4 a, Matrix4 b) {
//    return {
//        dot(a.row(0), b.cols[0]),
//        dot(a.row(1), b.cols[0]),
//        dot(a.row(2), b.cols[0]),
//        dot(a.row(3), b.cols[0]),
//        dot(a.row(0), b.cols[1]),
//        dot(a.row(1), b.cols[1]),
//        dot(a.row(2), b.cols[1]),
//        dot(a.row(3), b.cols[1]),
//        dot(a.row(0), b.cols[2]),
//        dot(a.row(1), b.cols[2]),
//        dot(a.row(2), b.cols[2]),
//        dot(a.row(3), b.cols[2]),
//        dot(a.row(0), b.cols[3]),
//        dot(a.row(1), b.cols[3]),
//        dot(a.row(2), b.cols[3]),
//        dot(a.row(3), b.cols[3])
//    };
//}
//
//inline Vector4 operator* (Vector4 v, Matrix4 m) {
//    return {
//        dot(v, m.cols[0]),
//        dot(v, m.cols[1]),
//        dot(v, m.cols[2]),
//        dot(v, m.cols[3])
//    };
//}
//
//inline Matrix4 transpose(Matrix4 m) {
//    return Matrix4{
//        m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
//        m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
//        m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
//        m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
//    };
//}
//
//inline Matrix4 inverse(Matrix4 mat)
//{
//    Matrix4 matrix;
//    float s0 = mat.m[0][0] * mat.m[1][1] - mat.m[1][0] * mat.m[0][1];
//    float s1 = mat.m[0][0] * mat.m[1][2] - mat.m[1][0] * mat.m[0][2];
//    float s2 = mat.m[0][0] * mat.m[1][3] - mat.m[1][0] * mat.m[0][3];
//    float s3 = mat.m[0][0] * mat.m[1][2] - mat.m[1][1] * mat.m[0][2];
//    float s4 = mat.m[0][0] * mat.m[1][3] - mat.m[1][1] * mat.m[0][3];
//    float s5 = mat.m[0][0] * mat.m[1][3] - mat.m[1][2] * mat.m[0][3];
//                             
//    float c5 = mat.m[0][0] * mat.m[3][3] - mat.m[3][2] * mat.m[2][3];
//    float c4 = mat.m[0][0] * mat.m[3][3] - mat.m[3][1] * mat.m[2][3];
//    float c3 = mat.m[0][0] * mat.m[3][2] - mat.m[3][1] * mat.m[2][2];
//    float c2 = mat.m[0][0] * mat.m[3][3] - mat.m[3][0] * mat.m[2][3];
//    float c1 = mat.m[0][0] * mat.m[3][2] - mat.m[3][0] * mat.m[2][2];
//    float c0 = mat.m[0][0] * mat.m[3][1] - mat.m[3][0] * mat.m[2][1];
//
//    float invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
//
//    matrix.m[0][0] = (mat.m[1][1] * c5 - mat.m[1][2] * c4 + mat.m[1][3] * c3) * invdet;
//    matrix.m[0][1] =(-mat.m[0][1] * c5 + mat.m[0][2] * c4 - mat.m[0][3] * c3) * invdet;
//    matrix.m[0][2] = (mat.m[3][1] * s5 - mat.m[3][2] * s4 + mat.m[3][3] * s3) * invdet;
//    matrix.m[0][3] =(-mat.m[2][1] * s5 + mat.m[2][2] * s4 - mat.m[2][3] * s3) * invdet;
//     
//    matrix.m[1][0] =(-mat.m[1][0] * c5 + mat.m[1][2] * c2 - mat.m[1][3] * c1) * invdet;
//    matrix.m[1][1] = (mat.m[0][0] * c5 - mat.m[0][2] * c2 + mat.m[0][3] * c1) * invdet;
//    matrix.m[1][2] =(-mat.m[3][0] * s5 + mat.m[3][2] * s2 - mat.m[3][3] * s1) * invdet;
//    matrix.m[1][3] = (mat.m[2][0] * s5 - mat.m[2][2] * s2 + mat.m[2][3] * s1) * invdet;
//       
//    matrix.m[2][0] = (mat.m[1][0] * c4 - mat.m[1][1] * c2 + mat.m[1][3] * c0) * invdet;
//    matrix.m[2][1] =(-mat.m[0][0] * c4 + mat.m[0][1] * c2 - mat.m[0][3] * c0) * invdet;
//    matrix.m[2][2] = (mat.m[3][0] * s4 - mat.m[3][1] * s2 + mat.m[3][3] * s0) * invdet;
//    matrix.m[2][3] =(-mat.m[2][0] * s4 + mat.m[2][1] * s2 - mat.m[2][3] * s0) * invdet;
//         
//    matrix.m[3][0] =(-mat.m[1][0] * c3 + mat.m[1][1] * c1 - mat.m[1][2] * c0) * invdet;
//    matrix.m[3][1] = (mat.m[0][0] * c3 - mat.m[0][1] * c1 + mat.m[0][2] * c0) * invdet;
//    matrix.m[3][2] =(-mat.m[3][0] * s3 + mat.m[3][1] * s1 - mat.m[3][2] * s0) * invdet;
//    matrix.m[3][3] = (mat.m[2][0] * s3 - mat.m[2][1] * s1 + mat.m[2][2] * s0) * invdet;
//    return matrix;
//}
//
//inline Vector3x3 Matrix4ToVector3x3(Matrix4 m) {
//    Vector3x3 result = {
//        m.m[0][0], m.m[0][1], m.m[0][2], 0.0,
//        m.m[1][0], m.m[1][1], m.m[1][2], 0.0,
//        m.m[2][0], m.m[2][1], m.m[2][2], 0.0
//    };
//    return result;
//}

#pragma once
#include <math.h>
#include <xmmintrin.h>
#define M_PI 3.14159265358979323846
//#define USE_INTRINSICS


////Uniform vector types to be sent to to shader for byte allignment
//struct UniformFloat2
//{
//	float x, y;
//
//
//};
//
//struct UniformFloat3
//{
//	//need to leave it 16 byte alligned
//	float x, y, z, padding;
//
//
//};
//
//struct UniformFloat4
//{
//	float x, y, z, w;
//
//};

#ifdef USE_INTRINSICS
struct float4
{
	float x, y, z, w;
};

struct float3
{
	float x, y, z, padding0;
};

struct float2
{
	float x, y;
};



union Vector4
{
	__m128 intrins;
	float4 data;

	Vector4()
	{
		intrins = _mm_set1_ps(0.0f);
	}

	Vector4(float x, float y, float z, float w)
	{
		data.x = x;
		data.y = y;
		data.z = z;
		data.w = w;
	}

	Vector4(const Vector4& vector)
	{
		intrins = vector.intrins;
	}

	void operator=(const Vector4& other)
	{
		intrins = other.intrins;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec)
	{
		os << "x: " << vec.data.x << " y: " << vec.data.y << " z: " << vec.data.z << " w: " << vec.data.w;
		return os;
	}

	float& operator[](int i)
	{
		switch (i)
		{
		case 0:
			return data.x;
			break;
		case 1:
			return data.y;
			break;
		case 2:
			return data.z;
			break;
		case 3:
			return data.w;
			break;
		}
	}

	float x()
	{
		return data.x;
	}

	float y()
	{
		return data.y;
	}

	float z()
	{
		return data.z;
	}

	float w()
	{
		return data.w;
	}
};

union Vector3
{
	__m128 intrins;
	float3 data;

	Vector3()
	{
		intrins = _mm_set1_ps(0.0f);
	}

	Vector3(float x, float y, float z)
	{
		data.x = x;
		data.y = y;
		data.z = z;
	}

	Vector3(const Vector3& vector)
	{
		intrins = vector.intrins;
	}



	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec)
	{
		os << "x: " << vec.data.x << " y: " << vec.data.y << " z: " << vec.data.z;
		return os;
	}

	void operator=(const Vector3& vec)
	{
		intrins = vec.intrins;
	}

	void operator=(const Vector4& other)
	{
		intrins = other.intrins;
	}

	float x() const
	{
		return data.x;
	}

	float y() const
	{
		return data.y;
	}

	float z() const
	{
		return data.z;
	}

	Vector3 abs()
	{
		return Vector3{ fabsf(data.x), fabsf(data.y), fabsf(data.z) };
	}

};

union Vector2
{
	__m128 intrins;
	float2 data;

	Vector2()
	{
		intrins = _mm_set1_ps(0.0f);
	}

	Vector2(float x, float y)
	{
		data.x = x;
		data.y = y;
	}

	Vector2(const Vector2& vector)
	{
		intrins = vector.intrins;
	}

	void operator=(const Vector2& other)
	{
		intrins = other.intrins;
	}

	//Vector2 operator=(const Vector2& )

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec)
	{
		os << "x: " << vec.data.x << " y: " << vec.data.y;
		return os;
	}

	float x()
	{
		return data.x;
	}

	float y()
	{
		return data.y;
	}
};


struct Matrix3
{
	Vector4 mat[3];

	Matrix3(float f)
	{
		mat[0] = { f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, f, 0.0f };
	}

	Matrix3()
	{
		mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	}


	inline Vector3 row(int i)
	{
		switch (i)
		{
		case 0:
			return { mat[0].data.x,mat[1].data.x,mat[2].data.x };
			break;
		case 1:
			return { mat[0].data.y,mat[1].data.y,mat[2].data.y };
			break;
		case 2:
			return { mat[0].data.z,mat[1].data.z,mat[2].data.z };
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix3& mat)
	{
		os << mat.mat[0] << std::endl << mat.mat[1] << std::endl << mat.mat[2];
		return os;
	}
};

struct Matrix4
{
	Vector4 mat[4];

	Matrix4(float f)
	{
		mat[0] = { f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, f, 0.0f };
		mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Matrix4()
	{
		mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
		mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Matrix3 ToMatrix3()
	{
		Matrix3 result;
		result.mat[0] = mat[0];
		result.mat[1] = mat[1];
		result.mat[2] = mat[2];
		return result;
	}


	inline Vector4 row(int i)
	{
		switch (i)
		{
		case 0:
			return { mat[0].data.x,mat[1].data.x,mat[2].data.x,mat[3].data.x };
			break;
		case 1:
			return { mat[0].data.y,mat[1].data.y,mat[2].data.y,mat[3].data.y };
			break;
		case 2:
			return { mat[0].data.z,mat[1].data.z,mat[2].data.z,mat[3].data.z };
			break;
		case 3:
			return { mat[0].data.w,mat[1].data.w,mat[2].data.w,mat[3].data.w };
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix4& mat)
	{
		os << mat.mat[0] << std::endl << mat.mat[1] << std::endl << mat.mat[2] << std::endl << mat.mat[3];
		return os;
	}
};





inline Vector2 operator+(Vector2& a, Vector2& b)
{
	Vector2 result;
	result.intrins = _mm_add_ps(a.intrins, b.intrins);
	return result;
}

inline Vector3 operator+(Vector3& a, Vector3& b)
{
	Vector3 result;
	result.intrins = _mm_add_ps(a.intrins, b.intrins);
	return result;
}

inline Vector4 operator+(Vector4& a, Vector4& b)
{
	Vector4 result;
	result.intrins = _mm_add_ps(a.intrins, b.intrins);
	return result;
}

inline Vector2 operator-(Vector2& a, Vector2& b)
{
	Vector2 result;
	result.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return result;
}

inline Vector3 operator-(Vector3& a, Vector3& b)
{
	Vector3 result;
	result.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return result;
}

inline Vector4 operator-(Vector4& a, Vector4& b)
{
	Vector4 result;
	result.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return result;
}

//
inline Vector2 operator+=(Vector2& a, Vector2 b)
{
	a.intrins = _mm_add_ps(a.intrins, b.intrins);
	return a;
}

inline Vector3 operator+=(Vector3& a, Vector3 b)
{
	a.intrins = _mm_add_ps(a.intrins, b.intrins);
	return a;
}

inline Vector4 operator+=(Vector4& a, Vector4 b)
{
	a.intrins = _mm_add_ps(a.intrins, b.intrins);
	return a;
}

inline Vector2 operator-=(Vector2& a, Vector2 b)
{
	a.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return a;
}

inline Vector3 operator-=(Vector3& a, Vector3 b)
{
	a.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return a;
}

inline Vector4 operator-=(Vector4& a, Vector4 b)
{
	a.intrins = _mm_sub_ps(a.intrins, b.intrins);
	return a;
}
//

inline Vector3 operator-(Vector3 vec)
{
	const __m128 scalar = _mm_set1_ps(-1.0f);
	vec.intrins = _mm_mul_ps(vec.intrins, scalar);
	return vec;
}

inline float VectorDot(Vector2 a, Vector2 b)
{
	a.intrins = _mm_mul_ps(a.intrins, b.intrins);
	return a.data.x + a.data.y;
}

inline float VectorDot(Vector3 a, Vector3 b)
{
	a.intrins = _mm_mul_ps(a.intrins, b.intrins);
	return a.data.x + a.data.y + a.data.z;
}


inline float VectorDot(Vector4 a, Vector4 b)
{
	a.intrins = _mm_mul_ps(a.intrins, b.intrins);
	return a.data.x + a.data.y + a.data.z + a.data.w;
}

inline float VectorLength(Vector2 vec)
{
	vec.intrins = _mm_mul_ps(vec.intrins, vec.intrins);
	return sqrt(vec.data.x + vec.data.y);
}

inline float VectorLength(Vector3 vec)
{
	vec.intrins = _mm_mul_ps(vec.intrins, vec.intrins);
	return sqrt(vec.data.x + vec.data.y + vec.data.z);
}

inline float VectorLength(Vector4 vec)
{
	vec.intrins = _mm_mul_ps(vec.intrins, vec.intrins);
	return sqrt(vec.data.x + vec.data.y + vec.data.z + vec.data.w);
}

inline Vector3 VectorCross(Vector2 a, Vector2 b)
{
	return { 0, 0, a.data.x * b.data.y - a.data.y * b.data.x };
}

inline Vector3 VectorCross(Vector3 a, Vector3 b)
{
	return { a.data.y * b.data.z - a.data.z * b.data.y, a.data.x * b.data.z - a.data.z * b.data.x, a.data.x * b.data.y - a.data.y * b.data.x };
}

inline Vector3 operator*(Vector3 vec, float f)
{
	const __m128 scalar = _mm_set1_ps(f);
	vec.intrins = _mm_mul_ps(vec.intrins, scalar);
	return vec;
}

inline Vector3 VectorNormalize(Vector3 vec)
{
	float len = VectorLength(vec);
	return { vec.data.x / len, vec.data.y / len, vec.data.z / len };
}

inline Matrix4 MatrixTranslate(Vector3 translation)
{
	Matrix4 mat;
	mat.mat[0] = { 1.0f, 0.0f, 0.0f, translation.data.x };
	mat.mat[1] = { 0.0f, 1.0f, 0.0f, translation.data.y };
	mat.mat[2] = { 0.0f, 0.0f, 1.0f, translation.data.z };
	mat.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return mat;
}

inline Matrix4 MatrixScale(Vector3 scale)
{
	Matrix4 mat;
	mat.mat[0] = { scale.data.x, 0.0f, 0.0f, 0.0f };
	mat.mat[1] = { 0.0f, scale.data.y, 0.0f, 0.0f };
	mat.mat[2] = { 0.0f, 0.0f, scale.data.z, 0.0f };
	mat.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return mat;
}

inline Matrix4 MatrixXRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
	rot.mat[1] = { 0.0f, cosTheta, -sinTheta, 0.0f };
	rot.mat[2] = { 0.0f, sinTheta, cosTheta, 0.0f };
	rot.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return rot;
}

inline Matrix4 MatrixYRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { cosTheta, 0, sinTheta, 0 };
	rot.mat[1] = { 0, 1, 0, 0 };
	rot.mat[2] = { -sinTheta, 0, cosTheta, 0 };
	rot.mat[3] = { 0, 0, 0, 1 };
	return rot;
}

inline Matrix4 MatrixZRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { cosTheta, -sinTheta, 0, 0 };
	rot.mat[1] = { sinTheta, cosTheta, 0, 0 };
	rot.mat[2] = { 0, 0, 1, 0 };
	rot.mat[3] = { 0, 0, 0, 1 };
	return rot;
}

inline Matrix4 MatrixEuler(Vector3 euler)
{
	float sinAlpha = sinf(euler.data.z);
	float cosAlpha = cosf(euler.data.z);
	float sinBeta = sinf(euler.data.y);
	float cosBeta = cosf(euler.data.y);
	float sinGamma = sinf(euler.data.x);
	float cosGamma = cosf(euler.data.x);
	Matrix4 rot;
	rot.mat[0] = { cosAlpha * cosBeta, cosAlpha * sinBeta * sinGamma - sinAlpha * cosGamma, cosAlpha * sinBeta * cosGamma + sinAlpha * sinGamma, 0.0f };
	rot.mat[1] = { sinAlpha * cosBeta, sinAlpha * sinBeta * sinGamma + cosAlpha * cosGamma, sinAlpha * sinBeta * cosGamma - cosAlpha * sinGamma, 0.0f };
	rot.mat[2] = { -sinBeta, cosBeta * sinGamma, cosBeta * cosGamma, 0.0f };
	rot.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return rot;
}

inline Matrix4 operator*(Matrix4 a, Matrix4 b)
{
	Matrix4 mat;
	mat.mat[0] = { VectorDot(a.row(0),b.mat[0]), VectorDot(a.row(1),b.mat[0]) , VectorDot(a.row(2),b.mat[0]) , VectorDot(a.row(3),b.mat[0]) };
	mat.mat[1] = { VectorDot(a.row(0),b.mat[1]), VectorDot(a.row(1),b.mat[1]) , VectorDot(a.row(2),b.mat[1]) , VectorDot(a.row(3),b.mat[1]) };
	mat.mat[2] = { VectorDot(a.row(0),b.mat[2]), VectorDot(a.row(1),b.mat[2]) , VectorDot(a.row(2),b.mat[2]) , VectorDot(a.row(3),b.mat[2]) };
	mat.mat[3] = { VectorDot(a.row(0),b.mat[3]), VectorDot(a.row(1),b.mat[3]) , VectorDot(a.row(2),b.mat[3]) , VectorDot(a.row(3),b.mat[3]) };
	return mat;
}

inline Matrix4 MatrixInverse(Matrix4 matrix)
{
	Matrix4 result;
	float s0 = matrix.mat[0].data.x * matrix.mat[1].data.y - matrix.mat[1].data.x * matrix.mat[0].data.y;
	float s1 = matrix.mat[0].data.x * matrix.mat[1].data.z - matrix.mat[1].data.x * matrix.mat[0].data.z;
	float s2 = matrix.mat[0].data.x * matrix.mat[1].data.w - matrix.mat[1].data.x * matrix.mat[0].data.w;
	float s3 = matrix.mat[0].data.y * matrix.mat[1].data.z - matrix.mat[1].data.y * matrix.mat[0].data.z;
	float s4 = matrix.mat[0].data.y * matrix.mat[1].data.w - matrix.mat[1].data.y * matrix.mat[0].data.w;
	float s5 = matrix.mat[0].data.z * matrix.mat[1].data.w - matrix.mat[1].data.z * matrix.mat[0].data.w;

	float c5 = matrix.mat[2].data.z * matrix.mat[3].data.w - matrix.mat[3].data.z * matrix.mat[2].data.w;
	float c4 = matrix.mat[2].data.y * matrix.mat[3].data.w - matrix.mat[3].data.y * matrix.mat[2].data.w;
	float c3 = matrix.mat[2].data.y * matrix.mat[3].data.z - matrix.mat[3].data.y * matrix.mat[2].data.z;
	float c2 = matrix.mat[2].data.x * matrix.mat[3].data.w - matrix.mat[3].data.x * matrix.mat[2].data.w;
	float c1 = matrix.mat[2].data.x * matrix.mat[3].data.z - matrix.mat[3].data.x * matrix.mat[2].data.z;
	float c0 = matrix.mat[2].data.x * matrix.mat[3].data.y - matrix.mat[3].data.x * matrix.mat[2].data.y;

	float invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	result.mat[0][0] = (matrix.mat[1][1] * c5 - matrix.mat[1][2] * c4 + matrix.mat[1][3] * c3) * invdet;
	result.mat[0][1] = (-matrix.mat[0][1] * c5 + matrix.mat[0][2] * c4 - matrix.mat[0][3] * c3) * invdet;
	result.mat[0][2] = (matrix.mat[3][1] * s5 - matrix.mat[3][2] * s4 + matrix.mat[3][3] * s3) * invdet;
	result.mat[0][3] = (-matrix.mat[2][1] * s5 + matrix.mat[2][2] * s4 - matrix.mat[2][3] * s3) * invdet;

	result.mat[1][0] = (-matrix.mat[1][0] * c5 + matrix.mat[1][2] * c2 - matrix.mat[1][3] * c1) * invdet;
	result.mat[1][1] = (matrix.mat[0][0] * c5 - matrix.mat[0][2] * c2 + matrix.mat[0][3] * c1) * invdet;
	result.mat[1][2] = (-matrix.mat[3][0] * s5 + matrix.mat[3][2] * s2 - matrix.mat[3][3] * s1) * invdet;
	result.mat[1][3] = (matrix.mat[2][0] * s5 - matrix.mat[2][2] * s2 + matrix.mat[2][3] * s1) * invdet;

	result.mat[2][0] = (matrix.mat[1][0] * c4 - matrix.mat[1][1] * c2 + matrix.mat[1][3] * c0) * invdet;
	result.mat[2][1] = (-matrix.mat[0][0] * c4 + matrix.mat[0][1] * c2 - matrix.mat[0][3] * c0) * invdet;
	result.mat[2][2] = (matrix.mat[3][0] * s4 - matrix.mat[3][1] * s2 + matrix.mat[3][3] * s0) * invdet;
	result.mat[2][3] = (-matrix.mat[2][0] * s4 + matrix.mat[2][1] * s2 - matrix.mat[2][3] * s0) * invdet;

	result.mat[3][0] = (-matrix.mat[1][0] * c3 + matrix.mat[1][1] * c1 - matrix.mat[1][2] * c0) * invdet;
	result.mat[3][1] = (matrix.mat[0][0] * c3 - matrix.mat[0][1] * c1 + matrix.mat[0][2] * c0) * invdet;
	result.mat[3][2] = (-matrix.mat[3][0] * s3 + matrix.mat[3][1] * s1 - matrix.mat[3][2] * s0) * invdet;
	result.mat[3][3] = (matrix.mat[2][0] * s3 - matrix.mat[2][1] * s1 + matrix.mat[2][2] * s0) * invdet;
	return result;
}

inline Matrix4 MatrixTranspose(Matrix4 matrix)
{
	Matrix4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.mat[i][j] = matrix.mat[j][i];
		}
	}
	return result;
}

inline Matrix4 MatrixPerspective(float aspectRatio, float fov, float zNear, float zFar)
{
	float yScale = tanf(0.5f * ((float)M_PI - fov));
	float xScale = yScale / aspectRatio;
	float zRangeInverse = 1.f / (zNear - zFar);
	float zScale = zFar * zRangeInverse;
	float zTranslation = zFar * zNear * zRangeInverse;

	Matrix4 result;
	result.mat[0] = { xScale, 0, 0, 0 };
	result.mat[1] = { 0, yScale, 0, 0 };
	result.mat[2] = { 0, 0, zScale, zTranslation };
	result.mat[3] = { 0, 0, -1, 0 };
	return result;
}

inline float DegreesToRadians(float degrees)
{
	return degrees * ((float)M_PI / 180.0f);
}

#else
#include <ostream>

struct Vector3;
struct Vector4
{
	float x, y, z, w;

	Vector4()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4(const Vector4& vector)
	{
		this->x = vector.x;
		this->y = vector.y;
		this->z = vector.z;
		this->w = vector.w;
	}

	Vector4(Vector3& vector);

	Vector4(const Vector3& vector);


	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec)
	{
		os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z << " w: " << vec.w;
		return os;
	}

	float& operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		case 3:
			return w;
			break;
		}
	}
};

struct Quaternion;

struct Vector3
{
	float x, y, z;
	Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3(const Vector3& vector)
	{
		this->x = vector.x;
		this->y = vector.y;
		this->z = vector.z;
	}

	Vector3(const Vector4& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec)
	{
		os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z;
		return os;
	}

	void operator=(const Vector4& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
};




struct Vector2
{
	float x, y;

	Vector2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2(const Vector2& vector)
	{
		this->x = vector.x;
		this->y = vector.y;
	}

	//Vector2 operator=(const Vector2& )

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec)
	{
		os << "x: " << vec.x << " y: " << vec.y;
		return os;
	}
};


struct Matrix3x3
{
	Vector3 mat[3];

	Matrix3x3(float f)
	{
		mat[0] = { f, 0.0f, 0.0f };
		mat[1] = { 0.0f, f, 0.0f };
		mat[2] = { 0.0f, 0.0f, f };
	}

	Matrix3x3()
	{
		mat[0] = { 1.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f };
	}


	inline Vector3 row(int i)
	{
		switch (i)
		{
		case 0:
			return { mat[0].x,mat[1].x,mat[2].x };
			break;
		case 1:
			return { mat[0].y,mat[1].y,mat[2].y };
			break;
		case 2:
			return { mat[0].z,mat[1].z,mat[2].z };
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix3x3& mat)
	{
		os << mat.mat[0] << std::endl << mat.mat[1] << std::endl << mat.mat[2];
		return os;
	}
};
struct Matrix3
{
	Vector4 mat[3];

	Matrix3(float f)
	{
		mat[0] = { f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, f, 0.0f };
	}

	Matrix3()
	{
		mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
	}


	inline Vector3 row(int i)
	{
		switch (i)
		{
		case 0:
			return { mat[0].x,mat[1].x,mat[2].x };
			break;
		case 1:
			return { mat[0].y,mat[1].y,mat[2].y };
			break;
		case 2:
			return { mat[0].z,mat[1].z,mat[2].z };
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix3& mat)
	{
		os << mat.mat[0] << std::endl << mat.mat[1] << std::endl << mat.mat[2];
		return os;
	}
};

struct Matrix4
{
	Vector4 mat[4];

	Matrix4(float f)
	{
		mat[0] = { f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, f, 0.0f };
		mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Matrix4()
	{
		mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		mat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		mat[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
		mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	Matrix3 ToMatrix3()
	{
		Matrix3 result;
		result.mat[0] = mat[0];
		result.mat[1] = mat[1];
		result.mat[2] = mat[2];
		return result;
	}


	inline Vector4 row(int i)
	{
		switch (i)
		{
		case 0:
			return { mat[0].x,mat[1].x,mat[2].x,mat[3].x };
			break;
		case 1:
			return { mat[0].y,mat[1].y,mat[2].y,mat[3].y };
			break;
		case 2:
			return { mat[0].z,mat[1].z,mat[2].z,mat[3].z };
			break;
		case 3:
			return { mat[0].w,mat[1].w,mat[2].w,mat[3].w };
			break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix4& mat)
	{
		os << mat.mat[0] << std::endl << mat.mat[1] << std::endl << mat.mat[2] << std::endl << mat.mat[3];
		return os;
	}
};

struct Quaternion
{
	float w, x, y, z;

	Quaternion()
	{

	}

	Quaternion(float w, float x, float y, float z)
	{
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Quaternion conjugate()
	{
		Quaternion q{ w, x, y, z };
		x = -x;
		y = -y;
		z = -z;
		return q;
	}

	Matrix4 ToMatrix4()
	{
		Matrix4 ret(1.0f);
		ret.mat[0][0] = 2 * (w * w + x * x) - 1;
		ret.mat[0][1] = 2 * (x * y - w * z);
		ret.mat[0][2] = 2 * (x * z + w * y);
		ret.mat[1][0] = 2 * (x * y + w * z);
		ret.mat[1][1] = 2 * (w * w + y * y) - 1;
		ret.mat[1][2] = 2 * (y * z - w * x);
		ret.mat[2][0] = 2 * (x * z - w * y);
		ret.mat[2][1] = 2 * (y * z + w * x);
		ret.mat[2][2] = 2 * (w * w + z * z) - 1;
		return ret;
	}

	Matrix3x3 ToMatrix3x3()
	{
		Matrix3x3 ret;
		ret.mat[0].x = 2 * (w * w + x * x) - 1;
		ret.mat[0].y = 2 * (x * y - w * z);
		ret.mat[0].z = 2 * (x * z + w * y);
		ret.mat[1].x = 2 * (x * y + w * z);
		ret.mat[1].y = 2 * (w * w + y * y) - 1;
		ret.mat[1].z = 2 * (y * z - w * x);
		ret.mat[2].x = 2 * (x * z - w * y);
		ret.mat[2].y = 2 * (y * z + w * x);
		ret.mat[2].z = 2 * (w * w + z * z) - 1;
		return ret;
	}

	static Quaternion FromEulerAngles(Vector3 euler)
	{
		Quaternion q{ 1,0,0,0 };
		float yaw = euler.z;
		float pitch = euler.y;
		float roll = euler.x;

		double cy = cos(yaw * 0.5);
		double sy = sin(yaw * 0.5);
		double cp = cos(pitch * 0.5);
		double sp = sin(pitch * 0.5);
		double cr = cos(roll * 0.5);
		double sr = sin(roll * 0.5);

		q.w = cr * cp * cy + sr * sp * sy;
		q.x = sr * cp * cy - cr * sp * sy;
		q.y = cr * sp * cy + sr * cp * sy;
		q.z = cr * cp * sy - sr * sp * cy;
		return q;
	}

	void normalize()
	{
		float mag = sqrtf(w * w + x * x + y * y + z * z);
		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}



};

inline Quaternion fromMatrix3(Matrix3x3 mat)
{
	return Quaternion();
}

inline Quaternion operator*(Quaternion a, Quaternion b)
{
	Quaternion ret;
	ret.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	ret.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	ret.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	ret.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	return ret;
}

inline Quaternion operator+(Quaternion a, Quaternion b)
{
	Quaternion ret;
	ret.w = a.w + b.w;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;
	return ret;
}

inline Quaternion operator*(Quaternion a, float f)
{
	Quaternion ret{ 0,0,0,0 };
	ret.w = a.w * f;
	ret.x = a.x * f;
	ret.y = a.y * f;
	ret.z = a.z * f;
	return ret;
}

inline Quaternion operator*(float f, Quaternion a)
{
	Quaternion ret{ 0,0,0,0 };
	ret.w = a.w * f;
	ret.x = a.x * f;
	ret.y = a.y * f;
	ret.z = a.z * f;
	return ret;
}


inline Vector2 operator+(Vector2& a, Vector2& b)
{
	return { a.x + b.x, a.y + b.y };
}

//inline Vector3 operator+(Vector3& a, Vector3& b)
//{
//	return { a.x + b.x, a.y + b.y,  a.z + b.z };
//}

inline Vector3 operator+(Vector3 a, Vector3 b)
{
	return { a.x + b.x, a.y + b.y,  a.z + b.z };
}

inline Vector4 operator+(Vector4& a, Vector4& b)
{
	return { a.x + b.x, a.y + b.y,  a.z + b.z, a.w + b.w };
}

inline Vector2 operator-(Vector2& a, Vector2& b)
{
	return { a.x - b.x, a.y - b.y };
}

//inline Vector3 operator-(Vector3& a, Vector3& b)
//{
//	return { a.x - b.x, a.y - b.y,  a.z - b.z };
//}

inline Vector3 operator-(Vector3 a, Vector3 b)
{
	return { a.x - b.x, a.y - b.y,  a.z - b.z };
}

inline Vector4 operator-(Vector4& a, Vector4& b)
{
	return { a.x - b.x, a.y - b.y,  a.z - b.z, a.w - b.w };
}

//
inline Vector2 operator+=(Vector2& a, Vector2 b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline Vector3 operator+=(Vector3& a, Vector3 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline Vector4 operator+=(Vector4& a, Vector4 b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

inline Vector2 operator-=(Vector2& a, Vector2 b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline Vector3 operator-=(Vector3& a, Vector3 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline Vector4 operator-=(Vector4& a, Vector4 b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}
//

inline Vector3 operator-(Vector3 vec)
{
	return { -vec.x, -vec.y, -vec.z };
}

inline Vector3 operator*(float a, Vector3 b)
{
	return Vector3(a * b.x, a * b.y, a * b.z);
}

inline float VectorDot(Vector2 a, Vector2 b)
{
	return  a.x * b.x + a.y * b.y;
}

inline float VectorDot(Vector3 a, Vector3 b)
{
	return  a.x * b.x + a.y * b.y + a.z * b.z;
}


inline float VectorDot(Vector4 a, Vector4 b)
{
	return  a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float VectorLength(Vector2 vec)
{
	return sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline float VectorLength(Vector3 vec)
{
	return  sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline float VectorLength(Vector4 vec)
{
	return  sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

inline Vector3 VectorCross(Vector2 a, Vector2 b)
{
	return { 0, 0, a.x * b.y - a.y * b.x };
}

inline Vector3 VectorCross(Vector3 a, Vector3 b)
{
	return { a.y * b.z - a.z * b.y, a.x * b.z - a.z * b.x, a.x * b.y - a.y * b.x };
}

inline Vector3 operator*(Vector3 vec, float f)
{
	return { vec.x * f, vec.y * f, vec.z * f };
}

inline Vector3 VectorNormalize(Vector3 vec)
{
	float len = VectorLength(vec);
	return { vec.x / len, vec.y / len, vec.z / len };
}

inline Vector4 VectorNormalize(Vector4 vec)
{
	float len = VectorLength(vec);
	return { vec.x / len, vec.y / len, vec.z / len, vec.w/len };
}

inline Matrix4 MatrixTranslate(Vector3 translation)
{
	Matrix4 mat;
	mat.mat[0] = { 1.0f, 0.0f, 0.0f, translation.x };
	mat.mat[1] = { 0.0f, 1.0f, 0.0f, translation.y };
	mat.mat[2] = { 0.0f, 0.0f, 1.0f, translation.z };
	mat.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return mat;
}

inline Matrix4 MatrixScale(Vector3 scale)
{
	Matrix4 mat;
	mat.mat[0] = { scale.x, 0.0f, 0.0f, 0.0f };
	mat.mat[1] = { 0.0f, scale.y, 0.0f, 0.0f };
	mat.mat[2] = { 0.0f, 0.0f, scale.z, 0.0f };
	mat.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return mat;
}

inline Matrix4 MatrixXRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
	rot.mat[1] = { 0.0f, cosTheta, -sinTheta, 0.0f };
	rot.mat[2] = { 0.0f, sinTheta, cosTheta, 0.0f };
	rot.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return rot;
}

inline Matrix4 MatrixYRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { cosTheta, 0, sinTheta, 0 };
	rot.mat[1] = { 0, 1, 0, 0 };
	rot.mat[2] = { -sinTheta, 0, cosTheta, 0 };
	rot.mat[3] = { 0, 0, 0, 1 };
	return rot;
}

inline Matrix4 MatrixZRotation(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	Matrix4 rot;
	rot.mat[0] = { cosTheta, -sinTheta, 0, 0 };
	rot.mat[1] = { sinTheta, cosTheta, 0, 0 };
	rot.mat[2] = { 0, 0, 1, 0 };
	rot.mat[3] = { 0, 0, 0, 1 };
	return rot;
}

inline Matrix4 MatrixEuler(Vector3 euler)
{
	float sinAlpha = sinf(euler.z);
	float cosAlpha = cosf(euler.z);
	float sinBeta = sinf(euler.y);
	float cosBeta = cosf(euler.y);
	float sinGamma = sinf(euler.x);
	float cosGamma = cosf(euler.x);
	Matrix4 rot;
	rot.mat[0] = { cosAlpha * cosBeta, cosAlpha * sinBeta * sinGamma - sinAlpha * cosGamma, cosAlpha * sinBeta * cosGamma + sinAlpha * sinGamma, 0.0f };
	rot.mat[1] = { sinAlpha * cosBeta, sinAlpha * sinBeta * sinGamma + cosAlpha * cosGamma, sinAlpha * sinBeta * cosGamma - cosAlpha * sinGamma, 0.0f };
	rot.mat[2] = { -sinBeta, cosBeta * sinGamma, cosBeta * cosGamma, 0.0f };
	rot.mat[3] = { 0.0f, 0.0f, 0.0f, 1.0f };
	return rot;
}

inline Matrix4 operator*(Matrix4 a, Matrix4 b)
{
	Matrix4 mat;
	mat.mat[0] = { VectorDot(a.row(0),b.mat[0]), VectorDot(a.row(1),b.mat[0]) , VectorDot(a.row(2),b.mat[0]) , VectorDot(a.row(3),b.mat[0]) };
	mat.mat[1] = { VectorDot(a.row(0),b.mat[1]), VectorDot(a.row(1),b.mat[1]) , VectorDot(a.row(2),b.mat[1]) , VectorDot(a.row(3),b.mat[1]) };
	mat.mat[2] = { VectorDot(a.row(0),b.mat[2]), VectorDot(a.row(1),b.mat[2]) , VectorDot(a.row(2),b.mat[2]) , VectorDot(a.row(3),b.mat[2]) };
	mat.mat[3] = { VectorDot(a.row(0),b.mat[3]), VectorDot(a.row(1),b.mat[3]) , VectorDot(a.row(2),b.mat[3]) , VectorDot(a.row(3),b.mat[3]) };
	return mat;
}

inline Matrix4 MatrixInverse(Matrix4 matrix)
{
	Matrix4 result;
	float s0 = matrix.mat[0].x * matrix.mat[1].y - matrix.mat[1].x * matrix.mat[0].y;
	float s1 = matrix.mat[0].x * matrix.mat[1].z - matrix.mat[1].x * matrix.mat[0].z;
	float s2 = matrix.mat[0].x * matrix.mat[1].w - matrix.mat[1].x * matrix.mat[0].w;
	float s3 = matrix.mat[0].y * matrix.mat[1].z - matrix.mat[1].y * matrix.mat[0].z;
	float s4 = matrix.mat[0].y * matrix.mat[1].w - matrix.mat[1].y * matrix.mat[0].w;
	float s5 = matrix.mat[0].z * matrix.mat[1].w - matrix.mat[1].z * matrix.mat[0].w;

	float c5 = matrix.mat[2].z * matrix.mat[3].w - matrix.mat[3].z * matrix.mat[2].w;
	float c4 = matrix.mat[2].y * matrix.mat[3].w - matrix.mat[3].y * matrix.mat[2].w;
	float c3 = matrix.mat[2].y * matrix.mat[3].z - matrix.mat[3].y * matrix.mat[2].z;
	float c2 = matrix.mat[2].x * matrix.mat[3].w - matrix.mat[3].x * matrix.mat[2].w;
	float c1 = matrix.mat[2].x * matrix.mat[3].z - matrix.mat[3].x * matrix.mat[2].z;
	float c0 = matrix.mat[2].x * matrix.mat[3].y - matrix.mat[3].x * matrix.mat[2].y;

	float invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	result.mat[0][0] = (matrix.mat[1][1] * c5 - matrix.mat[1][2] * c4 + matrix.mat[1][3] * c3) * invdet;
	result.mat[0][1] = (-matrix.mat[0][1] * c5 + matrix.mat[0][2] * c4 - matrix.mat[0][3] * c3) * invdet;
	result.mat[0][2] = (matrix.mat[3][1] * s5 - matrix.mat[3][2] * s4 + matrix.mat[3][3] * s3) * invdet;
	result.mat[0][3] = (-matrix.mat[2][1] * s5 + matrix.mat[2][2] * s4 - matrix.mat[2][3] * s3) * invdet;

	result.mat[1][0] = (-matrix.mat[1][0] * c5 + matrix.mat[1][2] * c2 - matrix.mat[1][3] * c1) * invdet;
	result.mat[1][1] = (matrix.mat[0][0] * c5 - matrix.mat[0][2] * c2 + matrix.mat[0][3] * c1) * invdet;
	result.mat[1][2] = (-matrix.mat[3][0] * s5 + matrix.mat[3][2] * s2 - matrix.mat[3][3] * s1) * invdet;
	result.mat[1][3] = (matrix.mat[2][0] * s5 - matrix.mat[2][2] * s2 + matrix.mat[2][3] * s1) * invdet;

	result.mat[2][0] = (matrix.mat[1][0] * c4 - matrix.mat[1][1] * c2 + matrix.mat[1][3] * c0) * invdet;
	result.mat[2][1] = (-matrix.mat[0][0] * c4 + matrix.mat[0][1] * c2 - matrix.mat[0][3] * c0) * invdet;
	result.mat[2][2] = (matrix.mat[3][0] * s4 - matrix.mat[3][1] * s2 + matrix.mat[3][3] * s0) * invdet;
	result.mat[2][3] = (-matrix.mat[2][0] * s4 + matrix.mat[2][1] * s2 - matrix.mat[2][3] * s0) * invdet;

	result.mat[3][0] = (-matrix.mat[1][0] * c3 + matrix.mat[1][1] * c1 - matrix.mat[1][2] * c0) * invdet;
	result.mat[3][1] = (matrix.mat[0][0] * c3 - matrix.mat[0][1] * c1 + matrix.mat[0][2] * c0) * invdet;
	result.mat[3][2] = (-matrix.mat[3][0] * s3 + matrix.mat[3][1] * s1 - matrix.mat[3][2] * s0) * invdet;
	result.mat[3][3] = (matrix.mat[2][0] * s3 - matrix.mat[2][1] * s1 + matrix.mat[2][2] * s0) * invdet;
	return result;
}

inline Matrix4 MatrixTranspose(Matrix4 matrix)
{
	Matrix4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.mat[i][j] = matrix.mat[j][i];
		}
	}
	return result;
}

inline Matrix4 MatrixPerspective(float aspectRatio, float fov, float zNear, float zFar)
{
	float yScale = tanf(0.5f * ((float)M_PI - fov));
	float xScale = yScale / aspectRatio;
	float zRangeInverse = 1.f / (zNear - zFar);
	float zScale = zFar * zRangeInverse;
	float zTranslation = zFar * zNear * zRangeInverse;

	Matrix4 result;
	result.mat[0] = { xScale, 0, 0, 0, };
	result.mat[1] = { 0, yScale, 0, 0, };
	result.mat[2] = { 0, 0, zScale, zTranslation, };
	result.mat[3] = { 0, 0, -1, 0 };
	return result;
}

inline float degreesToRadians(float degrees)
{
	return degrees * ((float)M_PI / 180.0f);
}


#endif