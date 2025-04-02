FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    python3 \
    git \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && rm -rf /var/lib/apt/lists/*

RUN cd opt && \
    git clone https://github.com/raspberrypi/pico-sdk.git && \
    cd pico-sdk && \
    git submodule update --init

ENV PICO_SDK_PATH=/opt/pico-sdk
