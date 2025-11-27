#include <iostream>

#include "../include/CheckerPigment.h"

std::ostream &operator<<(std::ostream &out, const CheckerPigment &cp)
{
	Vec3 c1 = cp.getColor1();
	Vec3 c2 = cp.getColor2();
	out << "CheckerPigment: "
		<< "color1(" << c1.x << ", " << c1.y << ", " << c1.z << ") "
		<< "color2(" << c2.x << ", " << c2.y << ", " << c2.z << ") "
		<< "size(" << cp.getSize() << ")";
	return out;
}

CheckerPigment::CheckerPigment()
	: Pigment(Pigment::CHECKER), color1(ONE_3D), color2(ZERO_3D), size(1.0f) {}

CheckerPigment::CheckerPigment(const Vec3 &col1, const Vec3 &col2, const GLfloat s)
	: Pigment(Pigment::CHECKER), color1(col1), color2(col2), size(s) {}

Vec3 CheckerPigment::getColor(const Vec4 &point) const
{
	// Avoid division by zero
	if (size == 0.0f)
		return color1;

	// Determine which color to return based on the point's position
	int xIndex = static_cast<int>(std::floor(point.x / size));
	int zIndex = static_cast<int>(std::floor(point.z / size));

	if ((xIndex + zIndex) % 2 == 0)
		return color1;
	else
		return color2;
}

Vec3 CheckerPigment::getColorOnSphere(const Vec4 &point, const Vec3 &center) const
{
	if (size == 0.0f)
		return color1;

	// Convert point to local sphere coordinates
	Vec3 pLocal = Vec3(point.x - center.x, point.y - center.y, point.z - center.z);
	pLocal = normalize(pLocal);

	// Spherical coordinates
	GLfloat theta = std::acos(pLocal.y);		  // polar angle
	GLfloat phi = std::atan2(pLocal.z, pLocal.x); // azimuthal angle

	GLfloat u = (phi + PI) / (2.0f * PI); // [0,1]
	GLfloat v = theta / PI;				  // [0,1]

	GLfloat repeats = size;
	if (repeats < 1.0f)
		repeats = 1.0f / std::max(1e-6f, repeats);

	int uIndex = static_cast<int>(std::floor(u * repeats));
	int vIndex = static_cast<int>(std::floor(v * repeats));

	if ((uIndex + vIndex) % 2 == 0)
		return color1;
	else
		return color2;
}
