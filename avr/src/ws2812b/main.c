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
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../ws2812b.h"

#define NUM_LED 24

const uint8_t palette[][3] = {
    {63, 0, 0},  // Green
    {63, 63, 0}, // Yellow
    {25, 63, 0}, // Orange
    {0, 63, 0},  // Red
    {0, 22, 22}, // Pink
    {16, 0, 39}, // Blue
};

const uint8_t sequence[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
const uint8_t len_sequence = 12;

int main(void)
{
    DDRB |= (1 << DDB1); // Port B data direction register (DDRB)

    uint8_t sequence0 = 0;
    while (1)
    {
        // Bit-Banging. No fancy calculations in this loop!
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            uint8_t seq = sequence[(sequence0 + i) % len_sequence];
            ws2812b_bang_byte(PB1, palette[seq][0]);
            ws2812b_bang_byte(PB1, palette[seq][1]);
            ws2812b_bang_byte(PB1, palette[seq][2]);
        }

        sequence0 = (sequence0 + 1) % len_sequence;
        _delay_ms(100);
    }
}
