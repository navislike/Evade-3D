#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class COpenAssetImportMesh;

// Class for generating a unit cube
class CHeart
{
public:
	CHeart();
	~CHeart();
	void Initialise();
	void Create();
	float getDisplacement();
	COpenAssetImportMesh *m_pHeart;

private:
	int location;
	float displacement;
};