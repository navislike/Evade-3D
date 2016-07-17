#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"


// Class for generating a unit cube
class CDodecahedron
{
public:
	CDodecahedron();
	~CDodecahedron();
	void Create();
	void Render();
	void Release();
	float getDisplacement();
	float getCentreToVertexDist();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	int location;
	float displacement;
	int totalVertices;
	const int sidesPerFace = 5;
	const int numOfVertices = 60;
	const float interiorAngle = 108.0f;
	const float exterior180Angle = 72.0f;
	const float pentagonSide = 2.0f;
	const float adjacentAngle = 116.565f;
	const float centreToVertexDist = sineRule(pentagonSide, 72, 54);

	float convertToRadians(float degrees);
	float sineRule(float side, float divAngle, float multAngle);
};