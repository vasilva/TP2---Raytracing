#pragma once

#include "GL/glut.h"
#include "vecFunctions.h"

class Light
{
public:
	Light(const Vec3 &position, const Vec3 &color,
		  const GLfloat rho0, const GLfloat rho1, const GLfloat rho2,
		  const GLenum lightID);

	// Setters
	void setPosition(const Vec3 &position) { pos = position; }
	void setColor(const Vec3 &color) { rgbColor = color; }
	void setAttenuationCoefficients(const GLfloat r0, const GLfloat r1, const GLfloat r2)
	{
		rho_0 = r0, rho_1 = r1, rho_2 = r2;
	}
	void setGLLightID(const GLenum lightID) { glLightID = lightID; }

	// Getters
	Vec3 getPosition() const { return pos; }
	Vec3 getColor() const { return rgbColor; }
	GLfloat getRho0() const { return rho_0; }
	GLfloat getRho1() const { return rho_1; }
	GLfloat getRho2() const { return rho_2; }
	GLenum getGLLightID() const { return glLightID; }

	// Apply the light parameters in OpenGL
	void applyLight() const;

	friend std::ostream &operator<<(std::ostream &out, const Light &light);

private:
	Vec3 pos;		  // Position of the light
	Vec3 rgbColor;	  // Color of the light
	GLfloat rho_0;	  // Constant attenuation coefficient
	GLfloat rho_1;	  // Linear attenuation coefficient
	GLfloat rho_2;	  // Quadratic attenuation coefficient
	GLenum glLightID; // OpenGL light ID
};
