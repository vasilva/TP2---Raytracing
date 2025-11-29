#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "GL/glut.h"
#include "vecFunctions.h"
#include "Camera.h"
#include "Light.h"
#include "Pigment.h"
#include "CheckerPigment.h"
#include "SolidPigment.h"
#include "TexmapPigment.h"
#include "SurfaceFinish.h"
#include "Object.h"
#include "Sphere.h"
#include "Polyhedron.h"

// Function to read scene inputs from a file
// Helper function to read camera parameters
void readCamera(std::ifstream &inFile, Camera &camera)
{
	Vec3 pos, target, normal;
	GLfloat fovY;

	inFile >> pos.x >> pos.y >> pos.z;
	camera.setPosition(pos);

	inFile >> target.x >> target.y >> target.z;
	camera.setTarget(target);

	inFile >> normal.x >> normal.y >> normal.z;
	camera.setNormal(normal);

	inFile >> fovY;
	camera.setFOV(fovY);
}

// Helper function to read lights
void readLights(std::ifstream &inFile, std::vector<Light> &lights)
{
	int numLights;
	inFile >> numLights;

	for (int i = 0; i < numLights; ++i)
	{
		Vec3 lightPos, lightColor;
		GLfloat rho0, rho1, rho2;

		inFile >> lightPos.x >> lightPos.y >> lightPos.z;
		inFile >> lightColor.x >> lightColor.y >> lightColor.z;
		inFile >> rho0 >> rho1 >> rho2;

		Light light(lightPos, lightColor, rho0, rho1, rho2, GL_LIGHT0 + i);
		lights.push_back(light);
	}
}

// Helper function to read pigments
void readPigments(std::ifstream &inFile, std::vector<std::unique_ptr<Pigment>> &pigments)
{
	int numPigments, numTextures = 0;
	inFile >> numPigments;

	// Read each pigment
	for (int i = 0; i < numPigments; ++i)
	{
		std::string pigmentType;
		inFile >> pigmentType;

		// Create pigment based on type
		if (pigmentType == "solid")
		{
			Vec3 color;
			inFile >> color.x >> color.y >> color.z;
			pigments.push_back(std::make_unique<SolidPigment>(color));
		}
		else if (pigmentType == "checker")
		{
			Vec3 color1, color2;
			GLfloat size;
			inFile >> color1.x >> color1.y >> color1.z;
			inFile >> color2.x >> color2.y >> color2.z;
			inFile >> size;
			pigments.push_back(std::make_unique<CheckerPigment>(color1, color2, size));
		}
		else if (pigmentType == "texmap")
		{
			std::string texFilename;
			Vec4 p0, p1;
			inFile >> texFilename;
			inFile >> p0.x >> p0.y >> p0.z >> p0.w;
			inFile >> p1.x >> p1.y >> p1.z >> p1.w;
			pigments.push_back(std::make_unique<TexmapPigment>(texFilename, p0, p1, ++numTextures));
		}
		else // Unknown pigment type
			std::cerr << "Warning: Unknown pigment type '" << pigmentType << "'; skipping.\n";
	}
}

// Helper function to read surface finishes
void readSurfaceFinishes(std::ifstream &inFile, std::vector<std::unique_ptr<SurfaceFinish>> &finishes)
{
	int numFinishes;
	inFile >> numFinishes;

	for (int i = 0; i < numFinishes; ++i)
	{
		GLfloat ka, kd, ks, a, kr, kt, ior;
		inFile >> ka >> kd >> ks >> a >> kr >> kt >> ior;
		finishes.push_back(std::make_unique<SurfaceFinish>(ka, kd, ks, a, kr, kt, ior));
	}
}

// Helper function to read surfaces
void readSurfaces(std::ifstream &inFile,
				  std::vector<std::unique_ptr<Pigment>> &pigments,
				  std::vector<std::unique_ptr<SurfaceFinish>> &finishes,
				  std::vector<std::unique_ptr<Object>> &surfaces)
{
	int numSurfaces;
	inFile >> numSurfaces;

	// Read each surface
	for (int i = 0; i < numSurfaces; ++i)
	{
		int pigmentIndex, finishIndex;
		std::string surfaceType;
		inFile >> pigmentIndex >> finishIndex >> surfaceType;

		// Create surface based on type
		if (surfaceType == "sphere")
		{
			Vec3 center;
			GLfloat radius;
			inFile >> center.x >> center.y >> center.z >> radius;
			auto sphere = std::make_unique<Sphere>(
				pigments[pigmentIndex].get(),
				finishes[finishIndex].get(),
				center, radius);
			surfaces.push_back(std::move(sphere));
		}
		else if (surfaceType == "polyhedron")
		{
			size_t numFaces;
			inFile >> numFaces;
			auto poly = std::make_unique<Polyhedron>(
				pigments[pigmentIndex].get(),
				finishes[finishIndex].get(),
				numFaces);

			for (size_t j = 0; j < numFaces; ++j)
			{
				Vec4 plane;
				inFile >> plane.x >> plane.y >> plane.z >> plane.w;
				poly->addPlane(plane);
			}
			surfaces.push_back(std::move(poly));
		}
	}
}

// Main function to read scene inputs from a file
void readInputs(const std::string &filename, Camera &camera,
				std::vector<Light> &lights,
				std::vector<std::unique_ptr<Pigment>> &pigments,
				std::vector<std::unique_ptr<SurfaceFinish>> &finishes,
				std::vector<std::unique_ptr<Object>> &surfaces)
{
	// Try to open the file directly; if it fails, try with DATA_PATH prefix
	std::ifstream inFile(filename);
	std::string fullPath = filename;
	
	// Base path for input files
	const std::string DATA_PATH = "data/scenes/"; 

	if (!inFile.is_open())
	{
		// Try with DATA_PATH prefix
		fullPath = DATA_PATH + filename;
		inFile.open(fullPath);
	}
	if (!inFile.is_open())
	{
		// Could not open file
		std::cerr << "Error: Could not open file " << filename << " or " << fullPath << std::endl;
		return;
	}
	std::cout << "File " << fullPath << " opened successfully." << std::endl;

	// Read scene components
	readCamera(inFile, camera);
	readLights(inFile, lights);
	readPigments(inFile, pigments);
	readSurfaceFinishes(inFile, finishes);
	readSurfaces(inFile, pigments, finishes, surfaces);

	inFile.close();
}
