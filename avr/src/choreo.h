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
#ifndef CHOREO_H
#define CHOREO_H

#include <stdint.h>

// Mechanism for time-uncritical concurrent execution of simple tasks.
//
// 1) Define a choreo function, like this one:
//   uint8_t choreo_func_blink(uint8_t step_old, uint32_t time, const void *data)
// The function will be called by choreo_tick().
// The function gets the step, it previously returned (old_step), or CHOREO_RESET if it shall stop,
// or CHOREO_IDLE if it shall start.
//   If CHOREO_RESET, the function "cleans up" and returns CHOREO_IDLE.
// The function must calculate a new step from the accumulated time, since the choreo was started (time).
// The function performs something, based on the new step.
//   Usually, only if step != step_old, something is done.
// The function always returns the current (new) step, or CHOREO_IDLE if the choreo is finished.
//
// 2) Create a choreo instance, like so:
//   choreo choreo_blink;
//
// 3) Call choreo_init(), like so:
//   choreo_init(&choreo_blink, 1, 0, choreo_func_blink);
// Set loop > 0 if it shall loop.
// optionally, pass user data as void*
// The initial state will be CHOREO_IDLE.
//
// 4) Start the choreo if desired:
//   choreo_start(&choreo_blink);
// This will init the time. The choreo function will be called with step=CHOREO_IDLE.
//
// 5) Tick the choreo, if it is not idling:
//   if (choreo_blink.step != CHOREO_IDLE){ choreo_tick(&choreo_blink); }
//
// 6) Stop the choreo if desired:
//   choreo_stop(&choreo_blink);
// The choreo function will be called with step=CHOREO_RESET. It should return CHOREO_IDLE afterwards.
#define CHOREO_RESET 0xfe
#define CHOREO_IDLE 0xff

typedef struct
{
    uint32_t start_time;
    uint8_t step;
    uint8_t loop;
    const void *data;
    uint8_t (*func)(uint8_t step_old, uint32_t time, const void *data);
} choreo;

void choreo_init(choreo *cho,
                 uint8_t loop,
                 const void *data,
                 uint8_t (*func)(uint8_t step_old, uint32_t time, const void *data));

void choreo_start(choreo *cho);

void choreo_tick(choreo *cho);

void choreo_stop(choreo *cho);

#endif
