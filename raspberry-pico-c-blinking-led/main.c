#include "pico/stdlib.h"

void gpio_toggle(uint pin) {
    gpio_put(pin, !gpio_get(pin));
}

int main() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_toggle(LED_PIN);
        sleep_ms(250);
    }
#endif
}