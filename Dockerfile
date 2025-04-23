FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# AVR
RUN apt-get update && apt-get install -y \
    avr-libc \
    binutils-avr \
    gcc-avr \
    make \
    # optionally, install avrdude
    #avrdude \
    && rm -rf /var/lib/apt/lists/*

# Raspberry Pi Pico
RUN apt-get update && apt-get install -y \
    g++ \
    python3 \
    git \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && rm -rf /var/lib/apt/lists/*

# Pico SDK
RUN cd opt && \
    git clone https://github.com/raspberrypi/pico-sdk.git && \
    cd pico-sdk && \
    git submodule update --init

ENV PICO_SDK_PATH=/opt/pico-sdk

# Picotool
# see: https://github.com/raspberrypi/picotool/blob/master/README.md#usage-by-the-raspberry-pi-pico-sdk
RUN cd opt && \
    git clone https://github.com/raspberrypi/picotool && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make install

ENV PICOTOOL_FETCH_FROM_GIT_PATH=/opt/picotool

WORKDIR /code
