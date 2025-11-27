#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>

#include "GL/glut.h"
#include "vecFunctions.h"
#include "Pigment.h"

class TexmapPigment : public Pigment
{
public:
	TexmapPigment(const std::string &file, const Vec4 &p0, const Vec4 &p1, const unsigned int id);

	// Setters
	void setP0(const Vec4 &p0) { P0 = p0; }
	void setP1(const Vec4 &p1) { P1 = p1; }

	// Getters
	std::string getFilename() const { return filename; }
	Vec4 getP0() const { return P0; }
	Vec4 getP1() const { return P1; }
	unsigned int getTextureID() const { return textureID; }

	// Returns the color from the texture at the given mapping point
	Vec3 getColor(const Vec4 &point) const override;
	Vec3 getColorOnSphere(const Vec4 &point, const Vec3 &center) const;

	friend std::ostream &operator<<(std::ostream &out, const TexmapPigment &tp);

private:
	std::string filename; // Texture file name
	Vec4 P0;			  // Mapping point 0
	Vec4 P1;			  // Mapping point 1

	// Texture data
	int texWidth = 0;
	int texHeight = 0;
	int texChannels = 0;
	unsigned int textureID = 0;
	std::vector<unsigned char> texData;

	// Loads the texture from file
	void loadTexture();
};
