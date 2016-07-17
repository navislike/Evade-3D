#include "Audio.h"

#pragma comment(lib, "lib/fmodex_vc.lib")

FMOD_VECTOR lPos;	// listener position defined here because it is the same for all objects of the class.
FMOD_VECTOR lVel;	// listener velocity defined here because it is the same for all objects of the class.
FMOD_VECTOR lFor;	// listener forward defined here because it is the same for all objects of the class.
FMOD_VECTOR lUp;	// listener up defined here because it is the same for all objects of the class.
const int buffer_size = 10000;

CAudio::CAudio(const bool isL, const bool isSound3D){
	isLoop = isL;
	is3D = isSound3D;
}

CAudio::~CAudio(){
}

void CAudio::SetSoundAttributes(const float rollOffXSteps){
	for (int i = 0; i < sizeof(rollOff) / sizeof(FMOD_VECTOR); i++){
		FMOD_VECTOR point;
		point.x = (i * rollOffXSteps); point.y = 1 - (i*0.2f); point.z = 0.0f;
		rollOff[i] = point;
	}
}

void CAudio::ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		int key = 0;
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		do
		{
			if (_kbhit())
			{
				key = _getch();
			}
		} while (key != 27); 

		exit(-1);
	}
}

void CAudio::PlayEventSound(const bool playDistortion, const float vol){
	ERRCHECK(system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel));
	ERRCHECK(channel->setVolume(vol));

	if (is3D){
		ERRCHECK(channel->set3DDopplerLevel(0.05f));
		ERRCHECK(channel->set3DCustomRolloff(rollOff, sizeof(rollOff) / sizeof(FMOD_VECTOR)));
	}

	if (playDistortion){
		ERRCHECK(system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &dsp));
		dsp->setParameter(FMOD_DSP_DISTORTION_LEVEL, 0.5f);
	}
	else{
		ERRCHECK(system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsp));
		dsp->setParameter(FMOD_DSP_LOWPASS_CUTOFF, 2500);
	}
	
	dsp->setBypass(false);
	ERRCHECK(system->addDSP(dsp, 0));
}

void CAudio::Initialise(char * filename){

	ERRCHECK(FMOD::System_Create(&system));
	ERRCHECK(system->getVersion(&version));

	if (version < FMOD_VERSION)
	{
		printf("Error! You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
	}

	ERRCHECK(system->getNumDrivers(&numdrivers));

	if (numdrivers == 0)
	{
		ERRCHECK(system->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
	else
	{
		ERRCHECK(system->getDriverCaps(0, &caps, 0, &speakermode));
		ERRCHECK(system->setSpeakerMode(speakermode));

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)          
		{                                                
			ERRCHECK(result = system->setDSPBufferSize(1024, 10));
		}

		ERRCHECK(system->getDriverInfo(0, name, 256, 0));

		if (strstr(name, "SigmaTel"))   
		{
			ERRCHECK(system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR));
		}
	}

	ERRCHECK(system->init(100, FMOD_INIT_3D_RIGHTHANDED, 0));
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)     
	{
		ERRCHECK(system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));
		ERRCHECK(system->init(100, FMOD_INIT_3D_RIGHTHANDED, 0));
	}

	if (is3D){
		ERRCHECK(system->createSound(filename, FMOD_SOFTWARE | FMOD_3D | FMOD_3D_CUSTOMROLLOFF, 0, &sound));
	}
	else{
		ERRCHECK(system->createSound(filename, FMOD_SOFTWARE | FMOD_2D, 0, &sound));
	}

	if (isLoop){
		ERRCHECK(sound->setMode(FMOD_LOOP_NORMAL));
	}
	else{
		ERRCHECK(sound->setMode(FMOD_LOOP_OFF));
	}
}

void CAudio::Release(){
	ERRCHECK(sound->release());
	ERRCHECK(system->close());
	ERRCHECK(system->release());
}

void CAudio::Update()
{
	if (system) {
		if (is3D){
			ERRCHECK(system->set3DListenerAttributes(0, &lPos, &lVel, &lFor, &lUp));
			ERRCHECK(channel->set3DAttributes(&sPos, &sVel));
		}

		ERRCHECK(system->update());
	}
}

void CAudio::SetListenerAttributes(const glm::vec3 &newLPos, const glm::vec3 &newLVel, const glm::vec3 &newLFor, const glm::vec3 &newLUp){
	lPos.x = newLPos.x; lPos.y = newLPos.y; lPos.z = newLPos.z;
	lVel.x = newLVel.x; lVel.y = newLVel.y; lVel.z = newLVel.z;
	lFor.x = newLFor.x; lFor.y = newLFor.y; lFor.z = newLFor.z;
	lUp.x = newLUp.x; lUp.y = newLUp.y; lUp.z = newLUp.z;
}

void CAudio::SetSourceAttributes(const glm::vec3 &newSPos, const glm::vec3 &newSVel){
	sPos.x = newSPos.x; sPos.y = newSPos.y; sPos.z = newSPos.z;
	sVel.x = newSVel.x; sVel.y = newSVel.y; sVel.z = newSVel.z;
}
