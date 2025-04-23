#!/bin/bash

if [ ! -d "build_$1" ]; then
  mkdir build_$1
  cd build_$1
  cmake -DPICO_BOARD=$1 .. || exit $?
  cd -
fi

cd build_$1
if [ -z "$2" ]; then
  cmake --build . || exit $?
else
  cmake --build . --target "$2" || exit $?
fi
cd -
