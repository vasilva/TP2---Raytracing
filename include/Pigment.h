#pragma once

#include "GL/glut.h"
#include "vecFunctions.h"

class Pigment
{
public:
	enum Type
	{
		SOLID,
		CHECKER,
		TEXMAP
	}; // Pigment types
	Type type;

	Pigment(Type t) : type(t) {}

	// Retorna cor RGB (0..1) no ponto em coordenadas globais
	virtual Vec3 getColor(const Vec4 &point) const = 0;
};
