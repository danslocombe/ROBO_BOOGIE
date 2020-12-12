#include "AudioPlayer.h"

constexpr size_t BUFFER_SIZE = 44100 * 4;

AudioPlayer::AudioPlayer(FMOD::Sound* sound) : m_ringBuffer(BUFFER_SIZE), m_sound(sound)
{
    m_dsp = nullptr;

	// Do we need this?
    memset(&m_dspDescr, 0, sizeof(m_dspDescr));
    
    strncpy_s(m_dspDescr.name, "Audio Player DSP", sizeof(m_dspDescr.name));
    m_dspDescr.version = 0x00010000;
    m_dspDescr.numinputbuffers = 1;
    m_dspDescr.numoutputbuffers = 1;
    m_dspDescr.read = DspStaticCallback; 
    m_dspDescr.userdata = nullptr;
}

FMOD_RESULT AudioPlayer::Register(FMOD::System* sys, std::string& error)
{
    FMOD_RESULT result = sys->createDSP(&m_dspDescr, &m_dsp);
    if (result != FMOD_OK)
    {
        error = "Could not create DSP";
        return result;
    }

    FMOD::ChannelGroup* masterGroup = nullptr;
    result = sys->getMasterChannelGroup(&masterGroup);

    if (result != FMOD_OK && masterGroup != nullptr)
    {
        error = "Could not get master channel";
        return result;
    }

    // Push to end of dsp list.
    result = masterGroup->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, m_dsp);
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
    for (uint32_t samp = 0; samp < length; samp++)
    {
        // We want to record in mono so average over channels
        float averagedSample = 0.0;
        for (int chan = 0; chan < *outChannels; chan++)
        {
			const uint32_t offset = (samp * *outChannels) + chan;
            float value = inbuffer[offset];// *1.f;
            averagedSample += value / ((float)(*outChannels));

            //outbuffer[offset] = value * 0.5 + m_ringBuffer.ReadOffset(100) * 0.5;
            //outbuffer[offset] = value;
        }

        m_ringBuffer.Push(averagedSample);
    }

    constexpr int baseOffset = -1024;
    for (uint32_t i = 0; i < length; i++)
    {
        m_ringBuffer.ReadOffset(baseOffset + i);
    }

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

