#pragma once

#include <iostream>
#include <vector>

#include "GL/glut.h"
#include "Object.h"
#include "Pigment.h"
#include "SurfaceFinish.h"
#include "vecFunctions.h"

class Polyhedron : public Object
{
public:
	Polyhedron(Pigment *p, SurfaceFinish *sf, const size_t f);

	// Getters
	size_t getFaces() const { return faces; }
	std::vector<Vec4> getPlanes() const { return planes; }

	// Setters
	void setFaces(const int f) { faces = f; }
	void addPlane(const Vec4 &plane);

	friend std::ostream &operator<<(std::ostream &out, const Polyhedron &poly);

	void draw() const override;

private:
	Vec3 intersectThreePlanes(const Vec4& p1, const Vec4& p2, const Vec4& p3) const;
	
	size_t faces;			  // Number of faces
	std::vector<Vec4> planes; // Plane equations for each face
};
