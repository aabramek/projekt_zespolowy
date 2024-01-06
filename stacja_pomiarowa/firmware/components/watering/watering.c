#include <stdio.h>
#include <time.h>
#include <string.h>

#include "watering.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "mcp23017.h"

#define VALVES_NUM 16
#define LINE_NUM_MIN 0
#define LINE_NUM_MAX 15
#define ACTIVE_STATE_MIN 0
#define ACTIVE_STATE_MAX 1
#define MODE_MIN 0
#define MODE_MAX 2

static const char* TAG = "Watering";

static Valve_t *valves = NULL;

static SemaphoreHandle_t configuration_mutex = NULL;

static bool time_matches_schedule(const TimePoint_t *time_point, 
	const Schedule_t *schedule);

static uint16_t get_valve_state(const TimePoint_t *time_point,
	const Valve_t *valve);

static DayOfWeek_t get_day_of_week(const struct tm *local_time);

static void configure_valve(cJSON *valve);

static void configure_schedules(Valve_t *valve, const cJSON *schedules);

static bool number_in_range(int number, int a, int b);

static const char *day_names[] = {
	"niedziela", "poniedzialek", "wtorek", "sroda",
	"czwartek", "piatek", "sobota"
};

static const char *mode_names[] = {
	"zawsze wylaczony",
	"zawsze wlaczony",
	"sterowanie wg harmonogramu"
};

void valves_dump_configuration(void);

void WateringTaskCode(void *pvParameters)
{
	ESP_LOGI(TAG, "Task started");

	valves = malloc(sizeof(Valve_t) * VALVES_NUM);
	if (valves == NULL)
	{
		ESP_LOGE(TAG, "Cannot allocate memory for valves configuration");
		abort();
	}

	for (int i = 0; i < VALVES_NUM; ++i)
	{
		valves[i].active_state = 1;
		valves[i].mode = off;
		valves[i].schedules = NULL;
		valves[i].num_schedules = 0;
	}

	configuration_mutex = xSemaphoreCreateMutex();
	if (configuration_mutex == NULL)
	{
		ESP_LOGE(TAG, "Cannot create mutex");
		abort();
	}

	time_t timestamp;
	struct tm local_time;
	TimePoint_t time_point;

	uint16_t valves_states;

	MCP23017_t mcp = {
		.address = 0x27,
		.i2c_port = I2C_NUM_0
	};

	MCP23017_Write(&mcp, MCP23017_REGISTER_MAP_BANK[IODIRA], 0);
	MCP23017_Write(&mcp, MCP23017_REGISTER_MAP_BANK[IODIRB], 0);

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
		
		while (xSemaphoreTake(configuration_mutex, pdMS_TO_TICKS(100)) == pdFALSE)
		{
			ESP_LOGI(TAG, "Waiting for semaphore to read configuration");
		}

		valves_states = 0;

		for (uint16_t i = 0; i < VALVES_NUM; ++i)
		{
			uint16_t state = get_valve_state(&time_point, &valves[i]);
			valves_states |= (state << i);
		}

		xSemaphoreGive(configuration_mutex);

		MCP23017_Write(&mcp, MCP23017_REGISTER_MAP_BANK[GPIOA],
			valves_states);
		MCP23017_Write(&mcp, MCP23017_REGISTER_MAP_BANK[GPIOB],
			valves_states >> 8);
	}
}

void watering_configure(cJSON *valves)
{
	if (valves == NULL)
	{
		return;
	}
	
	int n = cJSON_GetArraySize(valves);
	
	while (xSemaphoreTake(configuration_mutex, pdMS_TO_TICKS(100)) == pdFALSE)
	{
		ESP_LOGI(TAG, "Waiting for semaphore to update configuration");
	}

	for (int i = 0; i < n; ++i)
	{
		configure_valve(cJSON_GetArrayItem(valves, i));
	}

	xSemaphoreGive(configuration_mutex);
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
		&& time_point->minute >= schedule->minute_stop)
	{
		return false;
	}

	return true;
}

static uint16_t get_valve_state(const TimePoint_t *time_point,
	const Valve_t *valve)
{
	if (valve->mode == on)
	{
		return valve->active_state;
	}
	if (valve->mode == off)
	{
		return valve->active_state ^ 1;
	}
	for (int i = 0; i < valve->num_schedules; ++i)
	{
		if (time_matches_schedule(time_point, &valve->schedules[i]))
		{
			return valve->active_state; 
		}
	}
	return valve->active_state ^ 1;	
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

static void configure_valve(cJSON *valve)
{
	cJSON *line_json = cJSON_GetObjectItemCaseSensitive(valve, "line");
	if (!cJSON_IsNumber(line_json))
	{
		return;
	}
	int line = cJSON_GetNumberValue(line_json);
	if (!number_in_range(line, LINE_NUM_MIN, LINE_NUM_MAX))
	{
		return;
	}

	cJSON *active_state_json = cJSON_GetObjectItemCaseSensitive(valve,
		"active_state");
	if (!cJSON_IsNumber(active_state_json))
	{
		goto mode_json_property;
	}
	int active_state = cJSON_GetNumberValue(active_state_json);
	if (!number_in_range(active_state, ACTIVE_STATE_MIN, ACTIVE_STATE_MAX))
	{
		goto mode_json_property;
	}
	valves[line].active_state = active_state;

mode_json_property:
	cJSON *mode_json = cJSON_GetObjectItemCaseSensitive(valve, "mode");
	if (!cJSON_IsNumber(mode_json))
	{
		goto schedules_property;
	}	
	int mode = cJSON_GetNumberValue(mode_json);
	if (!number_in_range(mode, MODE_MIN, MODE_MAX))
	{
		goto schedules_property;
	}
	valves[line].mode = mode;

schedules_property:
	cJSON *schedules_json = cJSON_GetObjectItemCaseSensitive(valve,
		"schedules");
	if (!cJSON_IsArray(schedules_json))
	{
		return;
	}

	configure_schedules(&valves[line], schedules_json);
}

// todo: zrobić 'vector'
static void configure_schedules(Valve_t *valve, const cJSON *schedules)
{
	static Schedule_t* buffer = NULL;
	static int buffer_size = 0;

	int num_schedules_new = cJSON_GetArraySize(schedules);
	if (num_schedules_new == 0)
	{
		return;
	}
	int buffer_size_bytes = sizeof(Schedule_t) * num_schedules_new;
	if (num_schedules_new > buffer_size)
	{
		buffer = realloc(buffer, buffer_size_bytes);
	}
	buffer_size = num_schedules_new;

	bool correct = true;
	for (int i = 0; i < num_schedules_new; ++i)
	{
		cJSON *schedule_json = cJSON_GetArrayItem(schedules, i);
		
		if (!cJSON_IsObject(schedule_json))
		{
			correct = false;
			break;
		}

		cJSON *day_of_week_json = cJSON_GetObjectItemCaseSensitive(
			schedule_json, "day");

		cJSON *minute_start_json = cJSON_GetObjectItemCaseSensitive(
			schedule_json, "minute_start");

		cJSON *minute_stop_json = cJSON_GetObjectItemCaseSensitive(
			schedule_json, "minute_stop");

		cJSON *hour_start_json = cJSON_GetObjectItemCaseSensitive(
			schedule_json, "hour_start");

		cJSON *hour_stop_json = cJSON_GetObjectItemCaseSensitive(
			schedule_json, "hour_stop");

		if (!cJSON_IsNumber(day_of_week_json)
			|| !cJSON_IsNumber(minute_start_json)
			|| !cJSON_IsNumber(minute_stop_json)
			|| !cJSON_IsNumber(hour_start_json)
			|| !cJSON_IsNumber(hour_stop_json))
		{
			correct = false;
			break;
		}

		int day_of_week = cJSON_GetNumberValue(day_of_week_json);
		int minute_start = cJSON_GetNumberValue(minute_start_json);
		int minute_stop = cJSON_GetNumberValue(minute_stop_json);
		int hour_start = cJSON_GetNumberValue(hour_start_json);
		int hour_stop = cJSON_GetNumberValue(hour_stop_json);

		if (!number_in_range(day_of_week, sunday, saturday)
			|| !number_in_range(minute_start, 0, 59)
			|| !number_in_range(minute_stop, 0, 59)
			|| !number_in_range(hour_start, 0, 23)
			|| !number_in_range(hour_stop, 0, 23))
		{
			correct = false;
			break;
		}

		buffer[i].day_of_week = day_of_week;
		buffer[i].minute_start = minute_start;
		buffer[i].minute_stop = minute_stop;
		buffer[i].hour_start = hour_start;
		buffer[i].hour_stop = hour_stop;
	}

	if (!correct)
	{
		return;
	}

	if (num_schedules_new > valve->num_schedules)
	{
		valve->schedules = realloc(valve->schedules, buffer_size_bytes);
	}
	valve->num_schedules = num_schedules_new;
	memcpy(valve->schedules, buffer, buffer_size_bytes);
}

static bool number_in_range(int number, int a, int b)
{
	return number >= a && number <= b;
}

void valves_dump_configuration(void)
{
	for (int i = 0; i < VALVES_NUM; ++i)
	{
		printf("Elektrozawor #%d: stan aktywny: %d, %s\r\n", i, valves[i].active_state,
			mode_names[valves[i].mode]);

		for (int j = 0; j < valves[i].num_schedules; ++j)
		{
			printf("%s %02d:%02d -> %02d:%02d\r\n",
				day_names[valves[i].schedules[j].day_of_week],
				valves[i].schedules[j].hour_start,
				valves[i].schedules[j].minute_start,
				valves[i].schedules[j].hour_stop,
				valves[i].schedules[j].minute_stop);
		}
	}
}
