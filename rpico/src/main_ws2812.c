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
#include <math.h>
#include "pico/stdlib.h"
#include "ws2812.pio.h" // generated from ws2812.pio

#define NUM_LED 24           // Number of LEDs
#define LED_CHANNEL_FORMAT 0 // 0: GRB, 1: RGB
const uint data_pin = 0;     // TX data pin index
const PIO pio_idx = pio0;    // PIO instance
const int pio_sm = 0;        // PIO state machine index

uint32_t dimming[256];    // Lookup for LED channel dimming
uint32_t pixels[NUM_LED]; // Pixel data to transmit

void send_pixel(uint32_t pixel)
{
    pio_sm_put_blocking(pio_idx, pio_sm, pixel);
}

uint32_t make_pixel(uint32_t r, uint32_t g, uint32_t b)
{
#if LED_CHANNEL_FORMAT == 0 // GRB
    return (dimming[r] << 16) | (dimming[g] << 24) | (dimming[b] << 8);
#else // RGB
    return (dimming[r] << 24) | (dimming[g] << 16) | (dimming[b] << 8);
#endif
}

// creates a lookup table to make perceived LED colors more linear.
// also shifts the final value right to actually dim.
// example values: exp = 2.5, shift = 5
void init_dimming(float exp, uint8_t shift)
{
    for (int i = 0; i < 256; i++)
    {
        dimming[i] = (uint32_t)(powf(i / 255.f, exp) * 255.f) >> shift;
    }
}

int main()
{
    // Enable stdio via USB. Required for loading a program via picotool,
    // but required for this program itself.
    stdio_usb_init();

    init_dimming(2.5f, 5);

    uint offset = pio_add_program(pio_idx, &ws2812_program);

    // 1/800000Hz = 1.25us, which is the typical duration of a ws2812 bit signal
    ws2812_program_init(pio_idx, pio_sm, offset, data_pin, 800000, false);

    uint32_t palette[] = {
        make_pixel(255, 0, 0),
        make_pixel(255, 127, 0),
        make_pixel(255, 255, 0),
        make_pixel(0, 255, 0),
        make_pixel(102, 153, 204),
        make_pixel(150, 0, 210)};

    uint32_t sequence[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    int len_sequence = sizeof(sequence) / sizeof(sequence[0]);

    int sequence0 = 0;
    while (true)
    {
        for (int i = 0; i < NUM_LED; i++)
        {
            pixels[i] = palette[sequence[(sequence0 + i) % len_sequence]];
        }

        for (int i = 0; i < NUM_LED; i++)
        {
            send_pixel(pixels[i]);
        }

        sequence0 = (sequence0 + 1) % len_sequence;

        sleep_ms(100);
    }
}
