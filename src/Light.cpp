#include "../include/Light.h"

Light::Light(const Vec3& position, const Vec3& color,
	const GLfloat rho0, const GLfloat rho1, const GLfloat rho2,
	const GLenum lightID)
	: pos(position), rgbColor(color),
	rho_0(rho0), rho_1(rho1), rho_2(rho2),
	glLightID(lightID) {}

// Apply the light parameters in OpenGL
void Light::applyLight() const
{
	GLfloat position[] = { pos.x, pos.y, pos.z, 1.0f };
	GLfloat color[] = { rgbColor.x, rgbColor.y, rgbColor.z, 1.0f };

	glEnable(glLightID);
	glLightfv(glLightID, GL_POSITION, position);
	glLightfv(glLightID, GL_DIFFUSE, color);
	glLightfv(glLightID, GL_SPECULAR, color);
	glLightf(glLightID, GL_CONSTANT_ATTENUATION, rho_0);
	glLightf(glLightID, GL_LINEAR_ATTENUATION, rho_1);
	glLightf(glLightID, GL_QUADRATIC_ATTENUATION, rho_2);
}

std::ostream& operator<<(std::ostream& out, const Light& light)
{
	std::cout << "Light Parameters:\n";
	std::cout << "Position: " << light.getPosition() << "\n";
	std::cout << "Color: " << light.getColor() << "\n";
	std::cout << "Attenuation Coefficients: "
		<< "rho_0 = " << light.getRho0() << ", "
		<< "rho_1 = " << light.getRho1() << ", "
		<< "rho_2 = " << light.getRho2();
	return out;
}
