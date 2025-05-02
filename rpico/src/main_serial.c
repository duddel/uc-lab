/*
Copyright (c) 2023-2025 Alexander Scholz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#define RX_BUF_LEN 128

// End of Line character for messages to send via printf().
// The RX side can read until this character and interpret this as a line.
// '\x0A' is ASCII Line Feed (LF) ('\n'). One could also put \n at the end of the printf().
const char serial_eol = '\x0A';

int main()
{
    stdio_usb_init();

    char rxBuf[RX_BUF_LEN];
    
    while (1)
    {
        size_t writeIdx = 0;
        int rxChar;

        // getchar_timeout_us() returns 0-255 or PICO_ERROR_TIMEOUT as int.
        // There is also getchar(), which is blocking without timeout.
        while ((rxChar = getchar_timeout_us(100)) != PICO_ERROR_TIMEOUT &&
               writeIdx < (RX_BUF_LEN - 1))
        {
            rxBuf[writeIdx++] = (char)rxChar;
        }

        // Append null terminator to handle RX data as a string
        rxBuf[writeIdx] = '\0';

        if (writeIdx > 0) // received something?
        {
            // ping -> reply with a pong message
            if (!strcmp(rxBuf, "ping"))
            {
                printf("PICO PONG%c", serial_eol);
            }
            // or reply the received data unchanged
            else
            {
                printf("%s%c", rxBuf, serial_eol);
            }
        }
    }
}
