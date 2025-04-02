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
#include <stdint.h>

/*
This implementation assumes that the LOW part (always 2nd part) of a data bit
is allowed to stay LOW for (much) longer than the protocol asks for, as long
as it stays below the RESET time.

For any LED device meeting this assumption, this function can be called in
a loop (fast), sending individual bytes of color data.

WS2812B Timings:
  zero: 400ns HI, 850ns LO (1.25us)
  one:  800ns HI, 450ns LO (1.25us)
  reset: > 50us

APA106 Timings:
  zero: 350ns HI, 1360ns LO (1.710us)
  one: 1360ns HI,  350ns LO (1.710us)
  reset: > 50us

@ 8MHz (125ns cycle)
  WS2812B:
    3 cycles -> 375ns
    7 cycles -> 875ns
  APA106:
    3 cycles -> 375ns
    11 cycles -> 1375ns
*/
void ws2812b_bang_byte(const uint8_t portb_pin, const uint8_t data)
{
#if F_CPU != 8000000UL
#error ws2812b_bang_byte() is only implemented for 8MHz
#endif

    // Prepare register words for PORTB (0x18) Register,
    // with the desired pin HIGH and LOW
    const uint8_t pb = PORTB;
    const uint8_t pb_hi = (pb | (1 << portb_pin));
    const uint8_t pb_lo = (pb & ~(1 << portb_pin));

    __asm__ volatile(
        // Prepare first bit
        "ldi r17,0b10000000\n" // Mask for detecting bit to be sent. Initial value for bit 7
        "mov r16,%[data]\n"
        "and r16,r17\n" // Result is stored in r16
        "breq .send_zero\n"

        ".send_one:\n"
        "out 0x18,%[pb_hi]\n" // Set HIGH
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
#ifdef AVR_LAB_APA106
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
#endif
        "out 0x18,%[pb_lo]\n" // Set LOW
        // Staying LOW for >= 3 cycles, no extra nops required

        // Prepare next bit
        "lsr r17\n"
        "brcs .done\n"
        "mov r16,%[data]\n"
        "and r16,r17\n"
        "brne .send_one\n"

        ".send_zero:\n"
        "out 0x18,%[pb_hi]\n" // Set HIGH
        "nop\n"
        "nop\n"
        "out 0x18,%[pb_lo]\n" // Set LOW
        // Staying LOW for >= 7 cycles + optional extra nop cycles
#ifdef AVR_LAB_APA106
        "nop\n"
        "nop\n"
        "nop\n"
        "nop\n"
#endif

        // Prepare next bit
        "lsr r17\n"
        "brcs .done\n"
        "mov r16,%[data]\n"
        "and r16,r17\n"
        "brne .send_one\n"
        "rjmp .send_zero\n"

        ".done:\n"

        :                                                          // Outputs
        : [pb_hi] "r"(pb_hi), [pb_lo] "r"(pb_lo), [data] "r"(data) // Inputs
        : "r16", "r17");                                           // Clobbered registers
}
