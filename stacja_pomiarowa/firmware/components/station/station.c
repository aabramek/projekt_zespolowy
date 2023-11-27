#include "station.h"

#include "analog_measurements.h"
#include "dht11_measurements.h"
#include "one_wire_measurements.h"
#include "mux.h"

#include "esp_mac.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"

#include "driver/i2c.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define WIFI_BITS_SET(bits) (bits & (EVT_WIFI_CONNECTED | EVT_WIFI_FAIL))

#define JSON_BUFFER_LEN (2000)

// ============================= //
// STATIC FUNCTIONS DECLARATIONS //
// ============================= //
static esp_err_t peripherals_initialize(void);

static esp_err_t wifi_connect(StationHandle_t station);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

static void period_timer_callback(TimerHandle_t xTimer);

static void Station_WateringTaskCode(void *pvParameters);

static void Station_MeasurementsTaskCode(void *pvParameters);

static void Station_ConfigurationReceiverTaskCode(void *pvParameters);

static esp_err_t Station_StartTasks(StationHandle_t station);

static void mqtt_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

static int create_measurement_message(time_t timestamp, char *mac_address,
	int *analog_readings, DHT11MeasurementResult_t *dht11_results,
	OneWireMeasurementResult_t *onewire_result, char *buffer,
	size_t buffer_len);

// ===================== //
// FUNCTIONS DEFINITIONS //
// ===================== //

esp_err_t Station_Create(StationHandle_t *station)
{
	const char *TAG = "Station_Create";

	StationHandle_t s = calloc(1, sizeof(Station_t));
	
	if (station == NULL)
	{
		return ESP_FAIL;
	}

	uint8_t mac[6];
	if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot read mac address");
		return ESP_FAIL;
	}

	sprintf(s->configuration.mac_address,
		"%02X-%02X-%02X-%02X-%02X-%02X", 
		mac[0], mac[1], mac[2],
		mac[3], mac[4], mac[5]);

	s->configuration.software_version = SOFTWARE_VERSION;
	s->configuration.measurement_period = DEFAULT_MEASUREMENT_PERIOD;
	
	s->event_group = xEventGroupCreate();
	if (s->event_group == NULL)
	{
		ESP_LOGE(TAG, "Cannot create event group");
		return ESP_FAIL;
	}

	esp_mqtt_client_config_t mqtt_cfg = {
		.broker = {
			.address = {
				.uri = "mqtt://192.168.0.220:11337/"
			}
		},

		.credentials = {
			.username = "esp32",
			.authentication = {
				.password = "mqtt"
			}
		}
	};

	s->mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	if (s->mqtt_client == NULL)
	{
		ESP_LOGE(TAG, "Cannot create mqtt client");
		return ESP_FAIL;
	}

	s->period_timer = xTimerCreate("Station period timer",
		pdMS_TO_TICKS(1000 * 60), pdTRUE, s, period_timer_callback);

	if (s->period_timer == NULL)
	{
		ESP_LOGE(TAG, "Cannot create period timer");
		return ESP_FAIL;
	}

	*station = s;
	return ESP_OK;
}

esp_err_t Station_Start(StationHandle_t station)
{
	static const char* TAG = "Station_Start";

	ESP_ERROR_CHECK(peripherals_initialize());

	ESP_ERROR_CHECK(wifi_connect(station));

	EventBits_t bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for wifi connection");
		bits = xEventGroupWaitBits(station->event_group, EVT_WIFI_CONNECTED,
			pdTRUE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_WIFI_CONNECTED;
	}
	while (bits == 0);

	esp_sntp_config_t sntp_cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
	esp_netif_sntp_init(&sntp_cfg);
	while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(5000)) != ESP_OK)
	{
		ESP_LOGW(TAG, "Waiting for sntp sync");
	}

	ESP_LOGI(TAG, "Received sntp time");

	if (esp_mqtt_client_register_event( station->mqtt_client,
		ESP_EVENT_ANY_ID, mqtt_event_handler, station->event_group) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot register mqtt event");
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Registered mqtt event");

	if (esp_mqtt_client_start(station->mqtt_client) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot start mqtt client");
	}
	ESP_LOGI(TAG, "Started mqtt client");

	bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for mqtt client connection");
		bits = xEventGroupWaitBits(station->event_group, EVT_MQTT_CONNECTED,
			pdTRUE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_MQTT_CONNECTED;
	}
	while (bits == 0);

	ESP_LOGI(TAG, "Mqtt client connected");

	if (esp_mqtt_client_subscribe(station->mqtt_client,
		station->configuration.mac_address, 0) == -1)
	{
		ESP_LOGE(TAG, "Cannot subscribe to configuration receiver topic");
		return ESP_FAIL;
	}

	char register_msg[100];
	
	sprintf(register_msg, "{\"ip\":%lu,\"mac\":\"%s\",\"sv\":%d}",
		station->configuration.ip_address, station->configuration.mac_address, 
		station->configuration.software_version);

	ESP_LOGI(TAG, "%s", register_msg);

	if (esp_mqtt_client_publish(station->mqtt_client, "register", register_msg,
		strlen(register_msg), 2, 0) == -1)
	{
		ESP_LOGE(TAG, "Cannot send registration message");
		return ESP_FAIL;
	}

	do
	{
		bits = xEventGroupWaitBits(station->event_group, EVT_CONFIG_RECEIVED,
			pdFALSE, pdTRUE, pdMS_TO_TICKS(1000)) & EVT_CONFIG_RECEIVED;
		
		ESP_LOGI(TAG, "Waiting for configuration");
	}
	while (bits == 0);

	ESP_LOGI(TAG, "Registration message receved");

	//todo: odczytac konfiguracje i ja zaladowac

	if (Station_StartTasks(station) != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot create station tasks");
		return ESP_FAIL;
	}

	while (xTimerStart(station->period_timer, pdMS_TO_TICKS(100)) == pdFAIL)
	{
		ESP_LOGW(TAG, "Cannot start station period timer, waiting...");
	}

	return ESP_OK;
}

void Station_DumpConfiguration(StationHandle_t station)
{
	fprintf(stdout,

		"Station configuration:\n"
		"ip: %lu.%lu.%lu.%lu\n"
		"mac: %s\n"
		"name: %s\n"
		"measurement_period: %u\n",
		
		IP_TO_STRING(station->configuration.ip_address),
		station->configuration.mac_address,
		station->configuration.name,
		station->configuration.measurement_period);
}

// ============================ //
// STATIC FUNCTIONS DEFINITIONS //
// ============================ //

static esp_err_t Station_StartTasks(StationHandle_t station)
{
	BaseType_t ret;
	/*ret = xTaskCreate(Station_WatererTaskCode,
		"Waterer task",
		WATERER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&station->tasks.waterer_task);
	
	if (ret == pdFALSE)
	{
		return ESP_FAIL;
	}*/

	ret = xTaskCreate(Station_MeasurementsTaskCode, "Measurements task",
		MEASUREMENTS_TASK_STACK_DEPTH, station, STATION_TASKS_PRIORITY,
		&station->tasks.measurements);

	if (ret == pdFALSE)
	{
		return ESP_FAIL;
	}

	/*ret = xTaskCreate(Station_ConfigurationReceiverTaskCode,
		"Configuration receiver task",
		CONFIGURATION_RECEIVER_TASK_STACK_DEPTH,
		NULL,
		STATION_TASKS_PRIORITY,
		&station->tasks.configuration_receiver_task);*/

	if (ret == pdFALSE)
	{
		return ESP_FAIL;
	}

	return ESP_OK;
}

static esp_err_t peripherals_initialize(void)
{
	i2c_config_t i2c_cfg = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = GPIO_NUM_21,
		.scl_io_num = GPIO_NUM_22,
		.sda_pullup_en = false,
		.scl_pullup_en = false,
		.master.clk_speed = 50000
	};
	esp_err_t retval = ESP_OK;
	retval = i2c_param_config(I2C_NUM_0, &i2c_cfg);
	if (retval != ESP_OK)
	{
		return retval;
	}
	
	retval = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	if (retval != ESP_OK)
	{
		return retval;
	}

	return retval;
}

static esp_err_t wifi_connect(StationHandle_t station)
{
	const char *wifi_ssid[] = {
		"Galaxy S22 389D",
		"UPC2194636"
	};

	const char *wifi_passwd[] = {
		"tom12345",
		"hJwtdhP6bnyx"
	};

	esp_err_t ret = ESP_OK;

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler,
		station, NULL));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, 
		station, NULL));

	wifi_config_t wifi_cfg = {
		.sta = {
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
			.pmf_cfg = {
				.capable = true,
				.required = false
			}
		}
	};

	memcpy(wifi_cfg.sta.ssid, wifi_ssid[1], strlen(wifi_ssid[1]));
	memcpy(wifi_cfg.sta.password, wifi_passwd[1], strlen(wifi_passwd[1]));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
	ESP_ERROR_CHECK(esp_wifi_start());

	return ret;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data)
{
	const char *TAG = "wifi_event_handler";

	StationHandle_t station = (StationHandle_t)arg;
	if (event_base == WIFI_EVENT)
	{
		if (event_id == WIFI_EVENT_STA_START)
		{
			esp_wifi_connect();
		}
		else if (event_id == WIFI_EVENT_STA_CONNECTED)
		{

		}
		else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
		{
			ESP_LOGW(TAG, "Wifi connection lost, retrying");
			esp_wifi_connect();
		}
	}
	else
	{
		if (event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
       		station->configuration.ip_address = event->ip_info.ip.addr;
			xEventGroupSetBits(station->event_group, EVT_WIFI_CONNECTED);
		}
	}
}

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, 
	int32_t event_id, void *event_data)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)arg;
	
	esp_mqtt_event_handle_t mqtt_event = (esp_mqtt_event_handle_t)event_data;
	
	switch (mqtt_event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		xEventGroupSetBits(event_group, EVT_MQTT_CONNECTED);
		break;

	case MQTT_EVENT_DATA:
		
		xEventGroupSetBits(event_group, EVT_CONFIG_RECEIVED);
		break;

	default:
		break;
	}
	
}

static void period_timer_callback(TimerHandle_t xTimer)
{
	static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
	static int minutes_elapsed = 0;
	
	StationHandle_t station = (StationHandle_t)pvTimerGetTimerID(xTimer);
	
	int period;
	
	taskENTER_CRITICAL(&spinlock);
	period = station->configuration.measurement_period;
	taskEXIT_CRITICAL(&spinlock);
	
	if (++minutes_elapsed == station->configuration.measurement_period)
	{
		minutes_elapsed = 0;
		xTaskNotifyGive(station->tasks.measurements);
	}

	// xTaskNotifyGive(station->tasks.watering);
}

static void Station_WateringTaskCode(void *pvParameters)
{
	while (1)
	{

	}
}

static void Station_MeasurementsTaskCode(void *pvParameters)
{
	const char* TAG = "MeasurementsTask";
	
	ESP_LOGI(TAG, "Task started");

	StationHandle_t station = (StationHandle_t)pvParameters;
	
	if (mux_init() != ESP_OK)
	{
		ESP_LOGE(TAG, "Cannot initialize gpios for multiplexers");
		abort();
	}

	ESP_LOGI(TAG, "Gpio for muxes initialized");
	
	TaskHandle_t analog_measurements_task = NULL;
	TaskHandle_t dht11_measurements_task = NULL;
	TaskHandle_t ow_measurements_task = NULL;

	TaskHandle_t task = xTaskGetCurrentTaskHandle();

	if (xTaskCreate(AnalogMeasurementTaskCode, "Analog Measurements", 
		MEASUREMENTS_TASK_STACK_DEPTH, task, STATION_TASKS_PRIORITY - 1,
		&analog_measurements_task) == pdFAIL)
	{
		ESP_LOGE(TAG, "Cannot create AnalogMeasurementTask");
		abort();
	}

	ESP_LOGI(TAG, "Created AnalogMeasurementTask");

	if (xTaskCreate(DHT11MeasurementTaskCode, "DHT11 Measurements",
		MEASUREMENTS_TASK_STACK_DEPTH, task, STATION_TASKS_PRIORITY - 1,
		&dht11_measurements_task) == pdFAIL)
	{
		ESP_LOGE(TAG, "Cannot create DHT11MeasurementTask");
		abort();
	}

	if (xTaskCreate(OneWireMeasurementTaskCode, "OneWire Measurements",
		MEASUREMENTS_TASK_STACK_DEPTH, task, STATION_TASKS_PRIORITY - 1,
		&ow_measurements_task) == pdFAIL)
	{
		ESP_LOGE(TAG, "Cannot create OneWireMeasurementTask");
		abort();
	}

	AnalogMeasurementResult_t *analog_result = NULL;
	
	DHT11MeasurementResult_t *dht11_result = NULL;
	DHT11MeasurementResult_t *dht11_results = malloc(
		sizeof(DHT11MeasurementResult_t) * 8);

	OneWireMeasurementResult_t *onewire_result = NULL;

	int *analog_readings = malloc(sizeof(int) * 32);
	char *json = malloc(JSON_BUFFER_LEN);

	time_t timestamp;
	int message_length;
	int message_id;

	while (1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		time(&timestamp);
		ESP_LOGI(TAG, "Taking measurements at %lld", timestamp);

		xTaskNotifyGive(ow_measurements_task);

		for (int i = 0; i < 8; ++i)
		{
			mux_address(i);

			xTaskNotifyGiveIndexed(dht11_measurements_task, 0);
			xTaskNotifyGive(analog_measurements_task);
			
			while (xTaskNotifyWaitIndexed(ANALOG_MEASUREMENTS_NOTIFICATION_INDEX,
				0, ULONG_MAX, (uint32_t*)&analog_result, pdMS_TO_TICKS(60))
				== pdFALSE)
			{
				ESP_LOGW(TAG, 
					"Waiting for notification from AnalogMeasurementTask");
			}

			for (int j = 0, k = i; j < 4; ++j, k += 8)
			{
				analog_readings[k] = analog_result->values[j];
			}

			while (xTaskNotifyWaitIndexed(DHT11_MEASUREMENTS_NOTIFICATION_INDEX,
				0, ULONG_MAX, (uint32_t*)&dht11_result, pdMS_TO_TICKS(60))
				== pdFALSE)
			{
				ESP_LOGW(TAG,
					"Waiting for notification from DHT11MeasurementTask");
			}

			dht11_results[i] = *dht11_result;
		}

		while (xTaskNotifyWaitIndexed(ONE_WIRE_MEASUREMENTS_NOTIFICATION_INDEX,
			0, ULONG_MAX, (uint32_t*)&onewire_result, pdMS_TO_TICKS(1000))
			== pdFALSE)
		{
			ESP_LOGW(TAG,
					"Waiting for notification from OneWireMeasurementTask");
		}

		message_length =  create_measurement_message(timestamp,
			station->configuration.mac_address, analog_readings, dht11_results,
			onewire_result, json, JSON_BUFFER_LEN);

		message_id = esp_mqtt_client_enqueue(station->mqtt_client,
			"measurements", json, message_length, 0, 0, true);
	
		if (message_id < 0)
		{
			ESP_LOGE(TAG, "failed to send measurements");
		}
	}
}

static void Station_ConfigurationReceiverTaskCode(void *pvParameters)
{
	while (1)
	{

	}
}

static int create_measurement_message(time_t timestamp, char *mac_address,
	int *analog_readings, DHT11MeasurementResult_t *dht11_results,
	OneWireMeasurementResult_t *onewire_result, char *buffer,
	size_t buffer_len)
{
	int pos = 0;
	pos += sprintf(&buffer[pos], "{\"timestamp\":%lld,", timestamp);
	pos += sprintf(&buffer[pos], "\"mac\":\"%s\",", mac_address);
	pos += sprintf(&buffer[pos], "\"analog\":[");
	for (int i = 0; i < 32; ++i)
	{
		pos += sprintf(&buffer[pos], "%d,", analog_readings[i]);
	}
	
	pos += sprintf(&buffer[pos], "],\"dht11\":[");
	for (int i = 0; i < 8; ++i)
	{
		if (!dht11_results[i].available)
		{
			continue;
		}
		DHT11_Reading_t *r = &dht11_results[i].reading;
		pos += sprintf(&buffer[pos],
			"{\"line\":%d,\"temp\":%hhu.%hhu,\"hum\":%hhu.%hhu},", i,
			r->temp_integral, r->temp_decimal, r->rh_integral, r->rh_decimal);
	}
	
	pos += sprintf(&buffer[pos], "],\"ds18b20\":[");
	for (int i = 0; i < onewire_result->num_readings; ++i)
	{
		pos += sprintf(&buffer[pos], "{\"addr\":%llu,\"temp\":%.2f},",
			onewire_result->readings[i].address,
			onewire_result->readings[i].temperature);
	}
	
	pos += sprintf(&buffer[pos], "]}");
	return pos + 1;
}
