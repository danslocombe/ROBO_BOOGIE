#!/bin/bash
export SOURCES="*.cpp"
export FLAGS="-marm -march=armv6 -mfpu=vfp -mfloat-abi=hard"
export FMODLIB="-Wl,-rpath=. ./libfmod.so.12 -lwiringPi"
export COMMAND="g++ $SOURCES $FLAGS -I /usr/local/include $FMODLIB -o bin/audio"
echo $COMMAND
$COMMAND
