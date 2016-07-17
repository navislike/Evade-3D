
#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Cube.h"
#include "DoublePyramid.h"
#include "DelayAudio.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pLives = NULL;
	m_pSpeedTimer = NULL;
	m_pLaps = NULL;
	m_pTimeScore = NULL;
	m_pFighterJet = NULL;
	m_pStadium = NULL;
	m_pDelay = NULL;
	m_pHighResolutionTimer = NULL;
	m_pBackgroundMusic = NULL;
	m_pGetHeart = NULL;
	m_pRockCollision = NULL;
	m_pFly = NULL;
	m_pSlowMo = NULL;
	m_pNextLap = NULL;
	m_pCatmullRom = NULL;
	m_pCube = NULL;
	m_pMoon = NULL;

	srand(time(NULL));

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	cameraSpeed = normalSpeed;
	rotation = 0.0f;
	m_JetPosition = glm::vec3(0, 0, 0);
	m_JetOrientation = glm::mat4();
	gameYPlane = 35.0f;
	cubeScale = 10.0f;
	lives = 3;
	isRestTimerOn = true;
	isShaking = false;
	boxNumbers = 60;
	heartNumbers = 5;
	pymNumbers = 7;
	blinkCount = 0;
	hitTimer = 0;
	zoomFactor = 125.0f;
	isCameraRetreating = false;
	T_Rot = -1.25f;
	rotSpeed = 50.0f;
	isLapTimerOn = true;
	lapTimer = 5.0f;
	camTimer = 0.25f;
	camJustChanged = false;
}

// Destructor
Game::~Game()
{
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pLives;
	delete m_pSpeedTimer;
	delete m_pLaps;
	delete m_pStadium;
	delete m_pGetHeart;
	delete m_pTimeScore;
	delete m_pRockCollision;
	delete m_pDelay;
	delete m_pSlowMo;
	delete m_pNextLap;
	delete m_pFly;
	delete m_pFighterJet;
	delete m_pBackgroundMusic;
	delete m_pCatmullRom;
	delete m_pCube;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise()
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pLives = new CFreeTypeFont;
	m_pSpeedTimer = new CFreeTypeFont;
	m_pLaps = new CFreeTypeFont;
	m_pDelay = new CFreeTypeFont;
	m_pGetHeart = new CAudio(false);
	m_pStadium = new COpenAssetImportMesh;
	m_pRockCollision = new CDelayAudio;
	m_pSlowMo = new CAudio(false);
	m_pTimeScore = new CFreeTypeFont;
	m_pFighterJet = new COpenAssetImportMesh;
	m_pBackgroundMusic = new CAudio(false);
	m_pFly = new CAudio(true, true);
	m_pNextLap = new CAudio(false);
	m_pCatmullRom = new CCatmullRom;
	m_pCube = new CCube;
	m_pMoon = new COpenAssetImportMesh;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);

	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "moon.jpg", 7500, 7500, 250.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	// Loads the fonts and sets the shaders for them. Font taken from: http://www.dafont.com/triforce.font
	m_pLives->LoadFont("resources\\fonts\\SherwoodRegular.ttf", 32);
	m_pLives->SetShaderProgram(pFontProgram);
	m_pSpeedTimer->LoadFont("resources\\fonts\\Triforce.ttf", 32);
	m_pSpeedTimer->SetShaderProgram(pFontProgram);
	m_pLaps->LoadFont("resources\\fonts\\Triforce.ttf", 32);
	m_pLaps->SetShaderProgram(pFontProgram);
	m_pTimeScore->LoadFont("resources\\fonts\\SherwoodRegular.ttf", 32);
	m_pTimeScore->SetShaderProgram(pFontProgram);
	m_pDelay->LoadFont("resources\\fonts\\SherwoodRegular.ttf", 32);
	m_pDelay->SetShaderProgram(pFontProgram);

	// Loads the jet, tower and moon meshes. Credit to Alec Pike, models taken from: http://www.models-resource.com/nintendo_64/legendofzeldaocarinaoftime/
	m_pFighterJet->Load("resources\\models\\Arwing\\Arwing.obj");
	m_pMoon->Load("resources\\models\\Moon\\model.obj");
	m_pStadium->Load("resources\\models\\Stadium\\bowser stadium 1.obj");

	//glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	//m_pHeartGlow->Initialise("resources\\Audio\\glowLoop.wav");
	m_pBackgroundMusic->Initialise("resources\\Audio\\dark.mp3");
	m_pGetHeart->Initialise("resources\\Audio\\getHeart.wav");
	m_pRockCollision->Initialise("resources\\Audio\\rockCollide.wav");
	m_pSlowMo->Initialise("resources\\Audio\\slowMo.wav");
	m_pFly->Initialise("resources\\Audio\\fly.wav");
	m_pFly->SetSoundAttributes(65);
	m_pNextLap->Initialise("resources\\Audio\\nextLap.wav");
	m_pFly->PlayEventSound(false);
	m_pBackgroundMusic->PlayEventSound(false, 0.3f);
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateOffsetMidPoints();
	m_pCatmullRom->CreateTrack();

	for (auto i = 0; i < heartNumbers; i++){
		COpenAssetImportMesh heart;
		CAudio sound(true, true);
		hearts.push_back(heart);
		heartResonaters.push_back(sound);
		DeterminePathSpawn(heartPositions, heartNextPositions);
		operativeHearts.push_back(true);
	}

	for (auto i = 0; i < hearts.size(); i++){
		hearts[i].Load("resources\\models\\Heart\\High Poly Piece of Heart.obj");
		heartResonaters[i].Initialise("resources\\Audio\\glowLoop.wav");
		heartResonaters[i].SetSoundAttributes(250);
		heartResonaters[i].PlayEventSound(false);
	}

	for (auto i = 0; i < boxNumbers; i++){
		CCube newCube;
		cubes.push_back(newCube);
		DeterminePathSpawn(cubePositions, cubeNextPositions);
		operativeCubes.push_back(true);
	}

	for (auto i = 0; i < pymNumbers; i++){
		CDoublePyramid newPym;
		dPym.push_back(newPym);
		DeterminePathSpawn(pymPositions, pymNextPositions);
		operativePyramids.push_back(true);
	}

	for (size_t i = 0; i < cubes.size(); i++){
		cubes.at(i).Create();
	}

	for (size_t i = 0; i < dPym.size(); i++){
		dPym[i].Create();
	}

	//glDisable(GL_CULL_FACE);
}

void Game::DeterminePathSpawn(vector<glm::vec3> &toVec, vector<glm::vec3> &nextVec){
	randomPath = rand() % 3;

	if (randomPath == LEFT){
		AssignSpawnPoint(toVec, m_pCatmullRom->getLeftMidVector(), nextVec);
	}
	else if (randomPath == CENTRE){
		AssignSpawnPoint(toVec, m_pCatmullRom->getCentreLinePointVector(), nextVec);
	}
	else{
		AssignSpawnPoint(toVec, m_pCatmullRom->getRightMidVector(), nextVec);
	}
}

void Game::AssignSpawnPoint(vector<glm::vec3> &toVec, vector<glm::vec3> &pathVec, vector<glm::vec3> &nextVec){
	do{
		randomIndex = (rand() % m_pCatmullRom->getCentreLinePointVector().size()) - skipSpawnFactor - 2;

		for (int i = 0; i < toVec.size(); i++){
			for (int j = 0; j < takenPositions.size(); j++){
				if (toVec[i] == takenPositions[j]){
					randomIndex = -1;
				}
			}
		}
	} while (randomIndex < 0);

	toVec.push_back(pathVec.at(randomIndex + skipSpawnFactor));
	nextVec.push_back(pathVec.at(randomIndex + skipSpawnFactor + 1));
}

void Game::UpdatePathSpawns(){

	for (int i = 0; i < heartResonaters.size(); i++){
		if (!operativeHearts[i])
		heartResonaters[i].Initialise("resources\\Audio\\glowLoop.wav");
		heartResonaters[i].SetSoundAttributes(250);
		heartResonaters[i].PlayEventSound(false);
	}

	cubePositions.clear();
	cubeNextPositions.clear();
	heartPositions.clear();
	heartNextPositions.clear();
	pymPositions.clear();
	pymNextPositions.clear();
	operativeCubes.clear();
	operativeHearts.clear();
	operativePyramids.clear();

	for (int i = 0; i < boxNumbers; i++){
		DeterminePathSpawn(cubePositions, cubeNextPositions);
		operativeCubes.push_back(true);
	}

	for (int i = 0; i < heartNumbers; i++){
		DeterminePathSpawn(heartPositions, heartNextPositions);
		operativeHearts.push_back(true);
	}

	for (int i = 0; i < pymNumbers; i++){
		DeterminePathSpawn(pymPositions, pymNextPositions);
		operativePyramids.push_back(true);
	}
}

// Render method runs repeatedly in a loop
void Game::Render()
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("CubeMapTex", 1);

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render();
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0, -1250, -1500.0f));
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	for (size_t i = 0; i < hearts.size(); i++){
		// Render the heart 
		if (operativeHearts[i]){
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(heartPositions[i].x, heartPositions[i].y + 10, heartPositions[i].z));
			modelViewMatrixStack.Scale(1.75f);
			modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), rotation);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			hearts[i].Render();
			modelViewMatrixStack.Pop();
		}
	}
	/*
	// Render the ghosts
	for (int i = 0; i < statues.size(); i++){
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(statuePositions[i]);
	modelViewMatrixStack.Scale(statueScales[i]);
	//modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), -90);
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), rotation);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	statues[i].Render();
	modelViewMatrixStack.Pop();
	}
	*/
	// Render the fighter jet 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_JetPosition);
	modelViewMatrixStack *= m_JetOrientation;
	//modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), -displacement/12);
	modelViewMatrixStack.Rotate(T, T_Rot);
	modelViewMatrixStack.Scale(0.175f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pFighterJet->Render();
	modelViewMatrixStack.Pop();

	for (size_t i = 0; i < dPym.size(); i++){
		// Render the pyramids

		if (operativePyramids[i]){
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(pymPositions[i].x, pymPositions[i].y + 10, pymPositions[i].z));
			modelViewMatrixStack.Scale(7.5f);
			modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), rotation);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			dPym[i].Render();
			modelViewMatrixStack.Pop();
		}
	}

	// Render the stadium
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0, -1150, -1500));
	modelViewMatrixStack.Scale(250.0f);
	//modelViewMatrixStack.Rotate(glm::vec3(0, 0, 0), 30);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pStadium->Render();
	modelViewMatrixStack.Pop();

	// Render the moon 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-150, -100, -1500));
	modelViewMatrixStack.Scale(2.5f);

	angleOfRotation = 180 * (atan2(m_JetPosition.x - -150,
		m_JetPosition.z - -1500) / M_PI);
	modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), angleOfRotation);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pMoon->Render();
	modelViewMatrixStack.Pop();

	// Render the cubes
	float cubeRot;
	for (size_t i = 0; i < cubes.size(); i++){
		if (operativeCubes[i]){
			cubeRot = 180 * (atan2(cubeNextPositions[i].x - cubePositions[i].x,
				cubeNextPositions[i].z - cubePositions[i].z) / M_PI);
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(cubePositions[i].x, cubePositions[i].y + 10, cubePositions[i].z);
			modelViewMatrixStack.Scale(cubeScale);
			modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), cubeRot);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			cubes[i].Render();
			modelViewMatrixStack.Pop();
		}
	}

	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCatmullRom->RenderCentreline();
	m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderOffsetMidPoints();
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();
	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());
}

void Game::CameraChecker(){

	if (isShaking){
		float finalRot;

		if ((int)hitTimer % 30 == 0){
			finalRot = -rotationCount;
		}
		else{
			finalRot = rotationCount;
		}

		hitTimer += 15.0 / FPS;
		m_pCamera->RotateViewPoint(finalRot, T);
		rotationCount += 0.025f;
	}

	if (hitTimer >= reAdjustTime){
		isShaking = false;
		rotationCount = 0;
	}


	if (camJustChanged){
		camTimer -= 1.0 / FPS;

		if (camTimer <= 0){
			camJustChanged = false;
			camTimer = 0.25f;
		}
	}

	if (GetKeyState(VK_TAB) & 0x80 && !camJustChanged){

		camJustChanged = true;

		if ((camPos != cameraPositions.size() - 1)){
			camPos++;
		}
		else{
			camPos = 0;
		}
	}
}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_currentDistance += m_dt * cameraSpeed;
	m_pCamera->Update(m_dt);

	glm::vec3 p;
	glm::vec3 pNext;
	glm::vec3 up;

	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 2.5f, pNext);

	T = glm::normalize(pNext - p);
	N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	B = glm::normalize(glm::cross(N, T));

	glm::vec3 x;

	if (GetKeyState(VK_LEFT) & 0x80){
		if (displacement >= -m_pCatmullRom->GetPathWidth() / 2)
			displacement -= dodgeSpeed;
	}

	if (GetKeyState(VK_RIGHT) & 0x80){
		if (displacement <= m_pCatmullRom->GetPathWidth() / 2)
			displacement += dodgeSpeed;
	}

	if (GetKeyState('1') & 0x80){
		m_pRockCollision->IncreaseDelay();
	}
	if (GetKeyState('2') & 0x80){
		m_pRockCollision->DecreaseDelay();
	}

	cameraPositions.clear();
	cameraPositions.push_back(p - zoomFactor*T + (37.5f*B) + glm::vec3(0.0f, gameYPlane, 0));
	cameraPositions.push_back(p - (zoomFactor + 55.0f)*T + (120.0f*B) + glm::vec3(0.0f, gameYPlane, 0));
	cameraPositions.push_back(p + glm::vec3(0.0f, gameYPlane - 15.0f, 0.0f) + N*displacement);
	cameraPositions.push_back(glm::vec3());

	x = cameraPositions.at(camPos);

	glm::vec3 y(p + 120.0f*T);
	glm::vec3 z(0, 1, 0);

	if (camPos != cameraPositions.size() - 1){
		m_pCamera->Set(x, y, z);
	}

	m_JetOrientation = glm::mat4(glm::mat3(T, B, N));
	m_JetPosition = p + N*displacement;

	currentPoint = p + N*displacement;
	nextPoint = pNext;

	for (int i = 0; i < heartResonaters.size(); i++){
		if (operativeHearts[i]){
			heartResonaters[i].SetSourceAttributes(heartPositions[i], glm::vec3(0, 0, 0));
			heartResonaters[i].SetListenerAttributes(m_pCamera->GetPosition(), y*cameraSpeed, glm::normalize(m_pCamera->GetView()), m_pCamera->GetUpVector());
			heartResonaters[i].Update();
		}
		m_pGetHeart->Update();
	}

	m_pFly->SetSourceAttributes(m_JetPosition, y*cameraSpeed);
	m_pFly->SetListenerAttributes(m_pCamera->GetPosition(), y*cameraSpeed, glm::normalize(y), z);
	m_pFly->Update();

	m_pNextLap->Update();
	m_pBackgroundMusic->Update();
	m_pRockCollision->Update();

	rotation += 0.1f * m_dt;

	LapChecker();
	WobbleJet();
	CameraChecker();
	CheckCollisions();
	//UpdateStatues();

	timeScore += 1.0 / FPS;
	blinkCount += 5.0 / FPS;
}

void Game::CheckCollisions(){
	for (int i = 0; i < heartPositions.size(); i++){
		if (glm::length(heartPositions[i] - currentPoint) < m_pCatmullRom->GetPathWidth() / 6){
			if (operativeHearts[i] && !isShaking){
				m_pGetHeart->PlayEventSound(true, 0.6f);
				heartResonaters[i].Release();
				lives++;
			}
			operativeHearts[i] = false;
			//hearts[i].~COpenAssetImportMesh();
			//hearts.erase(hearts.begin() + i);
		}
	}

	for (int i = 0; i < cubes.size(); i++){
		if (glm::length(cubePositions[i] - currentPoint) < m_pCatmullRom->GetPathWidth() / 6){
			if (operativeCubes[i] && !isShaking){
				lives--;
				m_pRockCollision->PlayEventSound();
				hitTimer = 0;
				blinkCount = 0;
				isShaking = true;
				operativeCubes[i] = false;
			}
		}
	}

	for (int i = 0; i < pymPositions.size(); i++){
		if (glm::length(pymPositions[i] - currentPoint) < m_pCatmullRom->GetPathWidth() / 6){
			if (operativePyramids[i] && !isShaking){
				isRestTimerOn = true;
				isCameraRetreating = true;
				restTimer += 5;
				cameraSpeed = slowSpeed;
				m_pSlowMo->PlayEventSound(false, 0.3f);
				rotSpeed = 25.0f;
			}
			operativePyramids[i] = false;
		}
	}
}

void Game::LapChecker(){

	if (glm::length(m_pCatmullRom->getCentreLinePointVector().back() - currentPoint) < 20 ||
		glm::length(m_pCatmullRom->getLeftMidVector().back() - currentPoint) < 20 ||
		glm::length(m_pCatmullRom->getRightMidVector().back() - currentPoint) < 20){

		if (lapTimer == 0){
			lapTimer = 5.0f;
			m_pNextLap->PlayEventSound(false);
			UpdatePathSpawns();
			isLapTimerOn = true;
		}
	}

	if (lapTimer > 0){
		lapTimer -= 1.0 / FPS;

		if (lapTimer <= 0){
			isLapTimerOn = false;
			lapTimer = 0;
		}
	}

	if (isRestTimerOn){
		restTimer -= 1.0 / FPS;

		if (zoomFactor > 75.0f && restTimer > 1.0f){
			zoomFactor -= 12.5 / FPS;
		}
		else if (restTimer <= 1.0f){
			zoomFactor += 50.0 / FPS;
		}

		if (restTimer <= 0){
			restTimer = 0;
			zoomFactor = 125.0f;
			cameraSpeed = normalSpeed;
			isRestTimerOn = false;
			rotSpeed = 50.0f;
		}
	}
}

void Game::DisplayFrameRate()
{
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
	}

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());

		fontProgram->SetUniform("vColour", glm::vec4(textR / rgbDenominator, textG / rgbDenominator, textB / rgbDenominator, 1.0f));
		m_pTimeScore->Render(fontHeight / 2, height - (fontHeight * 1 * 1.5f), fontHeight*1.5f, "Score: %.2f", timeScore);
		fontProgram->SetUniform("vColour", glm::vec4(183 / rgbDenominator, 11 / rgbDenominator, 59 / rgbDenominator, 1.0f));
		m_pLives->Render(fontHeight / 2, height - (fontHeight * 2 * 1.25f) - 3, fontHeight * 1.25f, "Lives: %d", lives);

		fontProgram->SetUniform("vColour", glm::vec4(176 / rgbDenominator, 198 / rgbDenominator, 232 / rgbDenominator, 1.0f));

		if (!isLapTimerOn){
			m_pLaps->Render(fontHeight / 2, dimensions.top + fontHeight*0.75f, fontHeight*0.85f, "Lap %d", m_pCatmullRom->CurrentLap(m_currentDistance) + 1);
		}
		else{
			m_pLaps->Render(width / 2 - 80, height / 2, fontHeight*2.5f, "Lap %d", m_pCatmullRom->CurrentLap(m_currentDistance) + 1);
		}

		if (isRestTimerOn){
			m_pSpeedTimer->Render(width - 200, dimensions.top + fontHeight*0.75f, fontHeight*0.85f, "Rest Bonus: %.2f", restTimer);
		}

		m_pDelay->Render(fontHeight / 2, height - (fontHeight * 5 * 1.25f) - 3, fontHeight * 1.25f, "Delay: %d", m_pRockCollision->GetDelayFactor());
	}
}

void Game::WobbleJet(){
	if (isRotatingLeft){
		T_Rot -= rotSpeed / FPS;
	}
	else{
		T_Rot += rotSpeed / FPS;
	}

	if (T_Rot <= -1.25f){
		isRotatingRight = true;
		isRotatingLeft = false;
	}
	else if (T_Rot >= 1.25f){
		isRotatingLeft = true;
		isRotatingRight = false;
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{

	// Fixed timer
	m_dt = m_pHighResolutionTimer->Elapsed();
	if (m_dt > 1000.0 / (double)Game::FPS) {
		m_pHighResolutionTimer->Start();
		Update();
		Render();
	}

	/*
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();*/
}


WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if (!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();


	MSG msg;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (m_appActive) {
			GameLoop();
		}
		else Sleep(200); 
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {

	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}

	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			//m_pBackgroundMusic->PlayEventSound();
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance()
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance)
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int)
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
