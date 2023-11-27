#ifndef STATION_H
#define STATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"	
#include "freertos/timers.h"

#include "esp_err.h"
#include "mqtt_client.h"

#define SOFTWARE_VERSION 0x100
#define DEFAULT_MEASUREMENT_PERIOD 1

#define WATERING_TASK_STACK_DEPTH 5000
#define MEASUREMENTS_TASK_STACK_DEPTH 5000
#define CONFIGURATION_RECEIVER_TASK_STACK_DEPTH 5000

#define STATION_TASKS_PRIORITY 4

#define IP_TO_STRING(IP) IP & 0xFF, IP >> 8 & 0xFF, IP >> 16 & 0xFF, IP >> 24

#define EVT_WIFI_CONFIG_READY			BIT0
#define EVT_WIFI_CONNECTED				BIT1
#define EVT_WIFI_FAIL					BIT2
#define EVT_MQTT_CONFIG_READY			BIT3
#define EVT_MQTT_CONNECTED				BIT4
#define EVT_MQTT_FAIL					BIT5
#define EVT_CONFIG_RECEIVED				BIT6

typedef struct
{
	struct
	{
		uint32_t ip_address;
		char mac_address[18];
		uint16_t software_version;

		char name[31];

		uint8_t measurement_period;
	} configuration;

	struct
	{
		TaskHandle_t measurements;
		TaskHandle_t watering;
		TaskHandle_t configuration_receiver;
	} tasks;

	EventGroupHandle_t event_group;
	esp_mqtt_client_handle_t mqtt_client;
	TimerHandle_t period_timer;
} Station_t;

typedef Station_t *StationHandle_t;

esp_err_t Station_Create(StationHandle_t *station);

esp_err_t Station_Start(StationHandle_t station);

void Station_DumpConfiguration(StationHandle_t station);

#endif
