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
#include "../zzz.h"
#include "../timemeas.h"
#include "../debounce.h"

#define STATE_SLOW 1
#define STATE_FAST 2
#define STATE_PERMANENT 3

#define TOGGLE_DELAY_FAST 50
#define TOGGLE_DELAY_SLOW 250

ISR(PCINT0_vect) {}

int main(void)
{
    // Initial state
    uint8_t state = STATE_SLOW;

    // Initialize time measure
    timemeas_init();

    // Set direction of pin PB1 to out
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

    // Time of last toggle
    uint32_t lastToggleTime = timemeas_now();

    while (1)
    {
        // Insert raw button state into the debouncer
        debounce_update((PINB & (1 << PINB2)) ? 0 : 1, &button_deb);

        // Handle state change on button down
        if (button_deb.state_changed && button_deb.state == 1)
        {
            switch (state)
            {
            case STATE_SLOW:
                state = STATE_FAST;
                break;
            case STATE_FAST:
                state = STATE_PERMANENT;
                PORTB |= (1 << PB1);
                break;
            case STATE_PERMANENT:
                PORTB &= ~(1 << PB1);
                // This delay prevents additional Pin Change Interrupts
                // (due to button bouncing/release) canceling the sleep
                _delay_ms(500);
                zzz_sleep();
                // After wakeup, go to initial state
                state = STATE_SLOW;
                break;
            }
        }

        // Perform state specific logic
        switch (state)
        {
        case STATE_SLOW:
            if (timemeas_now() - lastToggleTime > TOGGLE_DELAY_SLOW)
            {
                PORTB ^= (1 << PB1);
                lastToggleTime = timemeas_now();
            }
            break;
        case STATE_FAST:
            if (timemeas_now() - lastToggleTime > TOGGLE_DELAY_FAST)
            {
                PORTB ^= (1 << PB1);
                lastToggleTime = timemeas_now();
            }
            break;
        }
    }
}
