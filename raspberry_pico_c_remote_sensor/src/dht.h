#ifndef DHT_H
#define DHT_H

#define DHT_PIN 28
#define MAX_HUMIDITY 100
#define MAX_TEMPERATURE 125
#define DATA_SIZE 40
#define DATA_SIZE_PARTS 8
#define TIME_SEPARATE_BIT_0_BIT_1 35

typedef struct {
    float humidity;
    float temp_celsius;
} dht_reading;

int dht_init(void);
int read_from_dht(dht_reading *result);

#endif //DHT_H
