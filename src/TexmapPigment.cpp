#include "../include/TexmapPigment.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

std::ostream &operator<<(std::ostream &out, const TexmapPigment &tp)
{
	Vec4 p0 = tp.getP0();
	Vec4 p1 = tp.getP1();
	out << "TexmapPigment: " << "file(\"" << tp.getFilename() << "\")\n"
		<< "  P0(" << p0.x << ", " << p0.y << ", " << p0.z << ", " << p0.w << ")\n"
		<< "  P1(" << p1.x << ", " << p1.y << ", " << p1.z << ", " << p1.w << ")";
	return out;
}

TexmapPigment::TexmapPigment(const std::string &file, const Vec4 &p0, const Vec4 &p1, const unsigned int id)
	: Pigment(Pigment::TEXMAP), filename(file), P0(p0), P1(p1), textureID(id) { loadTexture(); }

Vec3 TexmapPigment::getColor(const Vec4 &point) const
{
	// If texture data is not loaded, return magenta
	if (texData.empty() || texWidth == 0 || texHeight == 0)
		return Vec3(1.0f, 0.0f, 1.0f);

	// Map the point coordinates into [0, 1] range based on P0 and P1
	GLfloat denomX = P1.x - P0.x;
	GLfloat denomY = P1.y - P0.y;

	GLfloat u = 0.0f;
	GLfloat v = 0.0f;

	if (std::fabs(denomX) > 1e-6f)
		u = (point.x - P0.x) / denomX;
	if (std::fabs(denomY) > 1e-6f)
		v = (point.y - P0.y) / denomY;

	// Clamp u and v to [0, 1]
	u = std::clamp(u, 0.0f, 1.0f);
	v = std::clamp(v, 0.0f, 1.0f);

	// Convert u,v to texture pixel coordinates
	int ix = static_cast<int>(u * (texWidth - 1));
	int iy = static_cast<int>((1.0f - v) * (texHeight - 1));

	ix = std::clamp(ix, 0, texWidth - 1);
	iy = std::clamp(iy, 0, texHeight - 1);

	int index = (iy * texWidth + ix) * texChannels;
	
	// Ensure we don't read beyond array bounds
	if (index + texChannels > (int)texData.size())
		return Vec3(1.0f, 0.0f, 1.0f); // magenta for corrupted data

	unsigned char r = 0, g = 0, b = 0;
	
	// Handle different channel counts
	if (texChannels == 1) {
		// Grayscale
		r = g = b = texData[index];
	} else if (texChannels >= 3) {
		// RGB or RGBA
		r = texData[index + 0];
		g = texData[index + 1];
		b = texData[index + 2];
	}

	// Convert to floats 0..1
	return Vec3(r, g, b) / 255.0f;
}

Vec3 TexmapPigment::getColorOnSphere(const Vec4 &point, const Vec3 &center) const
{
	// If texture data is not loaded, return magenta
	if (texData.empty() || texWidth == 0 || texHeight == 0)
		return Vec3(1.0f, 0.0f, 1.0f); // magenta for missing texture

	// Convert point to local sphere coordinates
	Vec3 pLocal = Vec3(point.x - center.x, point.y - center.y, point.z - center.z);
	pLocal = normalize(pLocal);

	// Spherical coordinates
	GLfloat theta = std::acos(pLocal.y);		  // polar angle
	GLfloat phi = std::atan2(pLocal.z, pLocal.x); // azimuthal angle

	GLfloat u = (phi + PI) / (2.0f * PI); // [0,1]
	GLfloat v = theta / PI;				  // [0,1]

	// Convert u,v to texture pixel coordinates
	int ix = static_cast<int>(u * (texWidth - 1));
	int iy = static_cast<int>((1.0f - v) * (texHeight - 1));

	ix = std::clamp(ix, 0, texWidth - 1);
	iy = std::clamp(iy, 0, texHeight - 1);

	int index = (iy * texWidth + ix) * texChannels;
	unsigned char r = 255, g = 0, b = 255; // default magenta for missing channels
	if (index + 0 < (int)texData.size())
		r = texData[index + 0];
	if (texChannels >= 3 && index + 1 < (int)texData.size())
		g = texData[index + 1];
	if (texChannels >= 3 && index + 2 < (int)texData.size())
		b = texData[index + 2];

	// Convert to floats 0..1
	return Vec3(r, g, b) / 255.0f;
}

void TexmapPigment::loadTexture()
{
	if (filename.empty())
	{
		std::cerr << "TexmapPigment: filename empty, skipping texture load\n";
		return;
	}

	// Try to load file relative to executable working directory
	int w = 0, h = 0, channels = 0;
	unsigned char *data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
	std::string tried = filename;
	if (!data)
	{
		// try data/textures/ prefix
		std::string path = std::string("data/textures/") + filename;
		data = stbi_load(path.c_str(), &w, &h, &channels, 0);
		if (data)
			tried = path;
	}

	if (!data)
	{
		std::cerr << "TexmapPigment: failed to load image '" << filename << "' (tried " << tried << ")\n";
		texWidth = texHeight = texChannels = 0;
		texData.clear();
		textureID = 0;
		return;
	}
	texWidth = w;
	texHeight = h;
	texChannels = channels;
	texData.assign(data, data + (w * h * channels));

	// Create GL texture (optional, but useful for debugging with GL)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLenum fmt = (texChannels == 4 ? GL_RGBA : GL_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, texWidth, texHeight, 0, fmt, GL_UNSIGNED_BYTE, texData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Print debug info
	std::cout << "TexmapPigment: loaded '" << tried << "' (" << texWidth << "x" << texHeight << ", ch=" << texChannels << ")\n";
	// print first few pixels
	int count = std::min(8, texWidth * texHeight);
	std::cout << " First " << count << " texels (r,g,b):";
	for (int i = 0; i < count; ++i)
	{
		int idx = i * texChannels;
		int r = (idx + 0 < (int)texData.size()) ? texData[idx + 0] : 0;
		int g = (texChannels >= 3 && idx + 1 < (int)texData.size()) ? texData[idx + 1] : 0;
		int b = (texChannels >= 3 && idx + 2 < (int)texData.size()) ? texData[idx + 2] : 0;
		std::cout << " (" << r << "," << g << "," << b << ")";
	}
	std::cout << std::endl;

	stbi_image_free(data);
}
