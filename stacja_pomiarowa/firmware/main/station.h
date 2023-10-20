#ifndef STATION_H
#define STATION_H

#include "esp_err.h"

typedef struct
{
	uint32_t soil_humidity;
	uint32_t sun;
	uint8_t air_humidity;
	uint8_t air_temperature;
} Sensors_t;

typedef struct
{
	uint32_t id;

	uint32_t ip_address;
	uint64_t mac_address;
	uint16_t software_version;

	char name[31];

	uint8_t measurement_period;

	Sensors_t sensors;
} Station_t;

extern Station_t station;

#endif