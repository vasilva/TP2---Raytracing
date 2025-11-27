#pragma once

#include "SurfaceFinish.h"
#include "Pigment.h"

class Object
{
public:
	enum Type
	{
		Sphere,
		Polyhedron
	}; // Object types

	// Constructor
	Object(Type t, Pigment *p = nullptr, SurfaceFinish *sf = nullptr)
		: type(t), pigment(p), finish(sf) {}

	// Getters
	Type getType() const { return type; }
	Pigment *getPigment() const { return pigment; }
	SurfaceFinish *getFinish() const { return finish; }

	// Setters
	void setPigment(Pigment *p) { pigment = p; }
	void setFinish(SurfaceFinish *sf) { finish = sf; }

	// Apply material properties and pigment color in OpenGL
	void applyMaterials() const;
	void applyPigmentColor(const Vec4 &point) const;

	virtual void draw() const = 0;

private:
	Type type;
	Pigment *pigment;
	SurfaceFinish *finish;
};
