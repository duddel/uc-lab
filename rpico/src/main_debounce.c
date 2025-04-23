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
#include "debounce.h"

#if defined(RASPBERRYPI_PICO_W) || defined(RASPBERRYPI_PICO2_W)
#include "pico/cyw43_arch.h"
#endif

#if defined(PICO_DEFAULT_WS2812_PIN)
#include "ws2812.pio.h" // generated from ws2812.pio
#endif

int main()
{
    const uint button_pin = 15;

// WS2812 on-board LED
#if defined(PICO_DEFAULT_WS2812_PIN)
    const uint data_pin = PICO_DEFAULT_WS2812_PIN; // TX data pin index
    const PIO pio_idx = pio0;                      // PIO instance
    const int pio_sm = 0;                          // PIO state machine index

    uint offset = pio_add_program(pio_idx, &ws2812_program);
    // 1/800000Hz = 1.25us, which is the typical duration of a ws2812 bit signal
    ws2812_program_init(pio_idx, pio_sm, offset, data_pin, 800000, false);
#elif defined(RASPBERRYPI_PICO_W) || defined(RASPBERRYPI_PICO2_W)
    const uint led_pin = CYW43_WL_GPIO_LED_PIN;

    if (cyw43_arch_init())
    {
        return -1;
    }
#else
    const uint led_pin = PICO_DEFAULT_LED_PIN;

    // initialize gpio led pin as OUTPUT
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
#endif

    // initialize gpio button pin as INPUT
    gpio_init(button_pin);
    gpio_set_dir(button_pin, GPIO_IN);

    // initialize button debouncer
    debouncer button_deb;
    button_deb.delay_time_us = 50000; // 50ms

    // the last debounced state of the button
    bool last_button_state = false;

    // the current led on/off state
    bool led_on = false;

    while (true)
    {
        // insert raw button state into the debouncer
        debounce_update(gpio_get(button_pin), &button_deb);

        // if the (debounced) button state changed...
        if (last_button_state != button_deb.state)
        {
            // set last_button_state
            last_button_state = button_deb.state;

            // change led on/off state if button was pressed down
            if (last_button_state)
            {
                led_on = !led_on;
            }
        }

#if defined(PICO_DEFAULT_WS2812_PIN)
        pio_sm_put_blocking(pio_idx, pio_sm, led_on ? 0x00ff0000 : 0x00000000);
#elif defined(RASPBERRYPI_PICO_W) || defined(RASPBERRYPI_PICO2_W)
        cyw43_arch_gpio_put(led_pin, led_on);
#else
        gpio_put(led_pin, led_on);
#endif
    }
}
