#pragma once
#include <windows.h>									// Header File For The Windows Library
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "Common.h"
#include "./include/fmod/fmod.h"
#include "./include/fmod/fmod.hpp"
#include "./include/fmod/fmod_dsp.h"
#include "./include/fmod/fmod_errors.h"

class CAudio
{
public:
	CAudio(const bool isL, const bool isSound3D = false);	// Constructor, determines whether the audio source will be a loop and/or 3D
	~CAudio();
	void Initialise(char * filename);	// Initialises the sound object, takes a pathway to determine which file will be played
	void Update();						// Updates the audio every frame to allow real-time use of effects
	void Release();						// Releases all objects so it stops playing and clears from memory
	void PlayEventSound(const bool playDistortion, const float vol = 1.0f);	// Plays the sound when required, takes a parameter for distortion and volume
	void SetListenerAttributes(const glm::vec3 &lPos, const glm::vec3 &lVel, const glm::vec3 &lFor, const glm::vec3 &lUp);	// Sets the variables for the listener
	void SetSourceAttributes(const glm::vec3 &sPos, const glm::vec3 &sVel);	// Sets the variables for the source of the sound
	void SetSoundAttributes(const float rollOffXSteps);	// Sets the roll off for doppler, takes a float parameter for the spacing between each roll off point.

private:

	const int delayStep = 25;
	
	void ERRCHECK(FMOD_RESULT result);

	FMOD::System    *system;
	FMOD::Sound		*sound;
	FMOD::Channel	*channel = 0;
	FMOD_RESULT      result;
	FMOD::DSP		*dsp;
	unsigned int     version;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS        caps;
	int              key = 0, numdrivers;
	char             name[256];
	bool			 delay;

	FMOD_VECTOR rollOff[6];
	boolean initialised;

	bool isLoop;
	bool is3D;

	FMOD_VECTOR sPos;
	FMOD_VECTOR sVel;
};
