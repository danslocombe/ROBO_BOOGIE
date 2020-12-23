#include "AudioPlayer.h"
#include <cstring>
#include <string.h>

constexpr size_t BUFFER_SIZE = 44100 * 4;

AudioPlayer::AudioPlayer(FMOD::Sound* sound, FMOD::Channel* channel) : m_ringBuffer(BUFFER_SIZE)
{
    m_dsp = nullptr;

    // Do we need this?
    memset(&m_dspDescr, 0, sizeof(m_dspDescr));
    
    //strncpy_s(m_dspDescr.name, "Audio Player DSP", sizeof(m_dspDescr.name));
    m_dspDescr.version = 0x00010000;
    m_dspDescr.numinputbuffers = 1;
    m_dspDescr.numoutputbuffers = 1;
    m_dspDescr.read = DspStaticCallback; 
    m_dspDescr.userdata = nullptr;

    m_offset = 0;
    m_vel = 0.0;
    m_playing = false;
    m_sound = sound;
    m_channel = channel;
}

void AudioPlayer::Play()
{
    m_playing = true;
    m_channel->setPaused(false);
}

void AudioPlayer::Pause()
{
    m_playing = false;
    m_channel->setPaused(true);
}

FMOD_RESULT AudioPlayer::Register(FMOD::System* sys, FMOD::Channel* channel, std::string& error)
//FMOD_RESULT AudioPlayer::Register(FMOD::System* sys, std::string& error)
{
    m_sys = sys;

    FMOD_RESULT result = sys->createDSP(&m_dspDescr, &m_dsp);
    if (result != FMOD_OK)
    {
        error = "Could not create DSP";
        return result;
    }

    /*
    FMOD::ChannelGroup* masterGroup = nullptr;
    result = sys->getMasterChannelGroup(&masterGroup);

    if (result != FMOD_OK && masterGroup != nullptr)
    {
        error = "Could not get master channel";
        return result;
    }
    */

    // Push to end of dsp list.
    result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, m_dsp);
    if (result != FMOD_OK)
    {
        error = "Could not add dsp";
        return result;
    }

    m_dsp->setUserData(reinterpret_cast<void*>(this));

    return FMOD_OK;
}

FMOD_RESULT AudioPlayer::Callback(
    float* inbuffer,
    float* outbuffer,
    uint32_t length,
    int inChannels,
    int* outChannels)
{
    if (m_playing)
    {
        for (uint32_t samp = 0; samp < length; samp++)
        {
            // We want to record in mono so average over channels
            float averagedSample = 0.0;
            for (int chan = 0; chan < *outChannels; chan++)
            {
                const uint32_t offset = (samp * *outChannels) + chan;
                float value = inbuffer[offset];// *1.f;
                averagedSample += value / ((float)(*outChannels));
            }

            m_ringBuffer.Push(averagedSample);
        }

        m_offset -= length;
    }

    double f_offset = m_offset;

    for (uint32_t i = 0; i < length; i++)
    {
        if (m_playing)
        {
            m_vel = (m_vel * (m_vel_k-1.0) + 1.0) / m_vel_k;
        }
        else
        {
            m_vel = (m_vel * (m_vel_k - 1.0)) / m_vel_k;
        }

        f_offset += m_vel;
        if (f_offset > 0.0)
        {
            f_offset = 0.0;
        }

        // Do we need to interpolate?
        const float x = m_ringBuffer.ReadOffset((int)f_offset);
        for (int chan = 0; chan < *outChannels; chan++)
        {
            const uint32_t offset = (i * *outChannels) + chan;
            outbuffer[offset] = x;
        }
    }

    m_offset = (int)(f_offset);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AudioPlayer::DspStaticCallback(
    FMOD_DSP_STATE* dsp_state,
    float* inbuffer,
    float* outbuffer,
    uint32_t length,
    int inchannels,
    int* outchannels)
{
    FMOD_RESULT result;

    FMOD::DSP *thisdsp = reinterpret_cast<FMOD::DSP *>(dsp_state->instance); 

    AudioPlayer* player;
    result = thisdsp->getUserData(reinterpret_cast<void **>(&player));

    if (result != FMOD_OK)
    {
        return result;
    }

    return player->Callback(inbuffer, outbuffer, length, inchannels, outchannels);
}

