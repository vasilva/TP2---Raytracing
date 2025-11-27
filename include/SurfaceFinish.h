#pragma once

#include <iostream>

#include "GL/glut.h"

class SurfaceFinish
{
public:
	SurfaceFinish(const GLfloat ka, const GLfloat kd, const GLfloat ks,
				  const GLfloat a, const GLfloat kr, const GLfloat kt, const GLfloat ior)
		: kAmbient(ka), kDiffuse(kd), kSpecular(ks), alpha(a),
		  kReflection(kr), kTransmission(kt), indexOfRefraction(ior) {}

	// Setters
	void setAmbient(const GLfloat ka) { kAmbient = ka; }
	void setDiffuse(const GLfloat kd) { kDiffuse = kd; }
	void setSpecular(const GLfloat ks) { kSpecular = ks; }
	void setAlpha(const GLfloat a) { alpha = a; }
	void setReflection(const GLfloat kr) { kReflection = kr; }
	void setTransmission(const GLfloat kt) { kTransmission = kt; }
	void setIOR(const GLfloat ior) { indexOfRefraction = ior; }

	// Getters
	GLfloat getAmbient() const { return kAmbient; }
	GLfloat getDiffuse() const { return kDiffuse; }
	GLfloat getSpecular() const { return kSpecular; }
	GLfloat getAlpha() const { return alpha; }
	GLfloat getReflection() const { return kReflection; }
	GLfloat getTransmission() const { return kTransmission; }
	GLfloat getIOR() const { return indexOfRefraction; }

	friend std::ostream &operator<<(std::ostream &out, const SurfaceFinish &sf);

private:
	GLfloat kAmbient;		   // Ambient coefficient
	GLfloat kDiffuse;		   // Diffuse coefficient
	GLfloat kSpecular;		   // Specular coefficient
	GLfloat alpha;			   // Shininess exponent
	GLfloat kReflection;	   // Reflection coefficient
	GLfloat kTransmission;	   // Transmission coefficient
	GLfloat indexOfRefraction; // Index of refraction
};
