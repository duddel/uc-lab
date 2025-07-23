#!/bin/bash

docker run --rm -v $(pwd)/../..:/code uc-lab /bin/bash -c "cd projects/Gamelight && mkdir -p build && cd build && cmake -DPICO_BOARD=pico2 .. && make"
