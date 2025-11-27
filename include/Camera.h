#pragma once

#include <iostream>

#include "GL/glut.h"
#include "vecFunctions.h"

class Camera
{
public:
	Camera();
	Camera(const Vec3 &position, const Vec3 &target,
		   const Vec3 &normal, GLfloat fovY);

	// Apply the camera view using gluLookAt
	void applyView() const;

	// Setters
	void setPosition(const Vec3 &position) { pos = position; }
	void setTarget(const Vec3 &tgt) { target = tgt; }
	void setNormal(const Vec3 &n) { normal = n; }
	void setFOV(const GLfloat fov) { fovY = fov; }

	// Getters
	Vec3 getPosition() const { return pos; }
	Vec3 getTarget() const { return target; }
	Vec3 getNormal() const { return normal; }
	GLfloat getFOV() const { return fovY; }

	// Print
	friend std::ostream &operator<<(std::ostream &out, const Camera &c);

private:
	Vec3 pos, target, normal;
	GLfloat fovY;
};
