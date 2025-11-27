#pragma once

#include <iostream>

#include "GL/glut.h"
#include "vecFunctions.h"
#include "Pigment.h"

class SolidPigment : public Pigment
{
public:
	SolidPigment(const Vec3 &color);

	// Setter
	void setColor(const Vec3 &color) { rgbColor = color; }

	// Returns the solid color regardless of the point
	Vec3 getColor(const Vec4 &) const override;

	// Print
	friend std::ostream &operator<<(std::ostream &out, const SolidPigment &sp);

private:
	Vec3 rgbColor; // Solid color
};