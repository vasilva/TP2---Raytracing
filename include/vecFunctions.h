#pragma once

#include <iostream>
#include <cmath>

#include "GL/glut.h"

#ifndef PI
#define PI static_cast<GLfloat>(3.14159265358979323846)
#endif

// 4D Vector structure
struct Vec4
{
	GLfloat x, y, z, w;
	Vec4() : x(0), y(0), z(0), w(0) {}
	Vec4(GLfloat x, GLfloat y, GLfloat z, GLfloat w) : x(x), y(y), z(z), w(w) {}
};

// 3D Vector structure
struct Vec3
{
	GLfloat x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};

// Common vectors
static const Vec3 ZERO_3D(0.0f, 0.0f, 0.0f);
static const Vec4 ZERO_4D(0.0f, 0.0f, 0.0f, 0.0f);
static const Vec3 ONE_3D(1.0f, 1.0f, 1.0f);
static const Vec4 ONE_4D(1.0f, 1.0f, 1.0f, 1.0f);

// Unit vectors
static const Vec3 UNIT_X(1.0f, 0.0f, 0.0f);
static const Vec3 UNIT_Y(0.0f, 1.0f, 0.0f);
static const Vec3 UNIT_Z(0.0f, 0.0f, 1.0f);

// 4D Unit vectors
static const Vec4 UNIT_X4D(1.0f, 0.0f, 0.0f, 0.0f);
static const Vec4 UNIT_Y4D(0.0f, 1.0f, 0.0f, 0.0f);
static const Vec4 UNIT_Z4D(0.0f, 0.0f, 1.0f, 0.0f);
static const Vec4 UNIT_W4D(0.0f, 0.0f, 0.0f, 1.0f);

/* Vector operations */

// Cross product (only for Vec3)
inline Vec3 cross(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

// Dot product
inline GLfloat dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
inline GLfloat dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Length of vector squared
inline GLfloat lengthSq(const Vec4& v)
{
	return dot(v, v);
}
inline GLfloat lengthSq(const Vec3& v)
{
	return dot(v, v);
}

// Length of vector
inline GLfloat length(const Vec4& v)
{
	return std::sqrt(lengthSq(v));
}
inline GLfloat length(const Vec3& v)
{
	return std::sqrt(lengthSq(v));
}

/* Overload operations */

inline Vec4 operator+(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline Vec3	operator+(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec4 operator-(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
inline Vec3	operator-(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec4 operator*(const Vec4& v, GLfloat scalar)
{
	return Vec4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}
inline Vec3	operator*(const Vec3& v, GLfloat scalar)
{
	return Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
}

inline Vec4 operator*(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
inline Vec3	operator*(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline Vec4 operator/(const Vec4& v, GLfloat scalar)
{
	return Vec4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
}
inline Vec3	operator/(const Vec3& v, GLfloat scalar)
{
	return Vec3(v.x / scalar, v.y / scalar, v.z / scalar);
}

inline Vec4 operator/(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline Vec3	operator/(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

inline Vec4& operator+=(Vec4& a, const Vec4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}
inline Vec3& operator+=(Vec3& a, const Vec3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline Vec4& operator-=(Vec4& a, const Vec4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}
inline Vec3& operator-=(Vec3& a, const Vec3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline Vec4& operator*=(Vec4& v, GLfloat scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	v.w *= scalar;
	return v;
}
inline Vec3& operator*=(Vec3& v, GLfloat scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	v.z *= scalar;
	return v;
}

inline Vec4& operator/=(Vec4& v, GLfloat scalar)
{
	v.x /= scalar;
	v.y /= scalar;
	v.z /= scalar;
	v.w /= scalar;
	return v;
}
inline Vec3& operator/=(Vec3& v, GLfloat scalar)
{
	v.x /= scalar;
	v.y /= scalar;
	v.z /= scalar;
	return v;
}

// Normalize
inline Vec4 normalize(const Vec4& v)
{
	GLfloat len = length(v);
	if (len == 0) return ZERO_4D; // Avoid division by zero
	return v / len;
}
inline Vec3 normalize(const Vec3& v)
{
	GLfloat len = length(v);
	if (len == 0) return ZERO_3D; // Avoid division by zero
	return v / len;
}

// Limit vector length
inline void limit(Vec4& v, GLfloat maxLength)
{
	GLfloat lenSq = lengthSq(v);
	GLfloat maxLengthSq = maxLength * maxLength;
	if (lenSq > maxLengthSq)
	{
		GLfloat scale = maxLength / std::sqrt(lenSq);
		v *= scale;
	}
}
inline void limit(Vec3& v, GLfloat maxLength)
{
	GLfloat lenSq = lengthSq(v);
	GLfloat maxLengthSq = maxLength * maxLength;
	if (lenSq > maxLengthSq)
	{
		GLfloat scale = maxLength / std::sqrt(lenSq);
		v *= scale;
	}
}

// Linear interpolation
inline Vec4 lerp(const Vec4& a, const Vec4& b, GLfloat alpha)
{
	return a + (b - a) * alpha;
}
inline Vec3 lerp(const Vec3& a, const Vec3& b, GLfloat alpha)
{
	return a + (b - a) * alpha;
}

// Output stream overloads
inline std::ostream& operator<<(std::ostream& out, const Vec4& v)
{
	// Format: (x, y, z, w)
	out << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return out;
}
inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	// Format: (x, y, z)
	out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return out;
}
// End of vecFunctions.h