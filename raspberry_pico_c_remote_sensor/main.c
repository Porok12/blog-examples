#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "src/sleep_utils.h"
#include "src/tcp_utils.h"
#include "src/dht.h"

#define LED_PIN 22

int init_led(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    return 0;
}

int init_wifi(void) {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_POLAND)) {
        printf("Wi-Fi failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Failed to connect.\n");
        return 1;
    }

    printf("Connected.\n");
    return 0;
}

int comparator(const void *p, const void *q) {
    return (int)(((dht_reading*)p)->temp_celsius - ((dht_reading*)q)->temp_celsius);
}

static volatile bool awake;

static void sleep_callback(void) {
    awake = true;
}

int main(void) {
    /* Initialize */

    stdio_init_all();
    init_led();
    dht_init();

    /* Definitions */

    dht_reading measurements[5];

    uint scb_orig = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

    datetime_t t = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 00,
            .sec   = 00
    };

    rtc_init();

    /* Logic */

    sleep_ms(2000);
    measure_freqs();
    sleep_ms(1000);

    while (1) {
        awake = false;
        gpio_put(LED_PIN, 0);
        if (cyw43_is_initialized(&cyw43_state)) {
            cyw43_arch_deinit();
        }
        sleep_run_from_xosc(); // UART will be reconfigured by sleep_run_from_xosc
        rtc_set_datetime(&t);
        rtc_sleep(1, 0, sleep_callback);

        while (!awake) {
            printf("Should be sleeping\n");
        }

        gpio_put(LED_PIN, 1);
        recover_from_sleep(scb_orig, clock0_orig, clock1_orig);
        init_wifi();

        for (int c = 0; c < 5; ++c) {
            dht_reading data;
            read_from_dht(&data);
            printf("%f %f\n", data.temp_celsius, data.humidity);
            measurements[c] = data;
            sleep_ms(2000);
        }
        qsort((void*)measurements, 5, sizeof(dht_reading), comparator);

        send_measurements(&measurements[2]);
    }

    return 0;
}
