#ifndef STATION_H
#define STATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"	
#include "freertos/timers.h"

#include "esp_err.h"
#include "mqtt_client.h"

#define SOFTWARE_VERSION 0x100
#define DEFAULT_MEASUREMENT_PERIOD 5

#define EVT_WIFI_CONFIG_READY	BIT0
#define EVT_WIFI_CONNECTED		BIT1
#define EVT_WIFI_FAIL			BIT2
#define EVT_MQTT_CONFIG_READY	BIT3
#define EVT_MQTT_CONNECTED		BIT4
#define EVT_MQTT_FAIL			BIT5
#define EVT_CONFIG_RECEIVED		BIT6

#define WATERER_TASK_STACK_DEPTH 100
#define MEASURER_TASK_STACK_DEPTH 100
#define CONFIGURATION_RECEIVER_TASK_STACK_DEPTH 100

#define STATION_TASKS_PRIORITY 4

typedef struct
{
	uint32_t ip_address;
	uint8_t mac_address[6];
	uint16_t software_version;

	char name[31];

	uint8_t measurement_period;

	TaskHandle_t waterer_task;
	TaskHandle_t measurer_task;
	TaskHandle_t configuration_receiver_task;
} Station_t;

extern Station_t _station;
extern EventGroupHandle_t _station_event_group;
extern esp_mqtt_client_handle_t _mqtt_client;
extern TimerHandle_t _period_timer;

void Station_Start(void);

#endif
