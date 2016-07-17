#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"


// Class for generating a unit cube
class CDoublePyramid
{
public:
	CDoublePyramid();
	~CDoublePyramid();
	void Create();
	void Render();
	void Release();
	float getDisplacement();

private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	int location;
	float displacement;
	const float pyramidHeight = 1.0f;
	const int verticesPerFace = 3;
	const int verticesNum = 24;
};