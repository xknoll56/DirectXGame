#pragma once

#define _USE_MATH_DEFINES 
#include <math.h>
#pragma warning(push)
#pragma warning(disable:4201) // anonymous struct warning

struct Vector3
{
    float x, y, z;
};

union Vector4
{
    struct {
        float x, y, z, w;
    };
    struct {
        Vector3 xyz;
    };
};
#pragma warning(pop)

struct Vector3x3
{
    // We internally do 3x4 for HLSL-friendly alignment
    float m[3][4];
};

union Matrix4
{
    float m[4][4];
    Vector4 cols[4];

    inline Vector4 row(int i) { // Returns i-th row of matrix
        return { m[0][i], m[1][i], m[2][i], m[3][i] };
    }
};

inline float degreesToRadians(float degs) {
    return degs * ((float)M_PI / 180.0f);
}

inline float length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float length(Vector4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline float dot(Vector4 a, Vector4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline Vector3 operator* (Vector3 v, float f) {
    return { v.x * f, v.y * f, v.z * f };
}

inline Vector4 operator* (Vector4 v, float f) {
    return { v.x * f, v.y * f, v.z * f, v.w * f };
}

inline Vector3 normalise(Vector3 v) {
    return v * (1.f / length(v));
}

inline Vector4 normalise(Vector4 v) {
    return v * (1.f / length(v));
}

inline Vector3 cross(Vector3 a, Vector3 b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline Vector3 operator+= (Vector3& lhs, Vector3 rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

inline Vector3 operator-= (Vector3& lhs, Vector3 rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

inline Vector3 operator- (Vector3 v) {
    return { -v.x, -v.y, -v.z };
}

inline Matrix4 scaleMat(float s) {
    return {
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1
    };
}

inline Matrix4 rotateXMat(float rad) {
    float sinTheta = sinf(rad);
    float cosTheta = cosf(rad);
    return {
        1, 0, 0, 0,
        0, cosTheta, -sinTheta, 0,
        0, sinTheta, cosTheta, 0,
        0, 0, 0, 1
    };
}

inline Matrix4 rotateYMat(float rad) {
    float sinTheta = sinf(rad);
    float cosTheta = cosf(rad);
    return {
        cosTheta, 0, sinTheta, 0,
        0, 1, 0, 0,
        -sinTheta, 0, cosTheta, 0,
        0, 0, 0, 1
    };
}

inline Matrix4 translationMat(Vector3 trans) {
    return {
        1, 0, 0, trans.x,
        0, 1, 0, trans.y,
        0, 0, 1, trans.z,
        0, 0, 0, 1
    };
}

inline Matrix4 makePerspectiveMat(float aspectRatio, float fovYRadians, float zNear, float zFar)
{
    // float yScale = 1 / tanf(0.5f * fovYRadians); 
    // NOTE: 1/tan(X) = tan(90degs - X), so we can avoid a divide
    // float yScale = tanf((0.5f * M_PI) - (0.5f * fovYRadians));
    float yScale = tanf(0.5f * ((float)M_PI - fovYRadians));
    float xScale = yScale / aspectRatio;
    float zRangeInverse = 1.f / (zNear - zFar);
    float zScale = zFar * zRangeInverse;
    float zTranslation = zFar * zNear * zRangeInverse;

    Matrix4 result = {
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, zScale, zTranslation,
        0, 0, -1, 0
    };
    return result;
}

inline Matrix4 operator* (Matrix4 a, Matrix4 b) {
    return {
        dot(a.row(0), b.cols[0]),
        dot(a.row(1), b.cols[0]),
        dot(a.row(2), b.cols[0]),
        dot(a.row(3), b.cols[0]),
        dot(a.row(0), b.cols[1]),
        dot(a.row(1), b.cols[1]),
        dot(a.row(2), b.cols[1]),
        dot(a.row(3), b.cols[1]),
        dot(a.row(0), b.cols[2]),
        dot(a.row(1), b.cols[2]),
        dot(a.row(2), b.cols[2]),
        dot(a.row(3), b.cols[2]),
        dot(a.row(0), b.cols[3]),
        dot(a.row(1), b.cols[3]),
        dot(a.row(2), b.cols[3]),
        dot(a.row(3), b.cols[3])
    };
}

inline Vector4 operator* (Vector4 v, Matrix4 m) {
    return {
        dot(v, m.cols[0]),
        dot(v, m.cols[1]),
        dot(v, m.cols[2]),
        dot(v, m.cols[3])
    };
}

inline Matrix4 transpose(Matrix4 m) {
    return Matrix4{
        m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
        m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
        m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
        m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
    };
}

inline Matrix4 inverse(Matrix4 mat)
{
    Matrix4 matrix;
    float s0 = mat.m[0][0] * mat.m[1][1] - mat.m[1][0] * mat.m[0][1];
    float s1 = mat.m[0][0] * mat.m[1][2] - mat.m[1][0] * mat.m[0][2];
    float s2 = mat.m[0][0] * mat.m[1][3] - mat.m[1][0] * mat.m[0][3];
    float s3 = mat.m[0][0] * mat.m[1][2] - mat.m[1][1] * mat.m[0][2];
    float s4 = mat.m[0][0] * mat.m[1][3] - mat.m[1][1] * mat.m[0][3];
    float s5 = mat.m[0][0] * mat.m[1][3] - mat.m[1][2] * mat.m[0][3];
                             
    float c5 = mat.m[0][0] * mat.m[3][3] - mat.m[3][2] * mat.m[2][3];
    float c4 = mat.m[0][0] * mat.m[3][3] - mat.m[3][1] * mat.m[2][3];
    float c3 = mat.m[0][0] * mat.m[3][2] - mat.m[3][1] * mat.m[2][2];
    float c2 = mat.m[0][0] * mat.m[3][3] - mat.m[3][0] * mat.m[2][3];
    float c1 = mat.m[0][0] * mat.m[3][2] - mat.m[3][0] * mat.m[2][2];
    float c0 = mat.m[0][0] * mat.m[3][1] - mat.m[3][0] * mat.m[2][1];

    float invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    matrix.m[0][0] = (mat.m[1][1] * c5 - mat.m[1][2] * c4 + mat.m[1][3] * c3) * invdet;
    matrix.m[0][1] =(-mat.m[0][1] * c5 + mat.m[0][2] * c4 - mat.m[0][3] * c3) * invdet;
    matrix.m[0][2] = (mat.m[3][1] * s5 - mat.m[3][2] * s4 + mat.m[3][3] * s3) * invdet;
    matrix.m[0][3] =(-mat.m[2][1] * s5 + mat.m[2][2] * s4 - mat.m[2][3] * s3) * invdet;
     
    matrix.m[1][0] =(-mat.m[1][0] * c5 + mat.m[1][2] * c2 - mat.m[1][3] * c1) * invdet;
    matrix.m[1][1] = (mat.m[0][0] * c5 - mat.m[0][2] * c2 + mat.m[0][3] * c1) * invdet;
    matrix.m[1][2] =(-mat.m[3][0] * s5 + mat.m[3][2] * s2 - mat.m[3][3] * s1) * invdet;
    matrix.m[1][3] = (mat.m[2][0] * s5 - mat.m[2][2] * s2 + mat.m[2][3] * s1) * invdet;
       
    matrix.m[2][0] = (mat.m[1][0] * c4 - mat.m[1][1] * c2 + mat.m[1][3] * c0) * invdet;
    matrix.m[2][1] =(-mat.m[0][0] * c4 + mat.m[0][1] * c2 - mat.m[0][3] * c0) * invdet;
    matrix.m[2][2] = (mat.m[3][0] * s4 - mat.m[3][1] * s2 + mat.m[3][3] * s0) * invdet;
    matrix.m[2][3] =(-mat.m[2][0] * s4 + mat.m[2][1] * s2 - mat.m[2][3] * s0) * invdet;
         
    matrix.m[3][0] =(-mat.m[1][0] * c3 + mat.m[1][1] * c1 - mat.m[1][2] * c0) * invdet;
    matrix.m[3][1] = (mat.m[0][0] * c3 - mat.m[0][1] * c1 + mat.m[0][2] * c0) * invdet;
    matrix.m[3][2] =(-mat.m[3][0] * s3 + mat.m[3][1] * s1 - mat.m[3][2] * s0) * invdet;
    matrix.m[3][3] = (mat.m[2][0] * s3 - mat.m[2][1] * s1 + mat.m[2][2] * s0) * invdet;
    return matrix;
}

inline Vector3x3 Matrix4ToVector3x3(Matrix4 m) {
    Vector3x3 result = {
        m.m[0][0], m.m[0][1], m.m[0][2], 0.0,
        m.m[1][0], m.m[1][1], m.m[1][2], 0.0,
        m.m[2][0], m.m[2][1], m.m[2][2], 0.0
    };
    return result;
}
