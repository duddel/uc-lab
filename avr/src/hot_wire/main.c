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
#include <avr/interrupt.h>
#include "../ws2812b.h"
#include "../timemeas.h"
#include "../choreo.h"
#include "../zzz.h"

// Config
#define NUM_LED 10
#define TIME_UNTIL_SLEEP 90000 // [ms] Go to sleep mode if no input for TIME_UNTIL_SLEEP ms

// Stages
#define STATE_IDLE 1
#define STATE_PLAYING 2
#define STATE_WON 3
#define STATE_LOST 4

// Audio
// ToDo: check for F_CPU == 8000000 and assume PRESCALER from F_CPU
#define PRESCALER 1024
#define OCR1C_FROM_FREQ(freq) ((F_CPU / (2UL * PRESCALER * (uint32_t)(freq))) - 1)

typedef struct
{
    uint8_t ctr_top;   // top value for OCR1C
    uint32_t duration; // [ms]
} cnote_t;

// Melody: start
const cnote_t melody_start[] = {
    {9, 100},
    {0, 100},
    {9, 100},
    {0xff, 0} // END
};

// Melody: success
const cnote_t melody_success[] = {
    {OCR1C_FROM_FREQ(523), 250},
    {OCR1C_FROM_FREQ(659), 250},
    {OCR1C_FROM_FREQ(784), 250},
    {OCR1C_FROM_FREQ(1046), 250},
    {OCR1C_FROM_FREQ(784), 250},
    {OCR1C_FROM_FREQ(659), 250},
    {OCR1C_FROM_FREQ(523), 250},
    {OCR1C_FROM_FREQ(392), 500},
    {OCR1C_FROM_FREQ(440), 250},
    {OCR1C_FROM_FREQ(494), 250},
    {OCR1C_FROM_FREQ(523), 500},
    {0xff, 0} // END
};

// Melody: lost
const cnote_t melody_lost[] = {
    {13, 250},
    {0, 100},
    {17, 750},
    {0xff, 0} // END
};

// ToDo: merge the choreo light functions, pass color data via data

uint8_t choreo_func_lost_light(uint8_t step_old, uint32_t time, const void *data)
{
    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
        }
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
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 255);
            ws2812b_bang_byte(PB3, 0);
        }
        return step;
    case 1:
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
        }
        return step;
    case 2:
    default:
        return CHOREO_IDLE; // finish choreo
    }
}

uint8_t choreo_func_start_light(uint8_t step_old, uint32_t time, const void *data)
{
    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
        }
        return CHOREO_IDLE;
    }

    // Increment step by time
    // this will loop the choreo forever (% NUM_LED)
    uint8_t step = (uint8_t)(time >> 6) % NUM_LED;
    if (step == step_old)
    {
        return step;
    }

    uint8_t pos = NUM_LED - 1 - step;

    // Bit-Banging. No fancy calculations in this loop!
    for (uint8_t i = 0; i < NUM_LED; i++)
    {
        if (i == pos)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 255);
            ws2812b_bang_byte(PB3, 255);
        }
        else
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
        }
    }

    return step;
}

uint8_t choreo_func_success_light(uint8_t step_old, uint32_t time, const void *data)
{
    const uint8_t palette[][3] = {
        {63, 0, 0},  // Green
        {63, 63, 0}, // Yellow
        {25, 63, 0}, // Orange
        {0, 63, 0},  // Red
        {0, 22, 22}, // Pink
        {16, 0, 39}  // Blue
    };

    const uint8_t sequence[] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    const uint8_t len_sequence = 12;

    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        for (uint8_t i = 0; i < NUM_LED; i++)
        {
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
            ws2812b_bang_byte(PB3, 0);
        }
        return CHOREO_IDLE;
    }

    // Increment step by time
    // this will loop the choreo forever (% NUM_LED)
    uint8_t step = (uint8_t)(time >> 7) % NUM_LED;
    if (step == step_old)
    {
        return step;
    }

    uint8_t sequence0 = step % len_sequence;

    // Bit-Banging. No fancy calculations in this loop!
    for (uint8_t i = 0; i < NUM_LED; i++)
    {
        uint8_t seq = sequence[(sequence0 + i) % len_sequence];
        ws2812b_bang_byte(PB3, palette[seq][0]);
        ws2812b_bang_byte(PB3, palette[seq][1]);
        ws2812b_bang_byte(PB3, palette[seq][2]);
    }

    return step;
}

uint8_t choreo_func_melody(uint8_t step_old, uint32_t time, const void *data)
{
    // Assuming data points to a melody array
    const cnote_t *melody = data;

    // Reset requested: Go to idle state
    if (step_old == CHOREO_RESET)
    {
        OCR1A = 0;
        OCR1C = 0;
        return CHOREO_IDLE;
    }

    // Calculate step based on elapsed time and accumulated time of melody notes
    uint32_t accumulated_time = 0;
    uint8_t current_step = 0;

    for (;;)
    {
        const cnote_t *note = &melody[current_step];

        if (note->ctr_top == 0xff)
        {
            OCR1A = 0;
            OCR1C = 0;
            return CHOREO_IDLE;
        }

        if (time < accumulated_time + note->duration)
        {
            break;
        }

        accumulated_time += note->duration;
        current_step++;
    }

    if (current_step != step_old)
    {
        const cnote_t *note = &melody[current_step];

        // ToDo: this might not lead to the actual frequency
        OCR1C = note->ctr_top;
        OCR1A = note->ctr_top >> 1; // to from HI to LOW half way counting up -> 50% duty cycle -> square wave
    }

    return current_step;
}

choreo choreo_light_start;
choreo choreo_light_lost;
choreo choreo_light_success;
choreo choreo_melody_start;
choreo choreo_melody_lost;
choreo choreo_melody_success;

void tick_all_choreos(void)
{
    // ToDo: move the step != CHOREO_IDLE check into choreo_tick()?
    // ToDo: rename CHOREO_IDLE to CHOREO_STOPPED?
    if (choreo_light_start.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_light_start);
    }

    if (choreo_light_lost.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_light_lost);
    }

    if (choreo_light_success.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_light_success);
    }

    if (choreo_melody_start.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_melody_start);
    }

    if (choreo_melody_lost.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_melody_lost);
    }

    if (choreo_melody_success.step != CHOREO_IDLE)
    {
        choreo_tick(&choreo_melody_success);
    }
}

void stop_all_choreos(void)
{
    // ToDo: change choreo_stop() to only call the choreo function with CHOREO_RESET if it is not stopped already
    choreo_stop(&choreo_light_start);
    _delay_ms(2);
    choreo_stop(&choreo_light_lost);
    _delay_ms(2);
    choreo_stop(&choreo_light_success);
    _delay_ms(2);
    choreo_stop(&choreo_melody_start);
    _delay_ms(2);
    choreo_stop(&choreo_melody_lost);
    _delay_ms(2);
    choreo_stop(&choreo_melody_success);
    _delay_ms(2);
}

// Empty ISR for the pin change interrupt
ISR(PCINT0_vect) {}

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

    cli();

    // Set Pin Change Interrupt Enable (PCIE)
    GIMSK |= (1 << PCIE); // General Interrupt Mask Register (GIMSK)

    // Set mask bit for pin PB2 to activate interrupt if PB2 changes
    PCMSK |= (1 << PCINT2); // Pin Change Mask Register (PCMSK)

    // Enable time measure and interrupts
    timemeas_init();
    sei();

    // Initialize choreos
    choreo_init(&choreo_light_start, 1, 0, choreo_func_start_light);
    choreo_init(&choreo_light_lost, 1, 0, choreo_func_lost_light);
    choreo_init(&choreo_light_success, 0, 0, choreo_func_success_light);
    choreo_init(&choreo_melody_start, 0, melody_start, choreo_func_melody);
    choreo_init(&choreo_melody_lost, 0, melody_lost, choreo_func_melody);
    choreo_init(&choreo_melody_success, 0, melody_success, choreo_func_melody);

    // Time on which the last input occured.
    // Used to go into sleep mode if no one is playing
    uint32_t last_input_time = timemeas_now();

    while (1)
    {
        tick_all_choreos();

        // State Machine
        switch (state)
        {
        case STATE_IDLE:
        // ToDo: maybe add choreos for idle state
        case STATE_LOST:
        case STATE_WON:
            if ((PINB & (1 << PINB2)) == 0) // Hit start pad
            {
                stop_all_choreos();
                choreo_start(&choreo_light_start);
                choreo_start(&choreo_melody_start);
                state = STATE_PLAYING;
                last_input_time = timemeas_now();
            }
            break;
        case STATE_PLAYING:
            if ((PINB & (1 << PINB0)) == 0) // Hit wire
            {
                stop_all_choreos();
                choreo_start(&choreo_light_lost);
                choreo_start(&choreo_melody_lost);
                state = STATE_LOST;
                last_input_time = timemeas_now();
            }
            if ((PINB & (1 << PINB4)) == 0) // Hit goal pad
            {
                stop_all_choreos();
                choreo_start(&choreo_light_success);
                choreo_start(&choreo_melody_success);
                state = STATE_WON;
                last_input_time = timemeas_now();
            }
            break;
        }

        // Go to sleep if no one is playing
        if (timemeas_now() - last_input_time > TIME_UNTIL_SLEEP)
        {
            stop_all_choreos();
            // This delay prevents additional Pin Change Interrupts
            // (due to button bouncing/release) canceling the sleep
            _delay_ms(500);
            zzz_sleep();
            state = STATE_IDLE;
            last_input_time = timemeas_now();
        }
    }
}
