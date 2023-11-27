#ifndef ONE_WIRE_MEASUREMENTS_H
#define ONE_WIRE_MEASUREMENTS_H

#include "onewire_bus.h"

#define ONE_WIRE_MEASUREMENTS_NOTIFICATION_INDEX 2
#define ONE_WIRE_MAX_SENSORS_COUNT 15

typedef struct
{
	onewire_device_address_t address;
	float temperature;
}
OneWireReading_t;

typedef struct
{
	OneWireReading_t *readings;
	size_t num_readings;
}
OneWireMeasurementResult_t;

void OneWireMeasurementTaskCode(void *pvParameters);

#endif
