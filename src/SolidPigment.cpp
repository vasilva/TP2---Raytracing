#include <iostream>

#include "../include/SolidPigment.h"

std::ostream &operator<<(std::ostream &out, const SolidPigment &sp)
{
	Vec3 c = sp.getColor(Vec4());
	out << "SolidPigment: "
		<< "(" << c.x << ", " << c.y << ", " << c.z << ")";
	return out;
}

SolidPigment::SolidPigment(const Vec3 &color)
	: Pigment(Pigment::SOLID), rgbColor(color) {}

Vec3 SolidPigment::getColor(const Vec4 &) const { return rgbColor; }
