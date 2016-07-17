#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"


class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();
	void CreateOffsetMidPoints();
	void RenderOffsetMidPoints();
	void ApplyOffset(const vector<glm::vec3> &offsetPoints, GLuint &curve);

	void CreateTrack();
	void RenderTrack();

	float GetPathWidth();

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = glm::vec3(0, 0, 0)); // Return a point on the centreline based on a certain distance along the control curve.
	vector<glm::vec3> getCentrePointVector();
	vector<glm::vec3> getCentreLinePointVector();
	vector<glm::vec3> getLeftMidVector();
	vector<glm::vec3> getRightMidVector();

private:

	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);


	vector<float> m_distances;
	CTexture m_texture;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoMidLeftOffsetCurve;
	GLuint m_vaoMidRightOffsetCurve;
	GLuint m_vaoTrack;

	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors
	vector<glm::vec3> m_leftMidPoints;
	vector<glm::vec3> m_rightMidPoints;

	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points

	vector<glm::vec3> m_tangentPoints;

	float pathWidth;
	bool isLeft;
	bool isRight;

	unsigned int m_vertexCount;				// Number of vertices in the track VBO
};
