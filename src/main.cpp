#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "../include/GL/glut.h"
#include "../include/Camera.h"
#include "../include/inputFunctions.h"
#include "../include/Light.h"
#include "../include/Pigment.h"
#include "../include/CheckerPigment.h"
#include "../include/SolidPigment.h"
#include "../include/TexmapPigment.h"
#include "../include/SurfaceFinish.h"
#include "../include/Object.h"
#include "../include/Sphere.h"
#include "../include/Polyhedron.h"
#include "../include/Raytracer.h"
#include "../include/vecFunctions.h"
#include "../include/glut_callbacks.h"

// Print the scene components to the console
static void print(const Camera &camera,
				  const std::vector<Light> &lights,
				  const std::vector<std::unique_ptr<Pigment>> &pigments,
				  const std::vector<std::unique_ptr<SurfaceFinish>> &finishes,
				  const std::vector<std::unique_ptr<Object>> &surfaces)
{
	// Print camera
	std::cout << camera << "\n";

	// Print lights
	for (const auto &light : lights)
		std::cout << light << "\n";

	// Print pigments
	for (const auto &pigment : pigments)
	{
		if (pigment->type == Pigment::SOLID)
			std::cout << *(static_cast<SolidPigment *>(pigment.get())) << "\n";
		else if (pigment->type == Pigment::CHECKER)
			std::cout << *(static_cast<CheckerPigment *>(pigment.get())) << "\n";
		else if (pigment->type == Pigment::TEXMAP)
			std::cout << *(static_cast<TexmapPigment *>(pigment.get())) << "\n";
	}

	// Print finishes
	for (const auto &finish : finishes)
		std::cout << *finish << "\n";

	// Print surfaces
	for (const auto &surface : surfaces)
	{
		if (surface->getType() == Object::Sphere)
			std::cout << *(static_cast<Sphere *>(surface.get())) << "\n";
		else if (surface->getType() == Object::Polyhedron)
			std::cout << *(static_cast<Polyhedron *>(surface.get())) << "\n";
	}
}

// Parse command-line arguments
static void argsParse(int argc, char *argv[],
					  std::string &inputFilename,
					  std::string &outputFilename,
					  int &windowWidth,
					  int &windowHeight)
{
	// Command-line args: inputFile outputFile [width height]
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <input-file> <output-file> [width] [height]" << std::endl;
		exit(1);
	}

	// Parse command-line arguments
	inputFilename = argv[1];

	if (argc >= 3)
		outputFilename = argv[2];

	else
	{
		// Remove .txt extension and add .ppm
		size_t dotPos = inputFilename.find_last_of('.');
		if (dotPos != std::string::npos && inputFilename.substr(dotPos) == ".txt")
			outputFilename = inputFilename.substr(0, dotPos) + ".ppm";
		else
			outputFilename = inputFilename + ".ppm";
	}

	if (argc >= 4)
	{
		try
		{
			windowWidth = std::stoi(argv[3]);
		}
		catch (...)
		{
			std::cerr << "Warning: invalid width; using default 800." << std::endl;
			windowWidth = 800;
		}
	}
	if (argc >= 5)
	{
		try
		{
			windowHeight = std::stoi(argv[4]);
		}
		catch (...)
		{
			std::cerr << "Warning: invalid height; using default 600." << std::endl;
			windowHeight = 600;
		}
	}
}

int main(int argc, char *argv[])
{
	// Parse command-line arguments
	int windowWidth = 800;
	int windowHeight = 600;
	std::string inputFilename;
	std::string outputFilename;
	argsParse(argc, argv, inputFilename, outputFilename, windowWidth, windowHeight);

	// Glut initialization
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("TP2 - Raytracing");

	// Read scene from input file
	Camera camera;
	std::vector<Light> lights;
	std::vector<std::unique_ptr<Pigment>> pigments;
	std::vector<std::unique_ptr<SurfaceFinish>> finishes;
	std::vector<std::unique_ptr<Object>> surfaces;
	readInputs(inputFilename, camera, lights, pigments, finishes, surfaces);
	print(camera, lights, pigments, finishes, surfaces);

	// Set clear color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Register objects for rendering
	registerObjects(&camera, &surfaces, &lights);

	// Setup framebuffer dimensions
	sImageWidth = windowWidth;
	sImageHeight = windowHeight;

	// Set output filename for saving after first render
	setOutputFilename(outputFilename);

	// Register glut callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	// Light model
	for (const auto &light : lights)
		light.applyLight();

	glutMainLoop();
	return 0;
}
