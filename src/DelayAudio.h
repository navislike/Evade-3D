#pragma once
#include "Common.h"
#include <windows.h>									// Header File For The Windows Library
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "./include/fmod/fmod.h"
#include "./include/fmod/fmod.hpp"
#include "./include/fmod/fmod_dsp.h"
#include "./include/fmod/fmod_errors.h"

class CDelayAudio
{
public:
	CDelayAudio(const bool isL = false);
	~CDelayAudio();
	void Initialise(char * filename);
	void Update();
	void Release();
	void PlayEventSound();
	void IncreaseDelay();
	void DecreaseDelay();
	int GetDelayFactor();
	bool isDelay;

private:

	const int delayStep = 25;

	void ERRCHECK(FMOD_RESULT result);

	FMOD::System    *system;
	FMOD::Sound     *sound;
	FMOD::Channel   *channel = 0;
	FMOD_RESULT      result;
	FMOD::DSP       *mydsp;
	unsigned int     version;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS        caps;
	int              key = 0, numdrivers;
	bool			 isLoop;
	char             name[256];

	boolean m_initialised;
};
