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

#ifdef PI 
    #include "PiIO.h"
#else
    #include "DevIO.h"
#endif

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
AudioPlayer* player = nullptr;

void loop()
{
    const auto result = sys->update();
    assert_result(result);
}

int main_audio(RoutineSet routineSet)
{
    ioInit();
    // Reset positions
    //ioPwmWrite(0, 0.0);
    //ioPwmWrite(1, 0.0);

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
  
    std::cout << "Creating sound" << std::endl;

#ifdef PI
    const std::string soundPath("/home/pi/boom.wav");
#else
    const std::string soundPath("C:\\users\\daslocom\\music\\crunch.wav");
#endif

    result = sys->createSound(soundPath.c_str(), FMOD_DEFAULT, 0, &sound1);
    assert_result(result);
  
    std::cout << "Ok" << std::endl;
  
    std::cout << "Playing:" << std::endl;
    const bool paused = true;
    result = sys->playSound(sound1, 0, paused, &channel);
    assert_result(result);

    player = new AudioPlayer(sound1, channel);
    std::string error;
    result = player->Register(sys, error);
    assert_result(result);

    bool toggle = true;
  
    for (;;)
    {
      ioDelay(50);

      if (ioRead()) {
          if (!player->IsPlaying())
          {
              // TODO set a random speedup.
              player->Play();
          }

          routineSet.Run();
          player->Pause();
      }
      else {
          player->Pause();
      }
    }
  
    return 0;
}

RoutineSet parse()
{
#ifdef PI
    const std::string path("/home/pi/ROBO_BOOGIE/movesets/demo.moves");
#else
    const std::string path("C:\\Users\\daslocom\\source\\repos\\ROBO_BOOGIE\\movesets\\demo.moves");
#endif

    std::cout << "Parsing " << path << std::endl;

    std::ifstream movesetFile(path);

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(movesetFile, line))
    {
        trimEnd(line);
        std::cout << line << std::endl;
        lines.emplace_back(line);
    }

    RoutineSetParser parser;
    return parser.ParseFile(lines);
}

int main()
{
    auto routineSet = parse();
    return main_audio(routineSet);
}
