#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>

#include "GL/glut.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Raytracer.h"
#include "vecFunctions.h"

// Registered scene components
static Camera *sCamera = nullptr;
static std::vector<std::unique_ptr<Object>> *sSurfaces = nullptr;
static std::vector<Light> *sLights = nullptr;
static Raytracer *sRaytracer = nullptr;

// Raytracing toggle and framebuffer
static bool sRaytraceEnabled = true;
static std::vector<unsigned char> sFramebuffer;
static int sImageWidth = 800;
static int sImageHeight = 600;

// Flags to avoid re-rendering
static bool sNeedRender = true;
static bool sFramebufferValid = false;
static bool sPpmSaved = false;
static std::string sOutputFilename = "";

// Track which effects are enabled
static bool sSoftShadowsEnabled = false;
static bool sDOFEnabled = false;
static bool sMotionBlurEnabled = false;

// Forward declaration
static void writePPM(const std::string &outputFilename);

// Render the raytraced image into the framebuffer
static void renderRaytracedImage(void)
{
	if (!sRaytracer)
		return;
	sRaytracer->render(sImageWidth, sImageHeight, sFramebuffer);
}

// GLUT display callback
static void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!sCamera)
		return;

	if (sRaytraceEnabled)
	{
		if (sNeedRender)
		{
			renderRaytracedImage();
			sNeedRender = false;
			sFramebufferValid = true;

			// Save PPM after first render
			if (!sPpmSaved && !sOutputFilename.empty())
			{
				writePPM(sOutputFilename);
				sPpmSaved = true;
			}
		}
		size_t expectedSize = static_cast<size_t>(sImageWidth * sImageHeight * 3);
		if (sFramebufferValid && sFramebuffer.size() == expectedSize)
			glDrawPixels(sImageWidth, sImageHeight, GL_RGB, GL_UNSIGNED_BYTE, sFramebuffer.data());
	}
	else
	{
		sCamera->applyView();
		if (sSurfaces)
			for (const auto &surface : *sSurfaces)
				surface->draw();
	}
	glutSwapBuffers();
}

// GLUT idle callback
static void idle(void) { glutPostRedisplay(); }

// GLUT reshape callback
static void reshape(int w, int h)
{
	// Update framebuffer size
	sImageWidth = std::max(1, w);
	sImageHeight = std::max(1, h);

	// Safely resize framebuffer
	size_t newSize = static_cast<size_t>(sImageWidth * sImageHeight * 3);
	if (newSize > 0 && newSize < 100000000) // Sanity check: less than 100MB
	{
		try
		{
			sFramebuffer.resize(newSize);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error resizing framebuffer: " << e.what() << std::endl;
			sFramebuffer.clear();
		}
	}

	// Request re-render after resize
	sNeedRender = true;
	sFramebufferValid = false;

	// Set viewport and projection
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	GLfloat fovY = sCamera ? sCamera->getFOV() : 45.0f;
	GLfloat nearDist = 0.1f;
	GLfloat top = nearDist * tanf(fovY * PI / 360.0f);
	GLfloat bottom = -top;
	GLfloat right = top * aspect;
	GLfloat left = -right;
	glFrustum(left, right, bottom, top, nearDist, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// GLUT keyboard callback
static void keyboard(unsigned char key, int, int)
{
	switch (key)
	{
	case 27: // ESC key
		exit(0);
		break;

	case 'r':
	case 'R':
		sRaytraceEnabled = !sRaytraceEnabled;
		if (sRaytraceEnabled)
			sNeedRender = true;
		std::cout << "Raytracing " << (sRaytraceEnabled ? "enabled." : "disabled.") << std::endl;
		break;

	case '1': // Toggle soft shadows
		if (sRaytracer)
		{
			sSoftShadowsEnabled = !sSoftShadowsEnabled;
			sRaytracer->setSoftShadows(sSoftShadowsEnabled);
			sNeedRender = true;
			sPpmSaved = false;
			std::cout << "Soft shadows " << (sSoftShadowsEnabled ? "enabled" : "disabled") << std::endl;
		}
		break;

	case '2': // Toggle depth of field
		if (sRaytracer)
		{
			sDOFEnabled = !sDOFEnabled;
			sRaytracer->setDepthOfField(sDOFEnabled, 2.0f, 150.0f);
			sNeedRender = true;
			sPpmSaved = false;
			std::cout << "Depth of field " << (sDOFEnabled ? "enabled" : "disabled") << std::endl;
		}
		break;

	case '3': // Toggle motion blur
		if (sRaytracer)
		{
			sMotionBlurEnabled = !sMotionBlurEnabled;
			sRaytracer->setMotionBlur(sMotionBlurEnabled, 0.5f);
			sNeedRender = true;
			sPpmSaved = false;
			std::cout << "Motion blur " << (sMotionBlurEnabled ? "enabled" : "disabled") << std::endl;
		}
		break;

	default:
		break;
	}
}

// Register scene objects
static void registerObjects(Camera *camera,
							std::vector<std::unique_ptr<Object>> *surfaces,
							std::vector<Light> *lights)
{
	sCamera = camera;
	sSurfaces = surfaces;
	sLights = lights;

	// Create raytracer instance
	if (sRaytracer)
		delete sRaytracer;
	sRaytracer = new Raytracer(camera, surfaces, lights);

	// ensure next display triggers render
	sNeedRender = true;
	sFramebufferValid = false;
}

// Set output filename for PPM
static inline void setOutputFilename(const std::string &filename) { sOutputFilename = filename; }

// Write the raytraced image to a PPM file
static void writePPM(const std::string &outputFilename)
{
	if (sFramebuffer.empty())
	{
		std::cerr << "Error: Framebuffer not initialized" << std::endl;
		return;
	}

	// Generate filename based on active effects
	std::string finalFilename = outputFilename;
	size_t dotPos = finalFilename.find_last_of('.');
	std::string baseName = (dotPos != std::string::npos) ? finalFilename.substr(0, dotPos) : finalFilename;
	std::string extension = (dotPos != std::string::npos) ? finalFilename.substr(dotPos) : ".ppm";

	if (sSoftShadowsEnabled)
		finalFilename = baseName + "_soft" + extension;
	else if (sDOFEnabled)
		finalFilename = baseName + "_dof" + extension;
	else if (sMotionBlurEnabled)
		finalFilename = baseName + "_blur" + extension;
	// If no effects, use original filename

	// Write PPM file
	std::ofstream outFile("data/output/" + finalFilename, std::ios::binary);
	if (!outFile)
	{
		std::cerr << "Error: Could not open output file " << outputFilename << std::endl;
		return;
	}
	outFile << "P6\n"
			<< sImageWidth << " " << sImageHeight << "\n255\n";
	const size_t rowBytes = static_cast<size_t>(sImageWidth * 3);
	for (int row = sImageHeight - 1; row >= 0; --row)
	{
		size_t offset = static_cast<size_t>(row * sImageWidth * 3);
		outFile.write(reinterpret_cast<const char *>(sFramebuffer.data() + offset), rowBytes);
	}

	// Check for write errors
	if (!outFile.good())
		std::cerr << "Error: Failed to write to output file " << finalFilename << std::endl;
	else
		std::cout << "Image successfully written to data/output/" << finalFilename << std::endl;
	outFile.close();
}
