#ifndef WATERING_H
#define WATERING_H

typedef enum
{
	sunday, monday, tuesday, wednesday, thursday, friday, saturday
}
DayOfWeek_t;

typedef enum
{
	on, off, automatic
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
	struct
	{
		uint8_t active_state : 1;
		ControlMode_t mode : 2;
	} configuration;
	
	Schedule_t *schedules;
	int num_schedules;	
}
Valve_t;

void WateringTaskCode(void *pvParameters);

#endif
