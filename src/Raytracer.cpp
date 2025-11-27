#include "../include/Raytracer.h"

Raytracer::Raytracer(Camera* camera,
					 std::vector<std::unique_ptr<Object>>* surfaces,
					 std::vector<Light>* lights)
	: mCamera(camera), mSurfaces(surfaces), mLights(lights), mRNG(42)
{
	// RNG initialized with fixed seed for reproducibility
	// Can be changed to std::random_device for true randomness
}

// Configuration methods
void Raytracer::setSoftShadows(bool enable, int samples)
{
	mSoftShadowsEnabled = enable;
	mShadowSamples = samples;
}

void Raytracer::setDepthOfField(bool enable, GLfloat aperture, GLfloat focalDistance, int samples)
{
	mDepthOfFieldEnabled = enable;
	mAperture = aperture;
	mFocalDistance = focalDistance;
	mDOFSamples = samples;
}

void Raytracer::setMotionBlur(bool enable, GLfloat shutterTime, int samples)
{
	mMotionBlurEnabled = enable;
	mShutterTime = shutterTime;
	mMotionBlurSamples = samples;
}

// Helper: Random point in unit disk (for DOF)
Vec3 Raytracer::randomInUnitDisk() const
{
	std::uniform_real_distribution<GLfloat> dist(-1.0f, 1.0f);
	while (true)
	{
		Vec3 p(dist(mRNG), dist(mRNG), 0.0f);
		if (lengthSq(p) < 1.0f)
			return p;
	}
}

// Helper: Sample area light (soft shadows)
Vec3 Raytracer::sampleAreaLight(const Light& light) const
{
	if (!mSoftShadowsEnabled)
		return light.getPosition();
	
	std::uniform_real_distribution<GLfloat> dist(-1.0f, 1.0f);
	GLfloat lightRadius = 10.0f; // Area light radius
	
	Vec3 offset(dist(mRNG) * lightRadius, dist(mRNG) * lightRadius, dist(mRNG) * lightRadius);
	return light.getPosition() + offset;
}

// Helper: Get object position at given time (for motion blur)
Vec3 Raytracer::getObjectPosition(const Object* obj, GLfloat time) const
{
	if (!mMotionBlurEnabled || obj->getType() != Object::Sphere)
	{
		if (obj->getType() == Object::Sphere)
			return static_cast<const Sphere*>(obj)->getCenter();
		return Vec3(0, 0, 0);
	}
	
	// Simple linear motion - could be extended with velocity vectors
	const Sphere* sphere = static_cast<const Sphere*>(obj);
	Vec3 basePos = sphere->getCenter();
	
	// Example: objects with y < 0 move horizontally
	if (basePos.y < 0.0f)
	{
		GLfloat velocity = 20.0f; // units per second
		basePos.x += velocity * time;
	}
	return basePos;
}

bool Raytracer::intersectSphere(const Sphere* sphere, const Vec3& ro, const Vec3& rd,
								GLfloat& outT, Vec3& outN) const
{
	// Ray-sphere intersection using quadratic formula
	Vec3 oc = ro - sphere->getCenter();
	GLfloat radius = sphere->getRadius();

	// Coefficients for at^2 + 2*half_b*t + c = 0
	GLfloat a = lengthSq(rd);
	if (a <= 1e-12f)
		return false; // avoid degenerate direction

	GLfloat half_b = dot(oc, rd);
	GLfloat c = lengthSq(oc) - radius * radius;

	GLfloat delta = half_b * half_b - a * c;
	if (delta < 0.0f)
		return false;

	GLfloat sqrtD = std::sqrt(delta);

	// Try the nearer root first
	GLfloat t = (-half_b - sqrtD) / a;
	if (t <= EPS)
	{
		t = (-half_b + sqrtD) / a;
		if (t <= EPS)
			return false;
	}
	// Valid intersection
	outT = t;
	Vec3 hitPoint = ro + rd * t;
	outN = normalize(hitPoint - sphere->getCenter());
	return true;
}

bool Raytracer::intersectPolyhedron(const Polyhedron* poly, const Vec3& ro, const Vec3& rd,
									GLfloat& outT, Vec3& outN) const
{
	// Get planes of the polyhedron
	const auto planes = poly->getPlanes();
	GLfloat tEnter = -std::numeric_limits<GLfloat>::infinity();
	GLfloat tExit = std::numeric_limits<GLfloat>::infinity();
	Vec3 enterNormal(0, 0, 0);
	const GLfloat PLANE_EPS = 1e-6f;

	// Iterate over each plane of the polyhedron
	for (const auto& pl : planes)
	{
		Vec3 n(pl.x, pl.y, pl.z);
		GLfloat d = pl.w;
		GLfloat denom = dot(n, rd);
		GLfloat numer = -(dot(n, ro) + d);

		if (std::fabs(denom) < PLANE_EPS)
		{
			// Ray parallel to plane: if numer < 0 -> outside
			if (numer < 0.0f)
				return false;
			else
				continue;
		}

		GLfloat t = numer / denom;
		if (denom < 0.0f)
		{
			// entering
			if (t > tEnter)
			{
				tEnter = t;
				enterNormal = n;
			}
		}
		else
		{
			// exiting
			if (t < tExit)
				tExit = t;
		}

		if (tEnter - tExit > PLANE_EPS)
			return false;
	}

	// Determine the valid intersection t
	GLfloat tHit = (tEnter > EPS) ? tEnter : ((tExit > EPS) ? tExit : -1.0f);
	if (tHit < 0)
		return false;
	outT = tHit;
	outN = normalize(enterNormal);
	return true;
}

Vec3 Raytracer::traceRay(const Vec3& ro, const Vec3& rd, int depth, GLfloat time) const
{
	if (!mSurfaces)
		return ZERO_3D;

	// Find nearest intersection
	GLfloat nearestT = INF;
	Object* nearestObj = nullptr;
	Vec3 nearestN;

	// Iterate over all surfaces to find the nearest intersection
	for (const auto& surface : *mSurfaces)
	{
		const Object* obj = surface.get();
		if (obj->getType() == Object::Sphere)
		{
			// Sphere intersection
			const Sphere* sphere = static_cast<const Sphere*>(obj);
			GLfloat t;
			Vec3 n;
			if (intersectSphere(sphere, ro, rd, t, n) && t < nearestT)
			{
				nearestT = t;
				nearestObj = surface.get();
				nearestN = n;
			}
		}
		else if (obj->getType() == Object::Polyhedron)
		{
			// Polyhedron intersection
			const Polyhedron* poly = static_cast<const Polyhedron*>(obj);
			GLfloat t;
			Vec3 n;
			if (intersectPolyhedron(poly, ro, rd, t, n) && t < nearestT)
			{
				nearestT = t;
				nearestObj = surface.get();
				nearestN = n;
			}
		}
	}
	if (!nearestObj)
		return ONE_3D; // No intersection - white background

	// Compute hit point and color from pigment
	Vec3 hitPoint = ro + rd * nearestT;
	Vec4 samplePoint(hitPoint.x, hitPoint.y, hitPoint.z, 1.0f);
	Pigment* pigment = nearestObj->getPigment();

	// If the object is a sphere and pigment is a texmap, use spherical mapping for base color
	Vec3 baseColor;
	if (pigment && pigment->type == Pigment::TEXMAP && nearestObj->getType() == Object::Sphere)
	{
		auto tex = static_cast<TexmapPigment*>(pigment);
		const Sphere* sph = static_cast<const Sphere*>(nearestObj);
		baseColor = tex->getColorOnSphere(samplePoint, sph->getCenter());
	}
	else
	{
		baseColor = pigment ? pigment->getColor(samplePoint) : ONE_3D;
	}

	// Material
	SurfaceFinish* finish = nearestObj->getFinish();
	GLfloat kAmbient = finish ? finish->getAmbient() : 0.1f;
	GLfloat kDiffuse = finish ? finish->getDiffuse() : 0.7f;
	GLfloat kSpecular = finish ? finish->getSpecular() : 0.2f;
	GLfloat alpha = finish ? finish->getAlpha() : 10.0f;
	GLfloat kReflection = finish ? finish->getReflection() : 0.0f;
	GLfloat kTransmission = finish ? finish->getTransmission() : 0.0f;
	GLfloat ior = finish ? finish->getIOR() : 1.0f;

	// Ambient light from the first light source
	Vec3 ambientLight = ONE_3D;
	if (mLights && !mLights->empty())
		ambientLight = (*mLights)[0].getColor();

	// Start color with ambient
	Vec3 color = baseColor * kAmbient * ambientLight;

	// Iterate remaining lights (index 1..end) for diffuse/specular
	if (mLights)
	{
		for (size_t li = 1; li < mLights->size(); ++li)
		{
			const Light& light = (*mLights)[li];
			
			// Soft shadows: sample the area light multiple times
			int shadowSamples = mSoftShadowsEnabled ? mShadowSamples : 1;
			GLfloat shadowFactor = 0.0f;
			
			for (int si = 0; si < shadowSamples; ++si)
			{
				Vec3 lightPos = sampleAreaLight(light);
				Vec3 l = lightPos - hitPoint;
				GLfloat dist = length(l);
				l = normalize(l);

				// Shadow ray test
				bool hitShadow = false;
				Vec3 shadowRo = hitPoint + nearestN * EPS;
				Vec3 shadowRd = l;
				
				for (const auto& surfPtr2 : *mSurfaces)
				{
					const Object* obj2 = surfPtr2.get();
					if (obj2 == nearestObj)
						continue;
					GLfloat t2;
					Vec3 n2;
					if (obj2->getType() == Object::Sphere)
					{
						if (intersectSphere(static_cast<const Sphere*>(obj2), shadowRo, shadowRd, t2, n2))
						{
							hitShadow = (t2 > EPS && t2 < dist);
						}
					}
					else
					{
						if (intersectPolyhedron(static_cast<const Polyhedron*>(obj2), shadowRo, shadowRd, t2, n2))
						{
							hitShadow = (t2 > EPS && t2 < dist);
						}
					}
				if (hitShadow)
					break;
			}
			
			if (!hitShadow)
				shadowFactor += 1.0f / shadowSamples;
		}
		
		// Skip light calculation entirely if completely in shadow
		if (shadowFactor <= 0.0f)
			continue;

		Vec3 l = light.getPosition() - hitPoint;
		GLfloat dist = length(l);
		l = normalize(l);

		GLfloat nDotL = std::max(dot(nearestN, l), 0.0f);
		
		// Skip if light is behind surface
		if (nDotL <= 0.0f)
			continue;
		
		GLfloat d = dist;
		GLfloat att = 1.0f / std::max(1e-6f, light.getRho0() + light.getRho1() * d + light.getRho2() * d * d);
		Vec3 lightColor = light.getColor();

			// Diffuse (attenuated by shadow factor)
			color += baseColor * (kDiffuse * nDotL * att * shadowFactor) * lightColor;

			// Specular (attenuated by shadow factor)
			Vec3 v = normalize(mCamera->getPosition() - hitPoint);
			Vec3 r = normalize((nearestN * (2.0f * dot(nearestN, l))) - l);
			GLfloat rDotV = std::max(dot(r, v), 0.0f);
			color += lightColor * (kSpecular * std::pow(rDotV, alpha) * att * shadowFactor);
		}
	}

	// Reflection and Transmission (refraction) combined
	Vec3 reflectedColor = ZERO_3D;
	Vec3 transmittedColor = ZERO_3D;

	// Reflection - skip if negligible
	if (kReflection > 0.01f && depth < MAX_DEPTH)
	{
		// Perfect specular reflection
		Vec3 reflectDir = normalize(rd - nearestN * (2.0f * dot(rd, nearestN)));
		Vec3 reflectRo = hitPoint + nearestN * EPS;
		reflectedColor = traceRay(reflectRo, reflectDir, depth + 1, time);
	}

	// Transmission / Refraction using Snell's law - skip if negligible
	if (kTransmission > 0.01f && depth < MAX_DEPTH)
	{
		// Determine indices depending on entering/exiting
		Vec3 N = nearestN;
		GLfloat cosi = std::clamp(dot(rd, N), -1.0f, 1.0f);
		GLfloat eta_i = 1.0f; // assume air
		GLfloat eta_t = ior;
		if (cosi < 0.0f)
		{
			// Ray is outside hitting the surface: cosi = -dot(rd,N)
			cosi = -cosi;
		}
		else
		{
			// Ray is inside the object: swap indices and flip normal
			std::swap(eta_i, eta_t);
			N *= -1.0f;
			// recompute cosi as positive
			cosi = std::clamp(dot(rd, N), -1.0f, 1.0f);
			cosi = std::fabs(cosi);
		}

		GLfloat eta = eta_i / eta_t;
		GLfloat k = 1.0f - eta * eta * (1.0f - cosi * cosi);

		if (k < 0.0f)
		{
			// Total internal reflection
			if (kReflection <= 0.0f)
			{
				Vec3 tirDir = normalize(rd - nearestN * (2.0f * dot(rd, nearestN)));
				Vec3 tirRo = hitPoint + nearestN * EPS;
				reflectedColor = traceRay(tirRo, tirDir, depth + 1, time);
			}
		}
		else
		{
			Vec3 refractDir = normalize(rd * eta + N * (eta * cosi - std::sqrt(k)));
			Vec3 refractRo = hitPoint - N * EPS;
			transmittedColor = traceRay(refractRo, refractDir, depth + 1, time);
		}
	}

	// Combine base shading with reflection/transmission contributions
	GLfloat mixSum = kReflection + kTransmission;
	if (mixSum > 1.0f)
	{
		kReflection /= mixSum;
		kTransmission /= mixSum;
	}

	GLfloat localWeight = 1.0f - (kReflection + kTransmission);
	if (localWeight < 0.0f)
		localWeight = 0.0f;

	Vec3 finalColor = color * localWeight;
	if (kReflection > 0.0f)
		finalColor += reflectedColor * kReflection;
	if (kTransmission > 0.0f)
		finalColor += transmittedColor * kTransmission;

	// Clamp color to [0,1]
	finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
	finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
	finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);
	return finalColor;
}

void Raytracer::render(int width, int height, std::vector<unsigned char>& framebuffer)
{
	if (!mCamera || !mSurfaces)
	{
		std::cerr << "Error: Camera or surfaces not initialized" << std::endl;
		return;
	}

	if (width <= 0 || height <= 0 || width > 10000 || height > 10000)
	{
		std::cerr << "Error: Invalid dimensions " << width << "x" << height << std::endl;
		return;
	}

	std::cout << "Starting raytracing render: " << width << "x" << height << std::endl;

	// Ensure framebuffer is properly sized
	size_t expected = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
	std::cout << "Allocating framebuffer: " << expected << " bytes" << std::endl;
	
	try {
		if (framebuffer.size() != expected)
			framebuffer.assign(expected, 255u);
	} catch (const std::exception& e) {
		std::cerr << "Error allocating framebuffer: " << e.what() << std::endl;
		return;
	}

	// Prepare camera basis
	Vec3 eye = mCamera->getPosition();
	Vec3 target = mCamera->getTarget();
	Vec3 upV = mCamera->getNormal();

	Vec3 forward = normalize(target - eye);
	Vec3 right = normalize(cross(forward, upV));
	Vec3 up = normalize(cross(right, forward));

	GLfloat fovY = mCamera->getFOV();
	GLfloat aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
	GLfloat top = tanf(fovY * PI / 360.0f);
	GLfloat rightPlane = top * aspect;

	std::cout << "Rendering pixels..." << std::endl;
	int progressStep = height / 10;
	if (progressStep == 0) progressStep = 1;

	std::uniform_real_distribution<GLfloat> pixelJitter(-0.5f, 0.5f);
	std::uniform_real_distribution<GLfloat> timeDist(0.0f, 1.0f);

	// Loop over pixels
	for (int j = 0; j < height; ++j)
	{
		if (j % progressStep == 0)
			std::cout << "Progress: " << (j * 100 / height) << "%" << std::endl;
		
		for (int i = 0; i < width; ++i)
		{
			Vec3 col(0, 0, 0);
			
			// Multi-sampling for DOF and/or Motion Blur
			int totalSamples = 1;
			if (mDepthOfFieldEnabled) totalSamples = mDOFSamples;
			// Motion blur uses temporal sampling but not more rays per pixel
			
			for (int s = 0; s < totalSamples; ++s)
			{
				// Jitter pixel position ONLY if we have many samples (for anti-aliasing)
				GLfloat jitterX = (totalSamples >= 8) ? pixelJitter(mRNG) : 0.0f;
				GLfloat jitterY = (totalSamples >= 8) ? pixelJitter(mRNG) : 0.0f;
				
				// NDC screen space (-1..1)
				GLfloat u = ((i + 0.5f + jitterX) / width) * 2.0f - 1.0f;
				GLfloat v = ((j + 0.5f + jitterY) / height) * 2.0f - 1.0f;
				u *= rightPlane;
				v *= top;

				Vec3 dir = normalize(forward + right * u + up * v);
				Vec3 rayOrigin = eye;
				
				// Depth of Field: offset ray origin on aperture disk
				if (mDepthOfFieldEnabled)
				{
					Vec3 focalPoint = eye + dir * mFocalDistance;
					Vec3 apertureOffset = randomInUnitDisk() * mAperture;
					rayOrigin = eye + right * apertureOffset.x + up * apertureOffset.y;
					dir = normalize(focalPoint - rayOrigin);
				}
				
				// Motion Blur: random time sample
				GLfloat time = mMotionBlurEnabled ? timeDist(mRNG) * mShutterTime : 0.0f;
				
				col += traceRay(rayOrigin, dir, 0, time);
			}
			
			col = col * (1.0f / totalSamples);

			int idx = (j * width + i) * 3;
			framebuffer[idx + 0] = static_cast<unsigned char>(std::clamp(col.x, 0.0f, 1.0f) * 255.0f);
			framebuffer[idx + 1] = static_cast<unsigned char>(std::clamp(col.y, 0.0f, 1.0f) * 255.0f);
			framebuffer[idx + 2] = static_cast<unsigned char>(std::clamp(col.z, 0.0f, 1.0f) * 255.0f);
		}
	}
	std::cout << "Rendering complete!" << std::endl;
}
