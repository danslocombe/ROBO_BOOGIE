#include <fmod.hpp>
#include <iostream>
#include <fmod_errors.h>

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
    const std::string soundPath("C:\\users\\dan\\music\\diner2.wav");
    //const std::string soundPath("/home/pi/diner2.wav");
    result = sys->createSound(soundPath.c_str(), FMOD_DEFAULT, 0, &sound1);
    assert_result(result);
  
    std::cout << "Ok" << std::endl;
  
    std::cout << "Playing:" << std::endl;
    result = sys->playSound(sound1, 0, false, &channel);
    assert_result(result);

    player = new AudioPlayer(sound1);
    std::string error;
    result = player->Register(sys, error);
    assert_result(result);
  
    for (;;)
    {
    }
  
    return 0;
  }
