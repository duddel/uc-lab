# Copyright (c) 2023-2025 Alexander Scholz

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
import serial
import time
import psutil
import sys


VERBOSE = False
COM_PORT = "COM6"
IDLE_MODE = "off"
SLEEP_TIME = 1 # [sec]

PROCESS_MODES = {
    "hl.exe": "orange",
    "bms.exe": "orange",
    "hl2.exe": "orange"
}


def main():
    print(f"Initializing serial on {COM_PORT}...")

    try:
        ser = serial.Serial(port=COM_PORT,
                            parity=serial.PARITY_EVEN,
                            stopbits=serial.STOPBITS_ONE,
                            timeout=1)
        ser.flush()
    except serial.serialutil.SerialException as e:
        sys.exit(f"Goodbye. {e}")

    print("Success.")

    current_mode = IDLE_MODE
    next_mode = IDLE_MODE

    # Send initial mode
    ser.write(current_mode.encode(encoding="ascii"))

    print("Watching processes...")
    try:
        while True:
            # Get names of running processes
            processes = psutil.process_iter()
            proc_names = [p.name() for p in processes]

            if VERBOSE:
                print(f"Got {len(proc_names)} running processes.")

            for p, m in PROCESS_MODES.items():
                if p in proc_names:
                    next_mode = m
                    if VERBOSE:
                        print(f"Found process {p}, mode: {m}")
                    break
            else:
                next_mode = IDLE_MODE
                if VERBOSE:
                    print(f"No process of interest found.")

            if current_mode != next_mode:
                print(f"Switching mode: {current_mode} -> {next_mode}")
                ser.write(next_mode.encode(encoding="ascii"))
                current_mode = next_mode

            time.sleep(SLEEP_TIME)
    except KeyboardInterrupt:
        print("Goodbye. (KeyboardInterrupt)")


if __name__ == "__main__":
    main()
