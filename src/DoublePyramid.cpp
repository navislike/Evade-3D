#include "DoublePyramid.h"

CDoublePyramid::CDoublePyramid()
{}
CDoublePyramid::~CDoublePyramid()
{
	Release();
}
void CDoublePyramid::Create()
{
	m_texture.Load("resources\\textures\\blue.jpg");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();

	glm::vec3 pyramid[24]{
		// top front face
		glm::vec3(-1, 0, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(0, pyramidHeight, 0),

		// top left face
		glm::vec3(-1, 0, 1),
		glm::vec3(-1, 0, -1),
		glm::vec3(0, pyramidHeight, 0),

		// top back face
		glm::vec3(-1, 0, -1),
		glm::vec3(1, 0, -1),
		glm::vec3(0, pyramidHeight, 0),

		// top right face
		glm::vec3(1, 0, -1),
		glm::vec3(1, 0, 1),
		glm::vec3(0, pyramidHeight, 0),

		// bottom front face
		glm::vec3(-1, 0, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(0, -pyramidHeight, 0),

		// bottom left face
		glm::vec3(-1, 0, 1),
		glm::vec3(-1, 0, -1),
		glm::vec3(0, -pyramidHeight, 0),

		// bottom back face
		glm::vec3(-1, 0, -1),
		glm::vec3(1, 0, -1),
		glm::vec3(0, -pyramidHeight, 0),

		// bottom right face
		glm::vec3(1, 0, -1),
		glm::vec3(1, 0, 1),
		glm::vec3(0, -pyramidHeight, 0),
	};

	glm::vec2 textCoords[4]{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 normal[8]{
		// top front face
		glm::vec3(0, 1, 1),

		// top left face
		glm::vec3(-1, 1, 0),

		// top back face
		glm::vec3(0, 1, -1),

		// top right face
		glm::vec3(1, 1, 0),

		// bottom front face
		glm::vec3(0, -1, 1),

		// bottom left face
		glm::vec3(-1, -1, 0),

		// bottom back face
		glm::vec3(0, -1, -1),

		// bottom right face
		glm::vec3(1, -1, 0),
		
	};

	// Write the code to add interleaved vertex attributes to the VBO
	for (int i = 0; i < verticesNum; i++) {
		m_vbo.AddData(&pyramid[i], sizeof(glm::vec3));
		m_vbo.AddData(&textCoords[i % verticesPerFace], sizeof(glm::vec2));
		m_vbo.AddData(&normal[i % verticesPerFace], sizeof(glm::vec3));
	}

	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}
void CDoublePyramid::Render()
{
	glBindVertexArray(m_vao);
	// Call glDrawArrays to render each side

	m_texture.Bind();
	for (int i = 0; i < verticesNum/verticesPerFace; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, i * verticesPerFace, verticesPerFace);
	}
}
void CDoublePyramid::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

float CDoublePyramid::getDisplacement(){
	return displacement;
}