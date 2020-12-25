#pragma once

#include "FMSynth.h"
#include "ConstantObj.h"

class SpeechSynthDSP
{
public:
    SpeechSynthDSP();

    enum class SpeechSynthState
    {
        SYNTH_TALKING,
        SYNTH_STOPPED,
    };


    bool Register(FMOD::System* sys, std::string& error);

    void Talk(const std::string& text);
    void SetSpeaker(const std::string& speaker);
    void Tick(const ConstantObj& config);

    std::optional<std::string> TryGetText() const;
    bool IsTalking() const;

    RingBuffer m_freqBuf;
private:
    void UpdateConfigFromReader(const ConstantObj& constObj);
    void MutateConfig(char c, const ConstantObj& constObj);
    void NextChar(uint32_t c, const ConstantObj& config);

    FMSynthDSP m_synth;

    std::string m_text;
    std::string m_speaker;

    bool m_talking = false;
    uint32_t m_textCurChar = 0;
    uint32_t m_curCharLen = 0;
    uint32_t m_curCharEndWait = 0;
    uint32_t m_curCharT = 0;
};
