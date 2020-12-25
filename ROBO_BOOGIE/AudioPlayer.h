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
    FMOD::System* m_sys;
    FMOD::Channel* m_channel;
    int m_offset;
    double m_vel;
    bool m_playing;
    double m_vel_k = 30000.0;
    
public:
    AudioPlayer(FMOD::Sound* sound, FMOD::Channel* channel);
    //~AudioPlayer();

    FMOD_RESULT Register(FMOD::System* sys, FMOD::ChannelGroup* channel, std::string& error);

    int GetOffset() const
    {
        return m_offset;
    }

    double GetVel() const
    {
        return m_vel;
    }

    void SetVelK(double k)
    {
        m_vel_k = k;
    }

    void Play();
    void Pause();

    bool IsPlaying() const
    {
        return m_playing;
    }

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