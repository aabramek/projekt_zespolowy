#ifndef ANALOG_MEASUREMENTS_H
#define ANALOG_MEASUREMENTS_H

#include "esp_err.h"

#define ANALOG_MEASUREMENTS_NOTIFICATION_INDEX 0

typedef struct
{
	int32_t values[4];
}
AnalogMeasurementResult_t;

void AnalogMeasurementTaskCode(void *pvParameters);

#endif
