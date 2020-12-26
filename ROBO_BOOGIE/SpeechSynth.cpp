#include "SpeechSynth.h"
#include "StringHelpers.h"

SpeechSynthDSP::SpeechSynthDSP() : m_freqBuf(128)
{
}

bool SpeechSynthDSP::Register(FMOD::System* sys, std::string& error)
{
    const bool success = this->m_synth.Register(sys, error);
    if (success)
    {
        this->m_synth.SetEnabled(true);
    }

    return success;
}

void SpeechSynthDSP::SetSpeaker(const std::string& speaker)
{
    m_speaker = std::string(speaker);
}

void SpeechSynthDSP::Talk(const std::string& text)
{
    m_synth.SetKeydown(true);
    m_text = std::string(text);
    m_textCurChar = 0;
    m_talking = true;
}

bool SpeechSynthDSP::IsTalking() const
{
    return this->m_talking;
}

std::optional<std::string> SpeechSynthDSP::TryGetText() const
{
    if (m_textCurChar < m_text.size())
    {
        return { m_text };
    }

    return {};
}

void SpeechSynthDSP::UpdateConfigFromReader(const ConstantObj& speakerConfig)
{
    auto& config = m_synth.GetConfigMut();
    
    config.AmpASDR.Attack = speakerConfig.GetDouble("amp_a");
    config.AmpASDR.Decay = speakerConfig.GetDouble("amp_d");
    config.AmpASDR.Sustain = speakerConfig.GetDouble("amp_s");
    config.AmpASDR.Release = speakerConfig.GetDouble("amp_r");
    config.AmpSmoothK = speakerConfig.GetDouble("amp_smooth");

    const auto shape = speakerConfig.GetString("shape");
    if (stringEqualIgnoreCase(shape, "sin"))
    {
        config.Wave = WaveType::SIN;
    }
    else if (stringEqualIgnoreCase(shape, "saw"))
    {
        config.Wave = WaveType::SAW;
    }
    else
    {
        config.Wave = WaveType::PULSE;
    }

    config.PulseWidth = 6.282 * speakerConfig.GetDouble("pulse_width");
    config.Freq = speakerConfig.GetDouble("freq");
    config.FreqSmoothK = speakerConfig.GetDouble("freq_smooth");
}

void SpeechSynthDSP::MutateConfig(char c, const ConstantObj& speakerConfig)
{
    auto& config = m_synth.GetConfigMut();

    const int pwr = rand() % 100;
    config.PulseWidth = 6.282 * (double)(pwr) / 100.0;
    config.AmpASDR.Attack += (double)(rand() % 1000) - 500.0;
    config.Freq += speakerConfig.GetDouble("freq_mod") * (double)(rand() % 100) / 100.0;
}

void SpeechSynthDSP::NextChar(uint32_t pos, const ConstantObj& config)
{
    m_textCurChar = pos;
    const char c = m_text[pos];
    if (c == ' ')
    {
        m_curCharLen = 0;
        m_curCharEndWait = 10;
    }
    else
    {
        const auto speakerConfig = config.GetObj(m_speaker);
        if (speakerConfig != nullptr)
        {
            this->UpdateConfigFromReader(*speakerConfig);
            this->MutateConfig(c, *speakerConfig);
            m_curCharLen = speakerConfig->GetUint("char_len");
            m_curCharEndWait = speakerConfig->GetUint("end_dur");
        }
    }

    m_curCharT = 0;
}

void SpeechSynthDSP::Tick(const ConstantObj& config)
{
    if (m_talking)
    {
        m_curCharT += 1;
        if (m_curCharT > m_curCharLen + m_curCharEndWait)
        {
            if (m_textCurChar < m_text.size())
            {
                this->NextChar(++m_textCurChar, config);
            }
            else
            {
                m_talking = false;
                m_text = "";
                m_curCharT = 0;
                m_curCharLen = 0;
            }
        }

        this->m_synth.SetKeydown(m_curCharT < m_curCharLen);
    }
    else
    {
        this->m_synth.SetKeydown(false);
    }
}