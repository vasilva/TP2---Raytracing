#include "../include/Polyhedron.h"
#include <algorithm>
#include <cmath>

Polyhedron::Polyhedron(Pigment *p, SurfaceFinish *sf, const size_t f)
	: Object(Object::Polyhedron, p, sf), faces(f) {}

void Polyhedron::addPlane(const Vec4 &plane)
{
	if (planes.size() < faces)
		planes.push_back(plane);
}

std::ostream &operator<<(std::ostream &out, const Polyhedron &poly)
{
	out << "Polyhedron:" << std::endl;
	out << "  Faces: " << poly.faces << std::endl;
	out << "  Planes:" << std::endl;
	for (size_t i = 0; i < poly.planes.size(); ++i)
		out << "    Plane " << i + 1 << ": " << poly.planes[i] << std::endl;
	return out;
}

void Polyhedron::draw() const
{
	auto pigment = this->getPigment();
	if (!pigment)
		return;

	// Desenhar cada face do poliedro
	// Para cada face, encontramos seus vértices e desenhamos como polígono
	
	for (size_t i = 0; i < planes.size(); ++i)
	{
		const Vec4& plane = planes[i];
		Vec3 normal(plane.x, plane.y, plane.z);
		normal = normalize(normal);
		
		// Encontrar vértices da face (interseções com outras faces)
		std::vector<Vec3> vertices;
		
		// Para cada par de outras faces, verificar se a interseção dos 3 planos
		// resulta em um vértice válido (dentro do poliedro)
		for (size_t j = 0; j < planes.size(); ++j)
		{
			if (j == i) continue;
			
			for (size_t k = j + 1; k < planes.size(); ++k)
			{
				if (k == i) continue;
				
				Vec3 vertex = intersectThreePlanes(plane, planes[j], planes[k]);
				
				// Verificar se a interseção é válida (determinante não era zero)
				if (vertex.x > 1e9f || vertex.y > 1e9f || vertex.z > 1e9f)
					continue; // Planos paralelos ou colineares
				
				// Verificar se o vértice está dentro do poliedro
				// (satisfaz todas as inequações dos planos)
				bool valid = true;
				for (size_t m = 0; m < planes.size(); ++m)
				{
					const Vec4& p = planes[m];
					float dist = p.x * vertex.x + p.y * vertex.y + p.z * vertex.z + p.w;
					if (dist > 0.1f) // Margem de erro aumentada
					{
						valid = false;
						break;
					}
				}
				
				if (valid)
				{
					// Verificar se não é duplicado
					bool duplicate = false;
					for (const auto& v : vertices)
					{
						float dist = length(v - vertex);
						if (dist < 0.1f) // Margem para duplicados (ajustada)
						{
							duplicate = true;
							break;
						}
					}
					
					if (!duplicate)
						vertices.push_back(vertex);
				}
			}
		}
		
		// Se encontramos vértices suficientes, desenhar a face
		if (vertices.size() >= 3)
		{
			// Ordenar vértices em ordem circular ao redor do centro da face
			Vec3 center(0, 0, 0);
			for (const auto& v : vertices)
				center = center + v;
			center = center * (1.0f / vertices.size());
			
			// Criar sistema de coordenadas no plano
			Vec3 tangent;
			if (fabs(normal.x) < 0.9f)
				tangent = normalize(cross(normal, Vec3(1, 0, 0)));
			else
				tangent = normalize(cross(normal, Vec3(0, 1, 0)));
			Vec3 bitangent = cross(normal, tangent);
			
			// Ordenar vértices por ângulo
			std::vector<std::pair<float, Vec3>> sortedVertices;
			for (const auto& v : vertices)
			{
				Vec3 toVertex = v - center;
				float angle = atan2f(dot(toVertex, bitangent), dot(toVertex, tangent));
				sortedVertices.push_back({angle, v});
			}
			
			std::sort(sortedVertices.begin(), sortedVertices.end(),
				[](const auto& a, const auto& b) { return a.first < b.first; });
			
			// Desenhar a face
			Vec4 samplePoint(center.x, center.y, center.z, 1.0f);
			Vec3 color = pigment->getColor(samplePoint);
			
			glBegin(GL_POLYGON);
			glColor3f(color.x, color.y, color.z);
			glNormal3f(normal.x, normal.y, normal.z);
			for (const auto& pair : sortedVertices)
			{
				const Vec3& v = pair.second;
				glVertex3f(v.x, v.y, v.z);
			}
			glEnd();
		}
	}
}

Vec3 Polyhedron::intersectThreePlanes(const Vec4& p1, const Vec4& p2, const Vec4& p3) const
{
	// Resolver sistema linear 3x3 para encontrar ponto de interseção
	// p1.x * x + p1.y * y + p1.z * z + p1.w = 0
	// p2.x * x + p2.y * y + p2.z * z + p2.w = 0
	// p3.x * x + p3.y * y + p3.z * z + p3.w = 0
	
	// Usando regra de Cramer
	Vec3 n1(p1.x, p1.y, p1.z);
	Vec3 n2(p2.x, p2.y, p2.z);
	Vec3 n3(p3.x, p3.y, p3.z);
	
	Vec3 n2_cross_n3 = cross(n2, n3);
	float det = dot(n1, n2_cross_n3);
	
	if (fabs(det) < 1e-6f)
		return Vec3(1e10f, 1e10f, 1e10f); // Planos paralelos - valor sentinela
	
	Vec3 result = (cross(n2, n3) * -p1.w + cross(n3, n1) * -p2.w + cross(n1, n2) * -p3.w) * (1.0f / det);
	return result;
}
