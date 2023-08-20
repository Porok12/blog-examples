#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "dht.h"

int dht_init(void) {
    gpio_init(DHT_PIN);
    return 0;
}

inline int read_until(bool value) {
    int count = 0;
    while (gpio_get(DHT_PIN) == value) {
        ++count;
        sleep_us(1);
        if (count > 255) return -1;
    }
    return count;
}

int read_from_dht(dht_reading *result) {
    uint8_t data[5] = {0, 0, 0, 0, 0};
    int count;

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(20);
    gpio_set_dir(DHT_PIN, GPIO_IN);

    sleep_us(150);

    if (read_until(1) == -1) return 1;
    for (uint i = 0; i < DATA_SIZE; ++i) {
        if (read_until(0) == -1) return 2;
        if ((count = read_until(1)) == -1) return 3;

        data[i / DATA_SIZE_PARTS] <<= 1;
        if (count > TIME_SEPARATE_BIT_0_BIT_1) data[i / DATA_SIZE_PARTS] |= 1;
    }

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        result->humidity = (float) ((data[0] << 8) + data[1]) / 10;
        if (result->humidity > MAX_HUMIDITY) {
            result->humidity = data[0];
        }
        result->temp_celsius = (float) (((data[2] & 0x7F) << 8) + data[3]) / 10;
        if (result->temp_celsius > MAX_TEMPERATURE) {
            result->temp_celsius = data[2];
        }
        if (data[2] & 0x80) {
            result->temp_celsius = -result->temp_celsius;
        }
    } else {
        printf("bad data: %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4]);
        return 4;
    }

    return 0;
}
