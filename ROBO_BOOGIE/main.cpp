#include <fmod.hpp>
#include <iostream>
#include <fmod_errors.h>
#include <thread>
#include <chrono>

#include <fstream>
#include <sstream>

#include "AudioPlayer.h"
#include "RoutineSet.h"
#include "StringCompat.h"
#include "SpeechSynth.h"

#ifdef PI 
    #include "PiIO.h"
#else
bool switchEnabled;
    #include "DevIO.h"
#endif

void assert_result(bool res, const std::string& str)
{
    if (!res)
    {
        std::cerr << str << std::endl;
        exit(1);
    }
}

void assert_result(FMOD_RESULT res, const std::string& str)
{
    if (res != FMOD_OK)
    {
        std::cerr << str << std::endl;
        std::cerr << FMOD_ErrorString(res) << std::endl;
        exit(1);
    }
}

void assert_result(FMOD_RESULT res)
{
    if (res != FMOD_OK)
    {
        std::cerr << FMOD_ErrorString(res) << std::endl;
        exit(1);
    }
}

FMOD::System *sys = nullptr;
unsigned int version;
FMOD::Sound* sound1 = nullptr;
FMOD::Channel* channel = nullptr;

void loop()
{
    const auto result = sys->update();
    assert_result(result);
}

int main_audio(RoutineSet routineSet, const ConstantObj& voiceConfig, const std::string& soundPath)
{
    ioInit();

    auto result = FMOD::System_Create(&sys);
    assert_result(result);
  
    result = sys->getVersion(&version);
    assert_result(result);
  
    if (version < FMOD_VERSION)
    {
      return 1;
    }
  
    std::cout << "Starting, FMOD Version: " << version << std::endl;
  
    result = sys->init(32, FMOD_INIT_NORMAL, nullptr);
    assert_result(result);
  
    std::cout << "Creating sound " << soundPath << std::endl;

    result = sys->createSound(soundPath.c_str(), FMOD_LOOP_NORMAL, 0, &sound1);
    assert_result(result);
    std::cout << "Loading Music" << std::endl;

    FMOD::Sound *snore;
#ifdef PI
    std::string snorePath("/home/pi/snore.wav");
#else
    std::string snorePath("C:\\users\\daslocom\\Music\\snore.wav");
#endif
    std::cout << "Loaded Snore" << std::endl;

    result = sys->createSound(snorePath.c_str(), FMOD_LOOP_NORMAL, 0, &snore);
    assert_result(result);
    FMOD::Channel *snoreChannel;
    result = sys->playSound(snore, 0, false, &snoreChannel);
    assert_result(result);

    FMOD::ChannelGroup* channelGroup;
    result = sys->createChannelGroup("music", &channelGroup);
    assert_result(result);

    const bool paused = true;
    result = sys->playSound(sound1, channelGroup, paused, &channel);
    assert_result(result);

    /*
    FMSynthDSP synth;
    std::string error;
    assert_result(synth.Register(sys, error), error);
    synth.GetConfigMut().Wave = WaveType::SIN;
    synth.GetConfigMut().Freq = 0.5;
    synth.GetConfigMut().AmpASDR.Attack = 3000.0;
    synth.GetConfigMut().AmpASDR.Decay = 1.0;
    synth.GetConfigMut().AmpASDR.Sustain = 1.0;
    synth.GetConfigMut().AmpASDR.Release = 3000.0;
    synth.SetEnabled(true);
    */

    SpeechSynthDSP speechSynth;
    std::string error;
    assert_result(speechSynth.Register(sys, error), error);

    AudioPlayer player(sound1, channel);
    result = player.Register(sys, channelGroup, error);
    assert_result(result);

    bool toggle = true;
    Routine* routine = nullptr;
    bool switchEnabled = false;

    bool x = false;
    bool snoring = true;

    // TODO start snoring again
    //const int snoreBuildup = 20;
    for (;;)
    {
        if (routine != nullptr && routine->Run(speechSynth, voiceConfig))
        {
            switchEnabled = ioRead();
        }
        else
        {
            routine = nullptr;

            ioDelay(50);
            switchEnabled = ioReadBlock();

            if (switchEnabled)
            {
                snoring = false;
                routine = routineSet.GetRoutineIncrement();
                std::cout << "Loaded routine: " << routine->Name << std::endl;
            }
        }

        if (switchEnabled)
        {
            // TODO set random speed?
            player.Play();
        }
        else
        {
            player.Pause();
        }

        snoreChannel->setPaused(!snoring);
    }
  
    return 0;
}

ConstantObj parseVoiceConfig()
{
#ifdef PI
    std::string path("/home/pi/ROBO_BOOGIE/movesets/voices.config");
#else
    std::string path("C:\\Users\\daslocom\\source\\repos\\ROBO_BOOGIE\\movesets\\voices.config");
#endif
    std::ifstream file(path);

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line))
    {
        trimEnd(line);
        lines.emplace_back(line);
    }

    return ParseValues(lines);
}

RoutineSet parse()
{
#ifdef PI
    std::string path("/home/pi/ROBO_BOOGIE/movesets/demo.moves");
#else
    std::string path("C:\\Users\\daslocom\\source\\repos\\ROBO_BOOGIE\\movesets\\demo.moves");
#endif

    std::cout << "Input moveset:  " << path << std::endl;

    std::ifstream movesetFile(path);

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(movesetFile, line))
    {
        trimEnd(line);
        lines.emplace_back(line);
    }

    RoutineSetParser parser;
    return parser.ParseFile(lines);
}

int main(int argc, char** argv)
{
#ifdef PI
    std::string soundPath("/home/pi/boom.wav");
#else
    //std::string soundPath("C:\\users\\daslocom\\music\\crunch.wav");
    std::string soundPath("C:\\users\\daslocom\\music\\laura.wav");
#endif

    if (argc > 1)
    {
        soundPath = std::string(argv[1]);
    }

    auto routineSet = parse();
    const auto config = parseVoiceConfig();
    return main_audio(routineSet, config, soundPath);
}
