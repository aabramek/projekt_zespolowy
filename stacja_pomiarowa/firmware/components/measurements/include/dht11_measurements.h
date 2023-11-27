#ifndef DHT11_MEASUREMENTS_H
#define DHT11_MEASUREMENTS_H

#include "dht11.h"

#define DHT11_MEASUREMENTS_NOTIFICATION_INDEX 1

typedef struct
{
	DHT11_Reading_t reading;
	bool available;
}
DHT11MeasurementResult_t; 

void DHT11MeasurementTaskCode(void *pvParameters);

#endif