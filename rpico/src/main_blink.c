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
#include "pico/stdlib.h"

#ifdef RASPBERRYPI_PICO_W
#include "pico/cyw43_arch.h"
#endif

#ifdef PICO_DEFAULT_WS2812_PIN
#include "ws2812.pio.h" // generated from ws2812.pio
#endif

int main()
{
// WS2812 on-board LED
#ifdef PICO_DEFAULT_WS2812_PIN
    const uint data_pin = PICO_DEFAULT_WS2812_PIN; // TX data pin index
    const PIO pio_idx = pio0;                      // PIO instance
    const int pio_sm = 0;                          // PIO state machine index

    uint offset = pio_add_program(pio_idx, &ws2812_program);
    // 1/800000Hz = 1.25us, which is the typical duration of a ws2812 bit signal
    ws2812_program_init(pio_idx, pio_sm, offset, data_pin, 800000, false);

    while (true)
    {
        pio_sm_put_blocking(pio_idx, pio_sm, 0x00ff0000);
        sleep_ms(750);
        pio_sm_put_blocking(pio_idx, pio_sm, 0x00000000);
        sleep_ms(750);
    }
#else
// Simple on-board LED
#ifdef RASPBERRYPI_PICO_W
    if (cyw43_arch_init())
    {
        return -1;
    }

    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(750);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(750);
    }
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(750);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(750);
    }
#endif
#endif
}
