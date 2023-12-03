#include <stdio.h>
#include <time.h>

#include "watering.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#define VALVES_NUM (16)

static const char* TAG = "Watering";

static Valve_t *valves;

static bool time_matches_schedule(const TimePoint_t *time_point, 
	const Schedule_t *schedule);

static uint16_t set_valve_state(const TimePoint_t *time_point,
	const Valve_t *valve);

static DayOfWeek_t get_day_of_week(const struct tm *local_time);

void WateringTaskCode(void *pvParameters)
{
	valves = malloc(sizeof(Valve_t) * VALVES_NUM);
	
	for (int i = 0; i < VALVES_NUM; ++i)
	{
		valves[i].configuration.active_state = 1;
		valves[i].configuration.mode = off;
		valves[i].schedules = NULL;
		valves[i].num_schedules = 0;
	}

	time_t timestamp;
	struct tm local_time;
	TimePoint_t time_point;

	uint16_t valves_states;

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		time(&timestamp);
		localtime_r(&timestamp, &local_time);
		time_point.minute = local_time.tm_min;
		time_point.hour = local_time.tm_hour;
		time_point.day_of_week = get_day_of_week(&local_time);
			
		ESP_LOGI(TAG, "Watering: %02d-%02d-%02d %02d:%02d %d",
			local_time.tm_mday, local_time.tm_mon + 1,
			local_time.tm_year + 1900, local_time.tm_hour,
			local_time.tm_min, time_point.day_of_week);

		valves_states = 0;
		
		for (int i = 0; i < VALVES_NUM; ++i)
		{
			uint16_t state = set_valve_state(&time_point, &valves[i]);
			valves_states |= (state << i);
		}
	}
}

void watering_configure(cJSON *valves)
{
	if (valves == NULL)
	{
		return;
	}
	
}

static bool time_matches_schedule(const TimePoint_t *time_point, 
	const Schedule_t *schedule)
{
	if (time_point->day_of_week != schedule->day_of_week)
	{
		return false;
	}

	if (time_point->hour < schedule->hour_start
		|| time_point->hour > schedule->hour_stop)
	{
		return false;
	}

	if (time_point->hour == schedule->hour_start
		&& time_point->minute < schedule->minute_start)
	{
		return false;
	}

	if (time_point->hour == schedule->hour_stop
		&& time_point->minute > schedule->minute_stop)
	{
		return false;
	}

	return true;
}

static uint16_t set_valve_state(const TimePoint_t *time_point,
	const Valve_t *valve)
{
	for (int i = 0; i < valve->num_schedules; ++i)
	{
		if (time_matches_schedule(time_point, &valve->schedules[i]))
		{
			return valve->configuration.active_state; 
		}
	}
	return valve->configuration.active_state ^ 1;	
}

static DayOfWeek_t get_day_of_week(const struct tm *local_time)
{
	int d = local_time->tm_mday;
	int m = local_time->tm_mon + 1;
	int y = local_time->tm_year + 1900;

	int i = (d += m < 3 ? y-- :
		y - 2, 23 * m / 9 + d + 4 + y / 4- y / 100 + y / 400) % 7;

	return (DayOfWeek_t)i;
}