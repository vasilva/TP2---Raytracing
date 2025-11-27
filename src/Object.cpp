#include "../include/Object.h"
#include "../include/SolidPigment.h"
#include "../include/CheckerPigment.h"
#include "../include/TexmapPigment.h"
#include "../include/Pigment.h"

// Apply the surface finish material properties to OpenGL
void Object::applyMaterials() const
{
	auto ambient = finish->getAmbient();
	auto diffuse = finish->getDiffuse();
	auto specular = finish->getSpecular();
	auto shininess = finish->getAlpha();

	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialf(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// Apply the pigment color at the given point to OpenGL
void Object::applyPigmentColor(const Vec4& point) const
{
	auto pigmentType = pigment->type;
	switch (pigmentType)
	{
	case Pigment::SOLID:
	{
		auto solidPigment = static_cast<SolidPigment*>(pigment);
		Vec3 color = solidPigment->getColor(point);
		GLfloat colorArr[] = { color.x, color.y, color.z, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorArr);
		break;
	}
	case Pigment::CHECKER:
	{
		auto checkerPigment = static_cast<CheckerPigment*>(pigment);
		Vec3 color = checkerPigment->getColor(point);
		GLfloat colorArr[] = { color.x, color.y, color.z, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorArr);
		break;
	}
	case Pigment::TEXMAP:
	{
		auto texPigment = static_cast<TexmapPigment*>(pigment);
		Vec3 color = texPigment->getColor(point);
		GLfloat colorArr[] = { color.x, color.y, color.z, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colorArr);
		break;
	}
	default: break;
	}
}
