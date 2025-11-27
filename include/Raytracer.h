#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

#include "GL/glut.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Polyhedron.h"
#include "Pigment.h"
#include "TexmapPigment.h"
#include "SurfaceFinish.h"
#include "vecFunctions.h"

class Raytracer
{
public:
	Raytracer(Camera* camera, 
			  std::vector<std::unique_ptr<Object>>* surfaces,
			  std::vector<Light>* lights);

	// Render the scene to a framebuffer
	void render(int width, int height, std::vector<unsigned char>& framebuffer);

	// Configuration for distributed ray tracing
	void setSoftShadows(bool enable, int samples = 4);
	void setDepthOfField(bool enable, GLfloat aperture = 0.5f, GLfloat focalDistance = 150.0f, int samples = 8);
	void setMotionBlur(bool enable, GLfloat shutterTime = 0.5f, int samples = 4);

	// Ray intersection methods
	bool intersectSphere(const Sphere* sphere, const Vec3& ro, const Vec3& rd, 
						 GLfloat& outT, Vec3& outN) const;
	bool intersectPolyhedron(const Polyhedron* poly, const Vec3& ro, const Vec3& rd,
							 GLfloat& outT, Vec3& outN) const;

	// Ray tracing
	Vec3 traceRay(const Vec3& ro, const Vec3& rd, int depth, GLfloat time = 0.0f) const;

private:
	Camera* mCamera;
	std::vector<std::unique_ptr<Object>>* mSurfaces;
	std::vector<Light>* mLights;

	// Distributed ray tracing settings
	bool mSoftShadowsEnabled = false;
	int mShadowSamples = 4;
	
	bool mDepthOfFieldEnabled = false;
	GLfloat mAperture = 0.5f;
	GLfloat mFocalDistance = 150.0f;
	int mDOFSamples = 8;
	
	bool mMotionBlurEnabled = false;
	GLfloat mShutterTime = 0.5f;
	int mMotionBlurSamples = 4;

	// Random number generator (mutable for const methods)
	mutable std::mt19937 mRNG;
	
	// Helper functions for distributed ray tracing
	Vec3 randomInUnitDisk() const;
	Vec3 sampleAreaLight(const Light& light) const;
	Vec3 getObjectPosition(const Object* obj, GLfloat time) const;

	static constexpr int MAX_DEPTH = 3;
	static constexpr GLfloat EPS = 1e-4f;
	static constexpr GLfloat INF = 1e9f;
};
