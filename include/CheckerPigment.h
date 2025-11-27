#pragma once

#include <iostream>

#include "GL/glut.h"
#include "vecFunctions.h"
#include "Pigment.h"

class CheckerPigment : public Pigment
{
public:
	CheckerPigment();
	CheckerPigment(const Vec3 &col1, const Vec3 &col2, const GLfloat s);

	// Setters
	void setColor1(const Vec3 &col1) { color1 = col1; }
	void setColor2(const Vec3 &col2) { color2 = col2; }
	void setSize(const GLfloat s) { size = s; }

	// Getters
	Vec3 getColor1() const { return color1; }
	Vec3 getColor2() const { return color2; }
	GLfloat getSize() const { return size; }

	// Override getColor method
	Vec3 getColor(const Vec4 &point) const override;
	Vec3 getColorOnSphere(const Vec4 &point, const Vec3 &center) const;

	// Print
	friend std::ostream &operator<<(std::ostream &out, const CheckerPigment &cp);

private:
	Vec3 color1;  // First color
	Vec3 color2;  // Second color
	GLfloat size; // Size of each checker square
};
