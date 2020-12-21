#!/bin/bash
export SOURCES="*.cpp"
export FLAGS="-marm -march=armv6 -mfpu=vfp -O2 -mfloat-abi=hard -std=c++17 -D_GLIBCXX_USE_CXX11_ABI=0 -D PI"
export FMODLIB="-Wl,-rpath=. ./libfmod.so.12 -lwiringPi"
export COMMAND="g++ $SOURCES $FLAGS -I /usr/local/include $FMODLIB -o bin/audio"
echo $COMMAND
$COMMAND
