#include "DelayAudio.h"

#pragma comment(lib, "lib/fmodex_vc.lib")

int delayFactor;	// delay factor defined here otherwise its not able to be recognised by the myDSPCallback method as a global variable
const int buffer_size = 60000;

CDelayAudio::CDelayAudio(const bool isL){
	isLoop = isL;
	delayFactor = 0;
}

CDelayAudio::~CDelayAudio(){

}

void CDelayAudio::PlayEventSound(){
	result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	ERRCHECK(result);
}

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int outchannels)
{
	unsigned int samp;
	int chan;
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;
	static int sample_count = 0;

	static float *buffer = NULL;

	if (buffer == NULL)
		buffer = (float*)malloc(buffer_size * sizeof(float) * outchannels);

	for (samp = 0; samp < length; samp++)
	{
		for (chan = 0; chan < outchannels; chan++)
		{
			buffer[(sample_count * outchannels) % buffer_size + chan] = inbuffer[(samp * outchannels) + chan];

			if (sample_count < delayFactor) // don't reach before the the start of the buffer with sample_count-3 below
				outbuffer[(samp * outchannels) + chan] = 0.0f;
			else{
				// this is a simple averaging filter with 4 coefficients
				outbuffer[(samp * outchannels) + chan] = 0.5f*buffer[(sample_count * outchannels) % buffer_size + chan];
				outbuffer[(samp * outchannels) + chan] += 0.5f*buffer[((sample_count - delayFactor) * outchannels) % buffer_size + chan];
			}
		}
		sample_count++;
	}

	return FMOD_OK;
}

void CDelayAudio::ERRCHECK(FMOD_RESULT result)
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
		} while (key != 27); // wait for escape key

		exit(-1);
	}
}

void CDelayAudio::IncreaseDelay(){
	delayFactor += delayStep;

	if (delayFactor > (buffer_size / 2)){
		delayFactor = 0;
	}
}

void CDelayAudio::DecreaseDelay(){
	delayFactor -= delayStep;

	if (delayFactor < 0){
		delayFactor = (buffer_size / 2);
	}
}

void CDelayAudio::Initialise(char * filename){

	result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
	}

	result = system->getNumDrivers(&numdrivers);
	ERRCHECK(result);

	if (numdrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
	}
	else
	{
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		ERRCHECK(result);

		result = system->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = system->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);

		if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}

	result = system->init(100, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);

		result = system->init(100, FMOD_INIT_3D_RIGHTHANDED, 0);/* ... and re-init. */
		ERRCHECK(result);
	}

	result = system->createSound(filename, FMOD_SOFTWARE | FMOD_3D | FMOD_3D_CUSTOMROLLOFF, 0, &sound); //FMOD_SOFTWARE | FMOD_3D
	ERRCHECK(result);

	if (isLoop){
		result = sound->setMode(FMOD_LOOP_NORMAL); // change to FMOD_LOOP_OFF if needed
	}
	else{
		result = sound->setMode(FMOD_LOOP_OFF); // change to FMOD_LOOP_NORMAL if needed
	}

	ERRCHECK(result);

	{
		FMOD_DSP_DESCRIPTION  dspdesc;

		memset(&dspdesc, 0, sizeof(FMOD_DSP_DESCRIPTION));

		strcpy_s(dspdesc.name, "My first DSP unit");
		dspdesc.channels = 0;                   // 0 = whatever comes in, else specify. 
		dspdesc.read = myDSPCallback;

		result = system->createDSP(&dspdesc, &mydsp);
		ERRCHECK(result);
	}

	mydsp->setBypass(false);
	result = system->addDSP(mydsp, 0);
	ERRCHECK(result);
}

int CDelayAudio::GetDelayFactor(){
	return delayFactor;
}

void CDelayAudio::Release(){
	result = sound->release();
	ERRCHECK(result);
	result = system->close();
	ERRCHECK(result);
	result = system->release();
	ERRCHECK(result);
}

void CDelayAudio::Update()
{
	if (system) {
		result = system->update();
		ERRCHECK(result);
	}
}