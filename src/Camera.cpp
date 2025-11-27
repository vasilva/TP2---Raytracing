#include "../include/Camera.h"

Camera::Camera()
	: pos(ZERO_3D), target(UNIT_Z), normal(UNIT_Y), fovY(45.0f) {}

// Default constructor
Camera::Camera(const Vec3 &position, const Vec3 &target, const Vec3 &normal, GLfloat fovY)
	: pos(position), target(target), normal(normal), fovY(fovY) {}

// Apply the camera view using gluLookAt
void Camera::applyView() const
{
	// Reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply the camera view
	gluLookAt(pos.x, pos.y, pos.z,
			  target.x, target.y, target.z,
			  normal.x, normal.y, normal.z);
}

std::ostream &operator<<(std::ostream &out, const Camera &c)
{
	out << "Camera Parameters:\n";
	out << "Position: " << c.getPosition() << "\n";
	out << "Target: " << c.getTarget() << "\n";
	out << "Normal: " << c.getNormal() << "\n";
	out << "Field of View (Y): " << c.getFOV() << " degrees";
	return out;
}
