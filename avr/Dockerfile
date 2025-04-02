FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    avr-libc \
    binutils-avr \
    gcc-avr \
    make \
    # optionally, install avrdude
    #avrdude \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /code
