#pragma once
#include <stdint.h>
#include <fmod.hpp>
#include "RingBuffer.h"
#include <string>

class AudioPlayer
{
private:
	RingBuffer m_ringBuffer;
	FMOD::Sound* m_sound;
	int m_offset;
	double m_vel;
	
public:
	AudioPlayer(FMOD::Sound* sound);
	//~AudioPlayer();

	FMOD_RESULT Register(FMOD::System* sys, std::string& error);

private:
	FMOD::DSP* m_dsp;
	FMOD_DSP_DESCRIPTION m_dspDescr;

	FMOD_RESULT Callback(
		float* inbuffer,
		float* outbuffer,
		uint32_t length,
		int inchannels,
		int* outchannels);

	static FMOD_RESULT F_CALLBACK DspStaticCallback(
		FMOD_DSP_STATE* dsp_state,
		float* inbuffer,
		float* outbuffer,
		uint32_t length,
		int inchannels,
		int* outchannels);
};