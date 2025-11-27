#include "../include/Sphere.h"
#include "../include/CheckerPigment.h"
#include "../include/TexmapPigment.h"

Sphere::Sphere(Pigment *p, SurfaceFinish *sf,
			   const Vec3 &center, const GLfloat radius)
	: Object(Object::Sphere, p, sf),
	  center(center), radius(radius) {}

std::ostream &operator<<(std::ostream &out, const Sphere &s)
{
	out << "Sphere:" << std::endl;
	out << "  Center: " << s.center << std::endl;
	out << "  Radius: " << s.radius << std::endl;
	return out;
}

// Render the sphere using OpenGL
void Sphere::draw() const
{
	// Parameters for sphere tessellation
	const int latSteps = 32;
	const int lonSteps = 64;

	auto pigment = this->getPigment();
	if (!pigment)
		return;

	glBegin(GL_QUADS);
	for (int i = 0; i < latSteps; ++i)
	{
		float theta0 = (float(i) / latSteps) * (float)PI - (float)PI / 2.0f;
		float theta1 = (float(i + 1) / latSteps) * (float)PI - (float)PI / 2.0f;
		for (int j = 0; j < lonSteps; ++j)
		{
			float phi0 = (float(j) / lonSteps) * 2.0f * (float)PI;
			float phi1 = (float(j + 1) / lonSteps) * 2.0f * (float)PI;

			// Compute vertices on the sphere
			Vec3 v00 = Vec3(cosf(theta0) * cosf(phi0), sinf(theta0), cosf(theta0) * sinf(phi0));
			Vec3 v10 = Vec3(cosf(theta1) * cosf(phi0), sinf(theta1), cosf(theta1) * sinf(phi0));
			Vec3 v11 = Vec3(cosf(theta1) * cosf(phi1), sinf(theta1), cosf(theta1) * sinf(phi1));
			Vec3 v01 = Vec3(cosf(theta0) * cosf(phi1), sinf(theta0), cosf(theta0) * sinf(phi1));

			// Scale to sphere radius and translate to center
			Vec3 p00 = center + v00 * radius;
			Vec3 p10 = center + v10 * radius;
			Vec3 p11 = center + v11 * radius;
			Vec3 p01 = center + v01 * radius;

			// Sample point at the center of the quad
			Vec3 centerPoint = (p00 + p10 + p11 + p01) * 0.25f;
			Vec4 samplePoint(centerPoint.x, centerPoint.y, centerPoint.z, 1.0f);

			// Get color from pigment
			Vec3 color;
			if (pigment->type == Pigment::CHECKER)
			{
				// Use spherical mapping for checker on spheres
				auto checker = static_cast<CheckerPigment *>(pigment);
				color = checker->getColorOnSphere(samplePoint, center);
			}
			else if (pigment->type == Pigment::TEXMAP)
			{
				auto tex = static_cast<TexmapPigment *>(pigment);
				color = tex->getColorOnSphere(samplePoint, center);
			}
			else
				color = pigment->getColor(samplePoint);

			// Normals (same as vertices for a sphere)
			Vec3 n00 = normalize(v00);
			Vec3 n10 = normalize(v10);
			Vec3 n11 = normalize(v11);
			Vec3 n01 = normalize(v01);

			// Set color and emit vertices
			glColor3f(color.x, color.y, color.z);

			glNormal3f(n00.x, n00.y, n00.z);
			glVertex3f(p00.x, p00.y, p00.z);

			glNormal3f(n10.x, n10.y, n10.z);
			glVertex3f(p10.x, p10.y, p10.z);

			glNormal3f(n11.x, n11.y, n11.z);
			glVertex3f(p11.x, p11.y, p11.z);

			glNormal3f(n01.x, n01.y, n01.z);
			glVertex3f(p01.x, p01.y, p01.z);
		}
	}
	glEnd();
}
