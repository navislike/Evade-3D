#include "Heart.h"
#include "OpenAssetImportMesh.h"

CHeart::CHeart()
{
	m_pHeart = NULL;
}
CHeart::~CHeart()
{
	delete m_pHeart;
}

void CHeart::Initialise(){
	m_pHeart = new COpenAssetImportMesh;
	m_pHeart->Load("resources\\models\\Heart\\High Poly Piece of Heart.obj");
}