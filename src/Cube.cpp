#include "Cube.h"

CCube::CCube()
{}
CCube::~CCube()
{
	Release();
}
void CCube::Create()
{
	m_texture.Load("resources\\textures\\crescent.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();

	glm::vec3 cube[24]{
		//front face
		glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(-1, 1, 1),
			glm::vec3(1, 1, 1),
			//right face
			glm::vec3(1, -1, 1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, 1),
			glm::vec3(1, 1, -1),
			//back face
			glm::vec3(1, -1, -1),
			glm::vec3(-1, -1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),

			//left face
			glm::vec3(-1, -1, -1),
			glm::vec3(-1, -1, 1),
			glm::vec3(-1, 1, -1),
			glm::vec3(-1, 1, 1),
			//top face
			glm::vec3(-1, 1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, -1),
			glm::vec3(1, 1, -1),
			//bottom face
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
	};

	glm::vec2 textCoords[4]{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 normal[24]{
			//front face normals
			glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(-1, 1, -1),
			glm::vec3(1, 1, -1),
			//right face normals
			glm::vec3(1, -1, 1),
			glm::vec3(1, -1, -1),
			glm::vec3(1, 1, 1),
			glm::vec3(1, 1, -1),

			//back face normals
			glm::vec3(1, -1, 1),
			glm::vec3(-1, -1, 1),
			glm::vec3(1, 1, 1),
			glm::vec3(-1, 1, 1),
			//left face normals
			glm::vec3(-1, -1, 1),
			glm::vec3(-1, -1, -1),
			glm::vec3(-1, 1, 1),
			glm::vec3(-1, 1, -1),
			//top face normals
			glm::vec3(-1, 1, -1),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, 1),
			glm::vec3(1, 1, 1),
			//bottom face normals
			glm::vec3(-1, -1, -1),
			glm::vec3(1, -1, -1),
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
	};

	// Write the code to add interleaved vertex attributes to the VBO
	for (int i = 0; i < 24; i++) {
		m_vbo.AddData(&cube[i], sizeof(glm::vec3));
		m_vbo.AddData(&textCoords[i % 4], sizeof(glm::vec2));
		m_vbo.AddData(&normal[i / 4], sizeof(glm::vec3));
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
void CCube::Render()
{
	glBindVertexArray(m_vao);
	// Call glDrawArrays to render each side

	m_texture.Bind();
	for (int i = 0; i < 6; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
}
void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

float CCube::getDisplacement(){
	return displacement;
}