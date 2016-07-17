#include "Dodecahedron.h"

CDodecahedron::CDodecahedron()
{
	totalVertices = 60;
}
CDodecahedron::~CDodecahedron()
{
	Release();
}
void CDodecahedron::Create()
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
	
	glm::vec3 midCalc1 = glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
		sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist);
	glm::vec3 midCalc2 = glm::vec3(0, sineRule(centreToVertexDist * 2, 90, 63.435f), -centreToVertexDist - sineRule(centreToVertexDist * 2, 90, 26.565f));
	glm::vec3 topMidPoint = midCalc2 - midCalc1;

	glm::vec3 dodecahedron[30]{
		
		// bottom face
		glm::vec3(0, 0, centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3((pentagonSide / 2), 0, -centreToVertexDist),
		glm::vec3(-(pentagonSide / 2), 0, -centreToVertexDist),
		
		// back face
		glm::vec3(-(pentagonSide / 2), 0, -centreToVertexDist),
		glm::vec3((pentagonSide / 2), 0, -centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f), 
				sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f), 
				sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(0, sineRule(centreToVertexDist * 2, 90, 63.435f), -centreToVertexDist - sineRule(centreToVertexDist * 2, 90, 26.565f)),
		

		// bottom right back face
		glm::vec3((pentagonSide / 2), 0, -centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2) - topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),

		// bottom left back face
		glm::vec3((-pentagonSide / 2), 0, -centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2) + topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),

		// bottom right front face
		glm::vec3(0, 0, centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3(0, sineRule(pentagonSide, 90, 63.435f), centreToVertexDist + sineRule(pentagonSide, 90, 26.565f)),
		glm::vec3(sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3((sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2))/2, sineRule(centreToVertexDist * 2, 90, 63.435f), 
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18))/2),

		// bottom left front face
		glm::vec3(0, 0, centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 0, centreToVertexDist - sineRule(pentagonSide, 90, 36)),
		glm::vec3(0, sineRule(pentagonSide, 90, 63.435f), centreToVertexDist + sineRule(pentagonSide, 90, 26.565f)),
		glm::vec3(sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3((sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2)) / 2, sineRule(centreToVertexDist * 2, 90, 63.435f),
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)) / 2),
		/*
		// front face
		glm::vec3(0, sineRule(pentagonSide, 90, 63.435f), centreToVertexDist + sineRule(pentagonSide, 90, 26.565f)),
		glm::vec3((sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2)) / 2, sineRule(centreToVertexDist * 2, 90, 63.435f),
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)) / 2),
		glm::vec3((sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2)) / 2, sineRule(centreToVertexDist * 2, 90, 63.435f),
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)) / 2),
		glm::vec3(-(pentagonSide / 2), 4.3f, centreToVertexDist),
		glm::vec3((pentagonSide / 2), 4.3f, centreToVertexDist),
		
		// top left front face
		glm::vec3(sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3((sineRule(-pentagonSide, 90, 26.565f) - sineRule(pentagonSide, 90, 18) - (pentagonSide / 2)) / 2, sineRule(centreToVertexDist * 2, 90, 63.435f),
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)) / 2),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2) + topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),
		glm::vec3(-(pentagonSide / 2), 4.3f, centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		
		// top right front face
		glm::vec3(sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)),
		glm::vec3((sineRule(pentagonSide, 90, 26.565f) + sineRule(pentagonSide, 90, 18) + (pentagonSide / 2)) / 2, sineRule(centreToVertexDist * 2, 90, 63.435f),
			(centreToVertexDist + sineRule(pentagonSide, 90, 26.565f) + centreToVertexDist - sineRule(pentagonSide, 90, 36) + sineRule(pentagonSide, 90, 18)) / 2),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2) - topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),
		glm::vec3((pentagonSide / 2), 4.3f, centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) - (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		
		// top left back face
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2) + topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),
		glm::vec3(0, sineRule(centreToVertexDist * 2, 90, 63.435f), -centreToVertexDist - sineRule(centreToVertexDist * 2, 90, 26.565f)),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		glm::vec3(0, 4.3f, -centreToVertexDist),

		// top right back face
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), sineRule(pentagonSide, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2) - topMidPoint.z, sineRule(centreToVertexDist * 2, 90, 63.435f),
			sineRule(-pentagonSide, 90, 26.565f) - centreToVertexDist - topMidPoint.x),
		glm::vec3(0, sineRule(centreToVertexDist * 2, 90, 63.435f), -centreToVertexDist - sineRule(centreToVertexDist * 2, 90, 26.565f)),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		glm::vec3(0, 4.3f, -centreToVertexDist),


		// top face
		glm::vec3(0, 4.3f, -centreToVertexDist),
		glm::vec3(sineRule(pentagonSide, 90, 18) + (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		glm::vec3(sineRule(-pentagonSide, 90, 18) - (pentagonSide / 2), 4.3f, -centreToVertexDist + sineRule(pentagonSide, 90, 36)),
		glm::vec3((pentagonSide / 2), 4.3f, centreToVertexDist),
		glm::vec3(-(pentagonSide / 2), 4.3f, centreToVertexDist),
		*/
	};

	glm::vec2 textCoords[4]{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
	};

	glm::vec3 normal[30]{
			//bottom facing normals
			glm::vec3(1, 1, 1),
	};

	// Write the code to add interleaved vertex attributes to the VBO
	for (int i = 0; i < 30; i++) {
		m_vbo.AddData(&dodecahedron[i], sizeof(glm::vec3));
		m_vbo.AddData(&textCoords[i % sidesPerFace], sizeof(glm::vec2));
		m_vbo.AddData(&normal[i / sidesPerFace], sizeof(glm::vec3));
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
void CDodecahedron::Render()
{
	glBindVertexArray(m_vao);
	// Call glDrawArrays to render each side

	m_texture.Bind();
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
	
	for (int i = 0; i < numOfVertices / sidesPerFace; i++){
		glDrawArrays(GL_TRIANGLE_STRIP, i*sidesPerFace, sidesPerFace);
	}
	
}
void CDodecahedron::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

float CDodecahedron::getDisplacement(){
	return displacement;
}

float CDodecahedron::getCentreToVertexDist(){
	return centreToVertexDist;
}

float CDodecahedron::convertToRadians(float degrees){
	return (degrees*M_PI) / 180;
}

float CDodecahedron::sineRule(float side, float divAngle, float multAngle){
	return (side / sin(convertToRadians(divAngle)))*sin(convertToRadians(multAngle));
}