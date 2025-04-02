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

void setup(void);
void loop(void);

ISR(PCINT0_vect) {}

void setup()
{
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
}

void loop()
{
    for (uint8_t i = 0; i < 10; i++)
    {
        PORTB |= (1 << PB1);
        _delay_ms(50);
        PORTB &= ~(1 << PB1);
        _delay_ms(50);
    }

    zzz_sleep();
}

#ifndef ARDUINO
int main(void)
{
    setup();
    while (1)
    {
        loop();
    }
}
#endif
