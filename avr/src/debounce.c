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
#include "timemeas.h"
#include "debounce.h"

void debounce_init(debouncer *deb)
{
    deb->state = 0;
    deb->last_state = 0;
    deb->state_changed = 0;
    deb->delay_time = 50;
    deb->last_change_time = 0;
}

void debounce_update(uint8_t state, debouncer *deb)
{
    uint32_t now = timemeas_now();

    deb->state_changed = 0;

    if (state != deb->state &&
        (now - deb->last_change_time) > deb->delay_time)
    {
        deb->last_state = deb->state;
        deb->state = state;
        deb->state_changed = 1;
        deb->last_change_time = now;
    }
}
