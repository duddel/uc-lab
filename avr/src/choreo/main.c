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
#include <avr/interrupt.h>
#include <avr/io.h>
#include "../timemeas.h"
#include "../choreo.h"
#include "../debounce.h"

uint8_t choreo_func_blink(uint8_t step_old, uint32_t time, const void *data)
{
    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        PORTB &= ~(1 << PB1);
        return CHOREO_IDLE;
    }

    // Increment step by time
    uint8_t step = (uint8_t)(time >> 8);
    if (step == step_old)
    {
        return step;
    }

    // Handle step increment
    switch (step)
    {
    case 0:
        PORTB |= (1 << PB1);
        return step;
    case 1:
        PORTB &= ~(1 << PB1);
        return step;
    case 2:
    default:
        return CHOREO_IDLE; // finish choreo
    }
}

uint8_t choreo_func_morse(uint8_t step_old, uint32_t time, const void *data)
{
    // ['t', 'i', 'n', 'y'] (waste of memory)
    static const uint8_t signal[] = {1, 1, 1, 0, 0, 0,
                                     1, 0, 1, 0, 0, 0,
                                     1, 1, 1, 0, 1, 0, 0, 0,
                                     1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    static const uint8_t signal_size = sizeof(signal) / sizeof(signal[0]);

    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        PORTB &= ~(1 << PB2);
        return CHOREO_IDLE;
    }

    // Increment step by time
    uint8_t step = (uint8_t)(time >> 7);
    if (step == step_old)
    {
        return step;
    }

    if (step >= signal_size)
    {
        return CHOREO_IDLE; // finish choreo
    }
    else if (signal[step])
    {
        PORTB |= (1 << PB2);
    }
    else
    {
        PORTB &= ~(1 << PB2);
    }

    return step;
}

int main(void)
{
    debouncer button_deb_blink;
    debouncer button_deb_morse;
    debounce_init(&button_deb_blink);
    debounce_init(&button_deb_morse);

    choreo choreo_blink;
    choreo choreo_morse;

    timemeas_init();
    sei();

    // Init blink choreo: loop=1
    choreo_init(&choreo_blink, 1, 0, choreo_func_blink);
    // Init Morse choreo: perform once (loop=0)
    choreo_init(&choreo_morse, 0, 0, choreo_func_morse);

    // PB1, PB2: out
    DDRB |= (1 << DDB1) | (1 << DDB2);

    // PB3, PB4: in
    DDRB &= ~((1 << DDB3) | (1 << DDB4));

    // pull-up resistors
    PORTB |= (1 << PB3) | (1 << PB4);

    while (1)
    {
        if (choreo_blink.step != CHOREO_IDLE)
        {
            choreo_tick(&choreo_blink);
        }

        if (choreo_morse.step != CHOREO_IDLE)
        {
            choreo_tick(&choreo_morse);
        }

        // Toggle blink choreo
        debounce_update((PINB & (1 << PINB3)) ? 0 : 1, &button_deb_blink);
        if (button_deb_blink.state_changed && button_deb_blink.state == 1)
        {
            if (choreo_blink.step == CHOREO_IDLE)
            {
                choreo_start(&choreo_blink);
            }
            else
            {
                choreo_stop(&choreo_blink);
            }
        }

        // Toggle Morse choreo
        debounce_update((PINB & (1 << PINB4)) ? 0 : 1, &button_deb_morse);
        if (button_deb_morse.state_changed && button_deb_morse.state == 1)
        {
            if (choreo_morse.step == CHOREO_IDLE)
            {
                choreo_start(&choreo_morse);
            }
            else
            {
                choreo_stop(&choreo_morse);
            }
        }
    }
}
