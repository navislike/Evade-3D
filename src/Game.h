#pragma once

#include "Common.h"
#include "GameWindow.h"
#define LEFT 0
#define CENTRE 1
#define RIGHT 2
 
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CDelayAudio;
class CCatmullRom;
class CCube;
class CDoublePyramid;

class Game {
private:

	CSkybox *m_pSkybox;	// Skybox variable
	CCamera *m_pCamera;	// Camera variable
	vector <CShaderProgram *> *m_pShaderPrograms;	// Shader variable
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pLives;		// HUD for lives display
	CFreeTypeFont *m_pSpeedTimer;	// HUD for speed timer
	CFreeTypeFont *m_pLaps;			// HUD for laps timer
	CFreeTypeFont *m_pTimeScore;	// HUD for the score	
	CFreeTypeFont *m_pDelay;		// HUD for the delay
	COpenAssetImportMesh *m_pFighterJet;
	COpenAssetImportMesh *m_pMoon;
	COpenAssetImportMesh *m_pStadium;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pBackgroundMusic;	// audio for the background music
	CAudio *m_pGetHeart;		// audio for the collision for heart pick up
	CDelayAudio *m_pRockCollision;	// audio for the collision for rock hit
	CAudio *m_pSlowMo;				// audio for the slowing down of jet
	CAudio *m_pFly;					// audio for the flying of the spaceship
	CAudio *m_pNextLap;				// audio for the next lap verification
	CCatmullRom *m_pCatmullRom;
	CCube *m_pCube;

	HINSTANCE m_hInstance;
	GameWindow m_gameWindow;

	static const int FPS = 60;

	const float textR = 126.0f; // RGB colours
	const float textG = 185.0f;	// RGB colours
	const float textB = 234.0f;	// RGB colours
	const float rgbDenominator = 255.0f;	// RGB colours
	const float normalSpeed = 0.5f;	// Speed of spaceship (0.5f)
	const float slowSpeed = normalSpeed/2; // slow speed of spaceship
	const float reAdjustTime = 1.5f; // time to fix itself after hit
	const int fontHeight = 30; // height of font
	const int skipSpawnFactor = 100; // to skip first few points
	const float dodgeSpeed = 4.0f; // how fast it can displace
	const float adjusted = 2.0f; // where to adjust 

	int m_framesPerSecond;
	int boxNumbers; // number of boxes
	int heartNumbers; // number of hearts
	int lives; // number of lives
	int pymNumbers;
	int randomPath;
	int randomIndex;
	int m_frameCount;
	int camPos;
	double camTimer; // timer for camera switches
	double blinkCount; // timer for blink switches
	double m_dt;
	double m_elapsedTime;
	double restTimer; // timer for rest
	double timeScore; // score
	double result;
	double hitTimer;
	double adjustTimer;
	float rotationCount;
	float m_currentDistance;
	float cameraSpeed;
	float rotation;
	float gameYPlane;
	float cubeScale;
	float lapTimer;
	float zoomFactor;
	float T_Rot;
	float rotSpeed;
	float angleOfRotation;
	float N_displacement;
	float m_t;
	float displacement;
	bool m_appActive;
	bool isRestTimerOn;
	bool isShaking;
	bool isCameraRetreating;
	bool isRotatingLeft;
	bool isRotatingRight;
	bool isLapTimerOn;
	bool camJustChanged;

	glm::vec3 m_JetPosition;
	glm::vec3 T;
	glm::vec3 N;
	glm::vec3 B;
	glm::vec3 nextPoint;
	glm::vec3 currentPoint;
	glm::vec3 camera;

	glm::mat4 m_JetOrientation;

	vector<CCube> cubes;
	vector<CDoublePyramid> dPym;
	vector<COpenAssetImportMesh> hearts;
	vector<CAudio> heartResonaters;
	vector<glm::vec3> pymPositions;
	vector<glm::vec3> pymNextPositions;
	vector<glm::vec3> cubePositions;
	vector<glm::vec3> cubeNextPositions;
	vector<glm::vec3> heartPositions;
	vector<glm::vec3> heartNextPositions;
	vector<glm::vec3> takenPositions;
	vector<glm::vec3> cameraPositions;
	vector<bool> operativeHearts;
	vector<bool> operativeCubes;
	vector<bool> operativePyramids;

public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	void Initialise();			// Initalises all the game components to get it ready.
	void Update();				// Updates any logic required for game to function.
	void Render();				// Renders the objects into the world.
	void DisplayFrameRate();	// Displays text on screen
	void GameLoop();			// Runs repeatedly for a fixed frame rate
	void UpdatePathSpawns();	// Changes the positions of all the collectables on the path every new lap
	void DeterminePathSpawn(vector<glm::vec3> &toVec, vector<glm::vec3> &nextVec);	// Determines which part of the path a collectable will spawn
	void AssignSpawnPoint(vector<glm::vec3> &toVec, vector<glm::vec3> &fromVec, vector<glm::vec3> &nextVec);	// Assigns a point on the path 
	void WobbleJet();		// Wobbles the jet
	void LapChecker();		// Checks to see if there is a new lap
	void CameraChecker();	// Checks to see if camera needs to be updated
	void CheckCollisions();	// Checks for collisions of objects in the world
};
