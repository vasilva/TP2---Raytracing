#pragma once

#include <iostream>

#include "GL/glut.h"
#include "Object.h"
#include "vecFunctions.h"
#include "Pigment.h"
#include "SurfaceFinish.h"

class Sphere : public Object
{
public:
	Sphere(Pigment *p, SurfaceFinish *sf,
		   const Vec3 &center, const GLfloat radius);

	// Getters
	Vec3 getCenter() const { return center; }
	GLfloat getRadius() const { return radius; }

	// Setters
	void setCenter(const Vec3 &c) { center = c; }
	void setRadius(const GLfloat r) { radius = r; }
	
	// Print
	friend std::ostream &operator<<(std::ostream &out, const Sphere &s);
	
	void draw() const override;

private:
	Vec3 center;
	GLfloat radius;
};
