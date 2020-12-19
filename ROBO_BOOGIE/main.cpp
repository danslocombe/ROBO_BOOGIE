#include <fmod.hpp>
#include <iostream>
#include <fmod_errors.h>
#include <thread>
#include <chrono>
#ifdef PI 
	#include "PiIO.h"
#else
	#include "DevIO.h"
#endif

#include "AudioPlayer.h"

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

int main()
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
  
    std::cout << "Creating sound" << std::endl;

#ifdef PI
    const std::string soundPath("/home/pi/boom.wav");
#else
    const std::string soundPath("C:\\users\\dan\\music\\boom.wav");
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
      //const auto res = ioRead();
      //std::cout << res << std::endl;
	  char line[100];
	  std::cin.getline(line, 100);

	  toggle = !toggle;

      const double d = std::atof(line);
      if (d != 0.0)
      {
          player->SetVelK(d);
      }

      if (toggle) {
          player->Play();
      }
      else {
          player->Pause();
      }
    }
    /*
    for (;;)
    {
        using namespace std::chrono_literals;
        std::cout << player->GetOffset() << std::endl;
        std::this_thread::sleep_for(500ms);
    }
    for (;;)
    {
        char line[100];
        std::cin.getline(line, 100);
        std::cout << player->GetOffset() << std::endl;

        if (player->GetPlaying())
        {
            player->Pause();
        }
        else
        {
            player->Play();
        }
    }
    */
  
    return 0;
  }
