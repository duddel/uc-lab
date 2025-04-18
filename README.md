# uC Lab

[![build rpico](https://github.com/duddel/uc-lab/actions/workflows/build_rpico.yml/badge.svg?branch=main)](https://github.com/duddel/uc-lab/actions/workflows/build_rpico.yml)
[![build avr](https://github.com/duddel/uc-lab/actions/workflows/build_avr.yml/badge.svg?branch=main)](https://github.com/duddel/uc-lab/actions/workflows/build_avr.yml)

This repository contains

-   Template and Examples in `C/C++` for the [:arrow_down: Raspberry Pi Pico](#raspberry-pi-pico-lab) using the official SDK, via Docker
-   Template and Examples in `C` for [:arrow_down: AVR Microcontrollers](#avr-lab), optionally built via Docker

## License

The source code in this repository is licensed under the [MIT license](LICENSE.txt) (if not stated otherwise). For Licenses of thirdparty components, see below.

### Thirdparty

-   Raspberry Pi Pico ws2812 PIO driver ([rpico/src/ws2812.pio](rpico/src/ws2812.pio))
    -   [rpico/src/ws2812.pio](rpico/src/ws2812.pio) (license in file)
-   AVR Makefile template ([avr/Makefile_template](avr/Makefile_template))
    -   origin: `WinAVR-20100110`
    -   license: see [avr/Makefile_template](avr/Makefile_template)
    -   each AVR example project in [avr/src/](avr/src/) contains a `Makefile`, which is an adjusted copy of [avr/Makefile_template](avr/Makefile_template)

# Raspberry Pi Pico Lab

-   The official SDK is cloned into the Docker image, see [rpico/Dockerfile](rpico/Dockerfile) for details
-   Check out the examples in [rpico/src/](rpico/src/)
-   Utilize existing code for an own project or start with a basic example
-   Having Docker in place, the projects can be built with a single command

## Features

| Feature  | Example                                                | Info                        | Tested on boards |
| -------- | ------------------------------------------------------ | --------------------------- | ---------------- |
| blink    | [rpico/src/main_blink.c](rpico/src/main_blink.c)       | Hello world blink example   | `pico`, `pico_w` |
| debounce | [rpico/src/main_debounce.c](rpico/src/main_debounce.c) | Button debouncer            | `pico`, `pico_w` |
| ws2812   | [rpico/src/main_ws2812.c](rpico/src/main_ws2812.c)     | Basic ws2812 light controls | `pico`, `pico_w` |

## Kick-start

A Docker installation and basic Docker knowledge is required.

1.  Build the Docker image:

```bash
cd rpico
docker build -t rpico .
```

2.  Build the examples via Docker with one of these commands. Replace `<pico_board>` with the desired board to build for, such as `pico` or `pico_w`.

```bash
cd rpico

# Bash
docker run --rm -v $(pwd):/code -w /code rpico /bin/bash ./build.bash <pico_board>

# Powershell
docker run --rm -v ${PWD}:/code -w /code rpico /bin/bash ./build.bash <pico_board>
```

3.  Drop any UF2 binary from `build_<pico_board>/` directory onto the Pico following the official documentation.

## Advanced usage

The above should suffice for simple building of the examples. Here are some more things to do.

**Use this repository as a template for an own project:**

-   Fork or download this repository
-   Delete unneeded source files from [rpico/src/](rpico/src/)
    -   e.g. just keep `main_blink.c` to start with
-   Delete unneeded build targets from [rpico/CMakeLists.txt](rpico/CMakeLists.txt)
    -   Each `main_[...].c/.cpp` file in [rpico/src/](rpico/src/) has a corresponding block in [rpico/CMakeLists.txt](rpico/CMakeLists.txt) defining an executable target
    -   e.g. just keep the block starting with `add_executable(blink ...`

**More control over the build command:**

```bash
cd rpico

# Bash
docker run --rm -v $(pwd):/code -w /code rpico /bin/bash -c "mkdir -p build && cd build && cmake -DPICO_BOARD=pico .. && make"

# Powershell
docker run --rm -v ${PWD}:/code -w /code rpico /bin/bash -c "mkdir -p build && cd build && cmake -DPICO_BOARD=pico .. && make"
```

**Start Docker container with an interactive shell:**

```bash
cd rpico

# Bash
docker run --rm -it -v $(pwd):/code rpico

# Powershell
docker run --rm -it -v ${PWD}:/code rpico
```

# AVR Lab

-   Using default `gcc-avr` and `avr-libc`, see [avr/Dockerfile](avr/Dockerfile) for details
-   Mainly written for `ATtiny85`, if not stated otherwise
-   Check out the examples in [avr/src/](avr/src/)
-   Utilize existing code for an own project or start with a basic example
-   Having Docker in place, the projects can be built with a single command

## Examples and Helper Modules

Example projects, roughly sorted by complexity.

| Example      | Code                                                       | Info                                                     |
| ------------ | ---------------------------------------------------------- | -------------------------------------------------------- |
| blink        | [avr/src/blink/main.c](avr/src/blink/main.c)               | Hello world blink example                                |
| timemeas     | [avr/src/timemeas/main.c](avr/src/timemeas/main.c)         | No delay blink example using time measure                |
| togglebutton | [avr/src/togglebutton/main.c](avr/src/togglebutton/main.c) | Debounced button input                                   |
| states       | [avr/src/states/main.c](avr/src/states/main.c)             | Simple state logic with debounced button input           |
| choreo       | [avr/src/choreo/main.c](avr/src/choreo/main.c)             | Concurrent pin output sequences                          |
| sleep_pci    | [avr/src/sleep_pci/main.c](avr/src/sleep_pci/main.c)       | Sleep and wake-up via pin change interrupt               |
| pwm          | [avr/src/pwm/main.c](avr/src/pwm/main.c)                   | PWM signal with Timer/Counter0,1                         |
| ws2812b      | [avr/src/ws2812b/main.c](avr/src/ws2812b/main.c)           | WS2812B control via Bit-Banging                          |
| moodlight    | [avr/src/moodlight/main.c](avr/src/moodlight/main.c)       | Multiple WS2812B light sequences, switchable, auto-sleep |
| hot_wire     | [avr/src/hot_wire/main.c](avr/src/hot_wire/main.c)         | Simple hot wire game                                     |

Helper modules.

| Module   | API                                      | Code                                     | Info                                                 |
| -------- | ---------------------------------------- | ---------------------------------------- | ---------------------------------------------------- |
| choreo   | [avr/src/choreo.h](avr/src/choreo.h)     | [avr/src/choreo.c](avr/src/choreo.c)     | Time-uncritical concurrent execution of simple tasks |
| debounce | [avr/src/debounce.h](avr/src/debounce.h) | [avr/src/debounce.c](avr/src/debounce.c) | Button debouncer                                     |
| timemeas | [avr/src/timemeas.h](avr/src/timemeas.h) | [avr/src/timemeas.c](avr/src/timemeas.c) | Time measurement using Timer/Counter0                |
| ws2812b  | [avr/src/ws2812b.h](avr/src/ws2812b.h)   | [avr/src/ws2812b.c](avr/src/ws2812b.c)   | WS2812B interface                                    |
| zzz      | [avr/src/zzz.h](avr/src/zzz.h)           | [avr/src/zzz.c](avr/src/zzz.c)           | Power down sleep mode                                |

## Set up Docker

A Docker installation and basic Docker knowledge is required. Run these commands in the repository root.

First, build the Docker image (once).

```bash
cd avr
docker build -t avrlab .
```

### Build a project

This builds a project in an auto-removing container, from the image created above. **Recommended in most cases.**

```bash
cd avr

# Bash
docker run --rm -v $(pwd):/code avrlab /bin/bash -c "cd src/blink && make hex"

# Powershell
docker run --rm -v ${PWD}:/code avrlab /bin/bash -c "cd src/blink && make hex"
```

### Advanced (more Control over Container)

Start container with interactive shell.

```bash
cd avr

# Option 1: Auto-removing container after usage
# Bash
docker run --rm -it -v $(pwd):/code avrlab

# Powershell
docker run --rm -it -v ${PWD}:/code avrlab

# Option 2: Named container
# Bash
docker run -it -v $(pwd):/code --name avrlab_01 avrlab

# Powershell
docker run -it -v ${PWD}:/code --name avrlab_01 avrlab

# For option 2, start container like this
docker start -i avrlab_01
```

Build a project.

```bash
cd src/blink
make hex
```

## avrdude Example Commands

-   Install `avrdude` on host (recommended)
-   Use `stk500v1` compatible ISP (e.g. using Arduino with ISP example sketch)

### Program

```bash
avrdude -p attiny85 -P com3 -c stk500v1 -b 19200 -U flash:w:src/blink/main.hex
```

### Fuses

>   **Attention:** inappropriate fuse settings can brick the microcontroller, check **[1]**

```bash
# Set 1MHz clock
avrdude -p attiny85 -P com3 -c stk500v1 -b 19200 -U lfuse:w:0x62:m

# Set 8MHz clock
avrdude -p attiny85 -P com3 -c stk500v1 -b 19200 -U lfuse:w:0xE2:m
```

## References

**[1]** https://www.engbedded.com/fusecalc/
