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
#include <avr/interrupt.h>
#include <avr/io.h>
#include "../timemeas.h"
#include "../debounce.h"

int main(void)
{
    // Set direction of pin PB1 to out
    DDRB |= (1 << DDB1); // Port B data direction register (DDRB)

    // Set direction of pin PB2 to in
    DDRB &= ~(1 << DDB2);

    // Activate pull-up resistor for pin PB2 (input).
    // This pulls the input of this pin to HIGH by default. On button press,
    // the pin must be connected to GROUND. A button press is then read as LOW
    PORTB |= (1 << PB2);

    // Initialize time measure
    timemeas_init();

    // Enable interrupts (for time measure)
    sei();

    // Initialize button debouncer with default values
    debouncer button_deb;
    debounce_init(&button_deb);

    // The current led on/off state
    uint8_t led_on = 1;

    while (1)
    {
        // If PINB2 is LOW, the button is pressed (button_state_raw = 1)
        uint8_t button_state_raw = (PINB & (1 << PINB2)) ? 0 : 1;

        // Insert raw button state into the debouncer
        debounce_update(button_state_raw, &button_deb);

        // If the (debounced) button state changed...
        if (button_deb.state_changed)
        {
            // Change led on/off state if button was pressed down
            if (button_deb.state)
            {
                led_on = (led_on + 1) % 2;
            }
        }

        // Set LED state to output pin
        if (led_on)
        {
            PORTB |= (1 << PB1);
        }
        else
        {
            PORTB &= ~(1 << PB1);
        }
    }
}
