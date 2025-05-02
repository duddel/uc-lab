# PC Moodlight

-   This is a moodlight, connected to your PC
-   It can probably go inside your PC case
-   Based on the Raspberry Pi Pico and a WS2812 LED Ring, controlled via USB
-   Running a Python script on the host PC, it reacts to active processes (such as games!)

## Build

Build directly: see [./build.bash](./build.bash)

Interactive container:

Run the root [Docker container](../../../README.md#docker-image) in the **Pico Lab root** mounted to `/code`.

```bash
docker run --rm -it -v $(pwd)/../..:/code uc-lab /bin/bash
```
