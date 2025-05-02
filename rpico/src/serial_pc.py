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
import datetime
import sys
import os


COM_PORT = "COM6"


def main():
    ser = None
    try:
        ser = serial.Serial(port=COM_PORT,
            parity=serial.PARITY_EVEN,
            stopbits=serial.STOPBITS_ONE,
            timeout=1)

        ser.flush()

        print(f"Found Pico at {COM_PORT}.")
    except:
        sys.exit(f"NOT found any Pico at {COM_PORT}.")

    # Send ping message
    ser.write("ping".encode(encoding="ascii"))

    # Receive data until \n appears, or the timeout (set in Serial ctor) is hit
    rx_str = ser.readline().decode(encoding="ascii").rstrip(os.linesep)

    print(rx_str)

    # Send and receive date strings and compare
    while True:
        iso_time = datetime.datetime.now().isoformat()

        print(f"TX \"{iso_time}\"")

        # TX
        ser.write(iso_time.encode(encoding="ascii"))
        # RX
        rx_str = ser.readline().decode(encoding="ascii").rstrip(os.linesep)

        print(f"RX \"{rx_str}\" \
{"(RX == TX)" if iso_time == rx_str else "(RX != TX) ERROR"}")

        time.sleep(1)


if __name__ == "__main__":
    main()
