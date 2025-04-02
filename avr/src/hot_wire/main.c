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
#include <util/delay.h>
#include "../ws2812b.h"

#define STATE_IDLE 1
#define STATE_PLAYING 2
#define STATE_WON 3
#define STATE_LOST 4

void play_lost(void)
{
    ws2812b_bang_byte(PB3, 20);
    ws2812b_bang_byte(PB3, 0);
    ws2812b_bang_byte(PB3, 0);

    OCR1A = 6;
    OCR1C = 13;
    _delay_ms(250);
    OCR1C = 0;
    _delay_ms(100);
    OCR1A = 8;
    OCR1C = 17;
    _delay_ms(750);
    OCR1C = 0;
}

void play_won(void)
{
    ws2812b_bang_byte(PB3, 0);
    ws2812b_bang_byte(PB3, 20);
    ws2812b_bang_byte(PB3, 0);

    OCR1A = 4;
    OCR1C = 9;
    _delay_ms(250);
    OCR1C = 0;
    _delay_ms(100);
    OCR1A = 2;
    OCR1C = 5;
    _delay_ms(750);
    OCR1C = 0;
}

void play_start(void)
{
    ws2812b_bang_byte(PB3, 0);
    ws2812b_bang_byte(PB3, 0);
    ws2812b_bang_byte(PB3, 20);

    OCR1A = 4;
    OCR1C = 9;
    _delay_ms(100);
    OCR1C = 0;
    _delay_ms(100);
    OCR1C = 9;
    _delay_ms(100);
    OCR1C = 0;
}

int main(void)
{
    uint8_t state = STATE_IDLE;

    // Configure I/O directions
    // PB0: (in) Wire
    // PB1: (out) Buzzer PWM
    // PB2: (in) Start Pad
    // PB4: (in) Goal Pad
    // PB3: (out) RGB led
    DDRB = (1 << DDB1) | (1 << DDB3);

    // Pull-up for inputs
    PORTB |= (1 << DDB0) | (1 << DDB2) | (1 << DDB4);

    // Enable PWM for Buzzer
    // Counter/Timer1 PWM Mode and Prescaler /1024
    TCCR1 |= (1 << PWM1A) | (1 << COM1A1) | (1 << CS13) | (1 << CS11) | (1 << CS10);

    ws2812b_bang_byte(PB3, 20);
    ws2812b_bang_byte(PB3, 0);
    ws2812b_bang_byte(PB3, 20);

    while (1)
    {
        // State Machine
        switch (state)
        {
        case STATE_IDLE:
        case STATE_LOST:
        case STATE_WON:
            if ((PINB & (1 << PINB2)) == 0) // Hit start pad
            {
                play_start();
                state = STATE_PLAYING;
            }
            break;
        case STATE_PLAYING:
            if ((PINB & (1 << PINB0)) == 0) // Hit wire
            {
                play_lost();
                state = STATE_LOST;
            }
            if ((PINB & (1 << PINB4)) == 0) // Hit goal pad
            {
                play_won();
                state = STATE_WON;
            }
            break;
        }
    }
}
