/*
Copyright (c) 2024 Alexander Scholz

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
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../ws2812b.h"
#include "../zzz.h"
#include "../timemeas.h"
#include "../debounce.h"

#define NUM_LED 24
#define TIME_TO_SLEEP 300000

typedef struct
{
    uint8_t *seq;
    uint8_t seq_len;
    uint8_t time_shift;
} mode;

const uint8_t palette[][3] = {
    {63, 0, 0},  // Green
    {63, 63, 0}, // Yellow
    {25, 63, 0}, // Orange
    {0, 63, 0},  // Red
    {0, 22, 22}, // Pink
    {16, 0, 39}, // Blue
    {6, 17, 36}, // Purple
    {22, 0, 22}  // Turquoise
};

uint8_t sequence_0[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
uint8_t sequence_1[] = {6, 6, 7, 7};
uint8_t sequence_2[] = {6, 6, 1, 1, 7, 7};
uint8_t sequence_3[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
uint8_t sequence_4[] = {3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6};

const mode modes[] = {
    {sequence_0, 12, 7},
    {sequence_2, 6, 9},
    {sequence_1, 4, 9},
    {sequence_3, 24, 5},
    {sequence_4, 12, 5}};

const uint8_t num_modes = sizeof(modes) / sizeof(modes[0]);

ISR(PCINT0_vect) {}

void prepare_sleep(void)
{
    for (uint8_t i = 0; i < NUM_LED; i++)
    {
        ws2812b_bang_byte(PB1, 0);
        ws2812b_bang_byte(PB1, 0);
        ws2812b_bang_byte(PB1, 0);
    }
}

int main(void)
{
    // Initialize time measure
    timemeas_init();

    // Set direction of pins PB1 to out
    DDRB |= (1 << DDB1); // Port B data direction register (DDRB)

    // Set direction of pin PB2 to in
    DDRB &= ~(1 << DDB2);

    // Activate pull-up resistor for pin PB2 (input).
    // This pulls the input of this pin to HIGH by default. On button press,
    // the pin must be connected to GROUND. A button press is then read as LOW
    PORTB |= (1 << PB2);

    cli();

    // Set Pin Change Interrupt Enable (PCIE)
    GIMSK |= (1 << PCIE); // General Interrupt Mask Register (GIMSK)

    // Set mask bit for pin PB2 to activate interrupt if PB2 changes
    PCMSK |= (1 << PCINT2); // Pin Change Mask Register (PCMSK)

    sei();

    // Initialize button debouncer with default values
    debouncer button_deb;
    debounce_init(&button_deb);

    // Frame counter
    uint32_t frame = 0;
    uint32_t frame_last = 0xffffffff;

    // Mode State
    uint8_t current_mode_idx = 0;

    // Time of last input for auto-sleep
    uint32_t last_input_time = timemeas_now();

    while (1)
    {
        // If moodlight runs for TIME_TO_SLEEP ms with no input, go to sleep
        if ((timemeas_now() - last_input_time) > TIME_TO_SLEEP)
        {
            last_input_time = timemeas_now();
            current_mode_idx = 0;

            prepare_sleep();
            // This delay prevents additional Pin Change Interrupts
            // (due to button bouncing/release) canceling the sleep
            _delay_ms(500);
            zzz_sleep();
        }

        // Calc frame from elapsed time
        frame = (timemeas_now() >> modes[current_mode_idx].time_shift);

        // Insert raw button state into the debouncer
        debounce_update((PINB & (1 << PINB2)) ? 0 : 1, &button_deb);

        // Handle state change on button down
        if (button_deb.state_changed && button_deb.state == 1)
        {
            last_input_time = timemeas_now();

            current_mode_idx = (current_mode_idx + 1) % num_modes;

            // If switched back to first mode, go to sleep
            if (current_mode_idx == 0)
            {
                prepare_sleep();
                // This delay prevents additional Pin Change Interrupts
                // (due to button bouncing/release) canceling the sleep
                _delay_ms(500);
                zzz_sleep();
            }
        }
        else if (frame == frame_last)
        {
            continue;
        }

        // New frame
        frame_last = frame;

        // Update LEDs
        // Bit-Banging. No fancy calculations in this loop!
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            uint8_t framei = ((uint8_t)(frame) + i) % modes[current_mode_idx].seq_len;

            ws2812b_bang_byte(PB1, palette[modes[current_mode_idx].seq[framei]][0]);
            ws2812b_bang_byte(PB1, palette[modes[current_mode_idx].seq[framei]][1]);
            ws2812b_bang_byte(PB1, palette[modes[current_mode_idx].seq[framei]][2]);
        }

        _delay_ms(10);
    }
}
