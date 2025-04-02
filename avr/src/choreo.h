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

#define CHOREO_RESET 0xfe
#define CHOREO_IDLE 0xff

typedef struct
{
    uint32_t start_time;
    uint8_t step;
    uint8_t loop;
    uint8_t (*func)(uint8_t step_old, uint32_t time);
} choreo;

void choreo_init(choreo *cho, uint8_t loop, uint8_t (*func)(uint8_t step_old, uint32_t time));

void choreo_start(choreo *cho);

void choreo_tick(choreo *cho);

void choreo_stop(choreo *cho);

#endif
