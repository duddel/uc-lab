#!/bin/bash

if [ ! -d "build_$1" ]; then
  mkdir build_$1
  cd build_$1
  cmake -DPICO_BOARD=$1 .. || exit $?
  cd -
fi

cd build_$1
cmake --build . || exit $?
cd -
