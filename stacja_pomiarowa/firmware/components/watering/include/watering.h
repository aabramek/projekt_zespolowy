#ifndef WATERING_H
#define WATERING_H

#include "cJSON.h"

typedef enum
{
	sunday, monday, tuesday, wednesday, thursday, friday, saturday
}
DayOfWeek_t;

typedef enum
{
	off, on, automatic
}
ControlMode_t;

typedef struct
{
	uint8_t minute;
	uint8_t hour;
	DayOfWeek_t day_of_week;
}
TimePoint_t;

typedef struct
{
	DayOfWeek_t day_of_week;
	uint8_t minute_start;
	uint8_t hour_start;
	uint8_t minute_stop;
	uint8_t hour_stop;
}
Schedule_t;

typedef struct
{	
	uint8_t active_state;
	ControlMode_t mode;
	
	Schedule_t *schedules;
	int num_schedules;	
}
Valve_t;

void WateringTaskCode(void *pvParameters);

void watering_configure(cJSON *valves);

void valves_dump_configuration(void);

#endif
