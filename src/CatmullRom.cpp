#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	pathWidth = 200.0f;
	isLeft = true;
	isRight = false;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;
}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	
	const float constY = 0.1f;
	const int coordMultiplyer = 150;
	const float scale = coordMultiplyer / 10;
	float x;
	float z;
	float yVariant = 10.0f;

	for (float i = -6.05f; i <= 6.05f; i += 0.05f){
		x = (i * M_PI);
		z = 5 * cos(0.5*x);
		
		m_controlPoints.push_back(glm::vec3(x*coordMultiplyer, constY, z*coordMultiplyer));
	}

	glm::vec3 temp = m_controlPoints.back();

	m_controlPoints.push_back(glm::vec3(temp.x + 2.5f, constY*scale, temp.z));
	m_controlPoints.push_back(glm::vec3(250.0f*scale, constY*scale, temp.z));
	m_controlPoints.push_back(glm::vec3(250.0f*scale, constY*scale, -100.0f*scale));
	m_controlPoints.push_back(glm::vec3(250.0f*scale, constY*scale, -150.0f*scale));
	m_controlPoints.push_back(glm::vec3(112.5f*scale, constY*scale, -175.0*scale));
	m_controlPoints.push_back(glm::vec3(0 * scale, constY*scale, -200.0f*scale));
	m_controlPoints.push_back(glm::vec3(-112.5f*scale, constY*scale, -175.0f*scale));
	m_controlPoints.push_back(glm::vec3(-250.0f*scale, constY*scale, -150.0f*scale));
	m_controlPoints.push_back(glm::vec3(-250.0f*scale, constY*scale, -100.0f*scale));
	m_controlPoints.push_back(glm::vec3(-250.0*scale, constY*scale, temp.z));
	m_controlPoints.push_back(glm::vec3(-temp.x - 2.5f, constY*scale, temp.z));
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}

// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

		// Call PointAt to sample the spline, to generate the points
		for (int i = 0; i < numSamples; i++) {
			Sample(i * fSpacing, p, up);
			m_centrelinePoints.push_back(p);
			if (m_controlUpVectors.size() > 0)
				m_centrelineUpVectors.push_back(up);
		}
	


		// Repeat once more for truly equidistant points
		m_controlPoints = m_centrelinePoints;
		m_controlUpVectors = m_centrelineUpVectors;
		m_centrelinePoints.clear();
		m_centrelineUpVectors.clear();
		m_distances.clear();
		ComputeLengthsAlongControlPoints();
		fTotalLength = m_distances[m_distances.size() - 1];
		fSpacing = fTotalLength / numSamples;
		for (int i = 0; i < numSamples; i++) {
			Sample(i * fSpacing, p, up);
			m_centrelinePoints.push_back(p);
			if (m_controlUpVectors.size() > 0)
				m_centrelineUpVectors.push_back(up);
		}
}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(1000);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	// Create a VBO
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
	
		glm::vec3 v = m_centrelinePoints.at(i);
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	vbo.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

}

float CCatmullRom::GetPathWidth(){
	return pathWidth;
}

void CCatmullRom::CreateOffsetCurves()
{
	glm::vec3 l = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pNext = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_centrelinePoints.size(); i++){
		p = m_centrelinePoints.at(i);
		int pIndex = (i + 1) % m_centrelinePoints.size();

		pNext = m_centrelinePoints.at(pIndex);

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
		glm::vec3 B = glm::normalize(glm::cross(N, T));

		l = p - (pathWidth / 2)*N;
		r = p + (pathWidth / 2)*N;

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}

	ApplyOffset(m_leftOffsetPoints, m_vaoLeftOffsetCurve);
	ApplyOffset(m_rightOffsetPoints, m_vaoRightOffsetCurve);
}

void CCatmullRom::CreateOffsetMidPoints(){
	glm::vec3 l = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_centrelinePoints.size(); i++){

		l.x = (m_leftOffsetPoints.at(i).x + m_centrelinePoints.at(i).x)/2;
		l.y = (m_leftOffsetPoints.at(i).y + m_centrelinePoints.at(i).y) / 2;
		l.z = (m_leftOffsetPoints.at(i).z + m_centrelinePoints.at(i).z) / 2;
		r.x = (m_rightOffsetPoints.at(i).x + m_centrelinePoints.at(i).x) / 2;
		r.y = (m_leftOffsetPoints.at(i).y + m_centrelinePoints.at(i).y) / 2;
		r.z = (m_rightOffsetPoints.at(i).z + m_centrelinePoints.at(i).z) / 2;
		
		m_leftMidPoints.push_back(l);
		m_rightMidPoints.push_back(r);
	}

	ApplyOffset(m_leftMidPoints, m_vaoMidLeftOffsetCurve);
	ApplyOffset(m_rightMidPoints, m_vaoMidRightOffsetCurve);
}

void CCatmullRom::ApplyOffset(const vector<glm::vec3> &offsetPoints, GLuint &curve){
	glGenVertexArrays(1, &curve);
	glBindVertexArray(curve);
	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < offsetPoints.size(); i++) {
		glm::vec3 v = offsetPoints.at(i);
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}
	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}


void CCatmullRom::CreateTrack()
{
	m_texture.Load("resources\\textures\\road.jpg");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	CVertexBufferObject vbo;
	vbo.Bind();
	vbo.Create();
	glm::vec2 textCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	int i = 0;
	for (; i < m_centrelinePoints.size(); i++){
		glm::vec3 vL = glm::vec3(m_leftOffsetPoints.at(i));
		vbo.AddData(&vL, sizeof(glm::vec3));
		textCoord = glm::vec2(0, i);
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

		glm::vec3 vR = glm::vec3(m_rightOffsetPoints.at(i));
		vbo.AddData(&vR, sizeof(glm::vec3));
		textCoord = glm::vec2(1, i);
		vbo.AddData(&textCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.AddData(&m_leftOffsetPoints.at(0), sizeof(glm::vec3));
	textCoord = glm::vec2(0, i);
	vbo.AddData(&textCoord, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));

	vbo.AddData(&m_rightOffsetPoints.at(1), sizeof(glm::vec3));
	textCoord = glm::vec2(1, i);
	vbo.AddData(&textCoord, sizeof(glm::vec2));
	vbo.AddData(&normal, sizeof(glm::vec3));

	

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	//glDrawArrays(GL_LINE_STRIP, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	//glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());
	//glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	//glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
	//glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
}

void CCatmullRom::RenderOffsetMidPoints(){
	glBindVertexArray(m_vaoMidLeftOffsetCurve);
	//glDrawArrays(GL_POINTS, 0, m_leftMidPoints.size());
	//glDrawArrays(GL_LINE_STRIP, 0, m_leftMidPoints.size());

	glBindVertexArray(m_vaoMidRightOffsetCurve);
	//glDrawArrays(GL_POINTS, 0, m_rightMidPoints.size());
	//glDrawArrays(GL_LINE_STRIP, 0, m_rightMidPoints.size());
}

void CCatmullRom::RenderTrack()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_leftOffsetPoints.size() + m_rightOffsetPoints.size() + 2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int CCatmullRom::CurrentLap(float d)
{
	return (int)(d / m_distances.back());
}

vector<glm::vec3> CCatmullRom::getCentrePointVector(){
	return m_controlPoints;
}

vector<glm::vec3> CCatmullRom::getCentreLinePointVector(){
	return m_centrelinePoints;
}

vector<glm::vec3> CCatmullRom::getLeftMidVector(){
	return m_leftMidPoints;
}
vector<glm::vec3> CCatmullRom::getRightMidVector(){
	return m_rightMidPoints;
}