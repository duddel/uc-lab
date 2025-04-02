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
#include <avr/io.h>

// Select PWM example below
//  0: PWM on PB0 using Counter/Timer0 (488Hz, 25% duty cycle)
//    _____               ___
//    |   |_______________|  ...
//    0   64             255      Counter
//
//  1: PWM on PB1 using Counter/Timer1
//    ________       ___
//    |      |_______|  ...
//    0    OCR1A   OCR1C     Counter
//
//  2: ToDo: Output Compare Interrupt Example
#define AVR_LAB_PWM_EXAMPLE 1

void init_pwm(void)
{
#if AVR_LAB_PWM_EXAMPLE == 0
    // Timer/Counter0 Control Register A (TCCR0A)
    // WGM00, WGM01:
    //   Activate Fast PWM mode (counting to 0xFF)
    // COM0A1:
    //   Clear OC0A (Output Compare Pin A -> PB0) on Compare Match
    //   (when counter reaches value of OCR0A). Set OC0A (PB0) at
    //   BOTTOM (after overflow).
    // - For inverted mode: set COM0A0 AND COM0A1
    // - For 2nd (independent) PWM channel on OC0B (PB1), set COM0B0
    //   and COM0B1 and use OCR0B for comparison
    TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1);

    // Timer/Counter0 Control Register B (TCCR0B)
    // Set Clock Select: Prescaler (I/O Clock divisor)
    // -> Counting at 125kHz -> Overflow at 125000 / 256 Hz = 488.28Hz
    // -> Overflow every 2.05ms
    // CS00:
    //   divide by 8
    // CS00, CS01:
    //   divide by 64
#if F_CPU == 1000000
    TCCR0B |= (1 << CS01);
#elif F_CPU == 8000000
    TCCR0B |= (1 << CS00) | (1 << CS01);
#else
#error PWM example is only configured for an I/O Clock of 1MHz or 8MHz
#endif

    // Output Compare Register A (OCR0A)
    // Clear OC0A after 64 cycles (25% duty cycle)
    OCR0A = 64;

#elif AVR_LAB_PWM_EXAMPLE == 1
    // Timer/Counter1 Control Register (TCCR1)
    // PWM1A:
    //   Pulse Width Modulator A Enable ("PWM Mode")
    //   PWM based on compare OCR1A and counter reset based on Output Compare Register C (OCR1C)
    //   - (A 2nd, independent PWM channel, comparing OCR1B, is also available)
    // COM1A1 | COM1A0 (Comparator A Output Mode)
    //    0        0   No Pins connected
    //    0        1   Clear OC1A (PB1) on Compare Match (counted to OCR1A)
    //                 Additional (inverted) PWM on PB0
    //    1        0   Clear OC1A (PB1) on Compare Match (counted to OCR1A)
    //    1        1   Set OC1A (PB1) on Compare Match (counted to OCR1A)
    TCCR1 |= (1 << PWM1A) | (1 << COM1A1);

    // Set prescaler to count at 7.8125kHz
#if F_CPU == 1000000
    TCCR1 |= (1 << CS13); // Prescale /128
#elif F_CPU == 8000000
    TCCR1 |= (1 << CS13) | (1 << CS11) | (1 << CS10); // Prescale /1024
#else
#error PWM example is only configured for an I/O Clock of 1MHz or 8MHz
#endif

    // Now, control frequency and duty cycle with OCR1A and OCR1C
    // Examples:

    // 30.5Hz (= 7812.5Hz / 256) square wave
    // OCR1A = 128;
    // OCR1C = 255;

    // 50Hz (= 7812.5Hz / 156) square wave
    OCR1A = 77;
    OCR1C = 155;

    // 558Hz (= 7812.5Hz / 14) square wave
    // OCR1A = 6;
    // OCR1C = 13;
#endif
}

int main(void)
{
    DDRB |= (1 << DDB0) | (1 << DDB1);

    init_pwm();

    while (1)
    {
    }
}
