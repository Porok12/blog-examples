#ifndef SLEEP_UTILS_H
#define SLEEP_UTILS_H

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/watchdog.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/rtc.h"

void rtc_sleep(int8_t minute_to_sleep_to, int8_t second_to_sleep_to, rtc_callback_t sleep_callback);
void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig);
void measure_freqs(void);

#endif //SLEEP_UTILS_H
