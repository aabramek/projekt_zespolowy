#include "station.h"

#include "analog_measurements.h"
#include "dht11_measurements.h"
#include "one_wire_measurements.h"
#include "mux.h"
#include "watering.h"

#include "esp_mac.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "driver/i2c.h"
#include "driver/gpio.h"

#include "cJSON.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define WIFI_BITS_SET(bits) (bits & (EVT_WIFI_CONNECTED | EVT_WIFI_FAIL))

#define MQTT_BITS_SET(bits) (bits & (EVT_MQTT_CONNECTED | EVT_MQTT_FAIL)) 

static const char *TAG = "Station";

static portMUX_TYPE _spin_lock = portMUX_INITIALIZER_UNLOCKED;

// ============================= //
// STATIC FUNCTIONS DECLARATIONS //
// ============================= //
static esp_err_t peripherals_initialize(void);

static esp_err_t wifi_connect(StationHandle_t station);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

static void period_timer_callback(TimerHandle_t xTimer);

static void set_measurements_period(StationHandle_t station,
	uint8_t period);

static esp_err_t read_configuration(Configuration_t *configuration);

static uint8_t get_measurements_period(StationHandle_t station);

static void measurements_task_code(void *pvParameters);

static void configuration_receiver_task_code(void *pvParameters);

static esp_err_t create_tasks(StationHandle_t station);

static void mqtt_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data);

static bool mqtt_last_chunk_of_message(
	const esp_mqtt_event_handle_t event_data);

static int create_measurement_message(time_t timestamp, char *mac_address,
	int *analog_readings, DHT11MeasurementResult_t *dht11_results,
	OneWireMeasurementResult_t *onewire_result, char *buffer,
	size_t buffer_len);

// ===================== //
// FUNCTIONS DEFINITIONS //
// ===================== //
esp_err_t station_create(StationHandle_t *station)
{
	esp_err_t err = ESP_OK;
	StationHandle_t s = calloc(1, sizeof(Station_t));

	if (station == NULL)
	{
		err = ESP_FAIL;
		goto end;
	}

	uint8_t mac[6];
	err = esp_read_mac(mac, ESP_MAC_WIFI_STA);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read mac address: %s",
			esp_err_to_name(err));
		goto end;
	}

	sprintf(s->properties.mac_address, "%02X-%02X-%02X-%02X-%02X-%02X", 
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	s->properties.software_version = SOFTWARE_VERSION;
	
	s->event_group = xEventGroupCreate();
	if (s->event_group == NULL)
	{
		ESP_LOGE(TAG, "Failed to create event group");
		err = ESP_FAIL;
		goto end;
	}

	Configuration_t configuration;
	err = read_configuration(&configuration);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read configuration: %s",
			esp_err_to_name(err));
		goto end;
	}

	configuration.measurement_period = DEFAULT_MEASUREMENT_PERIOD;
	s->configuration = configuration;

	esp_mqtt_client_config_t mqtt_cfg = {
		.broker = {
			.address = {
				.transport = MQTT_TRANSPORT_OVER_TCP,
				.port = s->configuration.mqtt.port,
				.hostname = s->configuration.mqtt.host
			}
		},

		.session = {
			.disable_keepalive = true
		}
	};

	s->mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	if (s->mqtt_client == NULL)
	{
		ESP_LOGE(TAG, "Failed to create mqtt client");
		err = ESP_FAIL;
		goto end;
	}

	s->period_timer = xTimerCreate("Station period timer",
		pdMS_TO_TICKS(1000 * 60), pdTRUE, s, period_timer_callback);

	if (s->period_timer == NULL)
	{
		ESP_LOGE(TAG, "Failed to create period timer");
		err = ESP_FAIL;
		goto end;
	}

end:

	if (err == ESP_OK)
	{
		*station = s;
		return err;
	}

	if (s->event_group)
	{
		vEventGroupDelete(s->event_group);
	}

	if (s->period_timer)
	{
		xTimerDelete(s->period_timer, portMAX_DELAY);
	}

	if (s->mqtt_client)
	{
		esp_mqtt_client_destroy(s->mqtt_client);
	}

	free(s);

	return err;
}

esp_err_t station_start(StationHandle_t station)
{
	esp_err_t err = ESP_OK;

	err = peripherals_initialize();
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to initialize peripherals: %s",
			esp_err_to_name(err));
		goto end;
	}

	if (create_tasks(station) != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to start station tasks");
		err = ESP_FAIL;
		goto end;
	}

	err = wifi_connect(station);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to initialize wifi connection: %s",
			esp_err_to_name(err));
		goto end;
	}

	EventBits_t bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for wifi connection...");
		bits = xEventGroupWaitBits(station->event_group,
			EVT_WIFI_CONNECTED, pdTRUE, pdTRUE,
			WIFI_CONNECTION_ANTICIPATION_TIME);
	}
	while (!WIFI_BITS_SET(bits));

	if (bits & EVT_WIFI_FAIL)
	{
		ESP_LOGE(TAG, "Failed to connect to wifi");
		err = ESP_FAIL;
		goto end;
	}

	esp_sntp_config_t sntp_cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG(NTP_HOST);
	esp_netif_sntp_init(&sntp_cfg);
	while (esp_netif_sntp_sync_wait(NTP_ANTICIPATION_TIME) != ESP_OK)
	{
		ESP_LOGI(TAG, "Waiting for sntp sync...");
	}

	ESP_LOGI(TAG, "Received sntp time");

	err = esp_mqtt_client_register_event(station->mqtt_client,
		ESP_EVENT_ANY_ID, mqtt_event_handler, station);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to register mqtt event: %s",
			esp_err_to_name(err));
		goto end;
	}
	ESP_LOGI(TAG, "Registered mqtt event");

	err = esp_mqtt_client_start(station->mqtt_client);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to start mqtt client: %s", esp_err_to_name(err));
		goto end;
	}
	ESP_LOGI(TAG, "Started mqtt client");

	bits = 0;
	do
	{
		ESP_LOGI(TAG, "Waiting for mqtt client connection...");
		bits = xEventGroupWaitBits(station->event_group, EVT_MQTT_CONNECTED,
			pdTRUE, pdTRUE, MQTT_CONNECTION_ANTICIPATION_TIME);
	}
	while (!MQTT_BITS_SET(bits));

	if (bits & EVT_MQTT_FAIL)
	{
		ESP_LOGE(TAG, "Failed to connect to mqtt broker");
		err = ESP_FAIL;

		goto end;
	}

	ESP_LOGI(TAG, "Mqtt client connected");

	if (esp_mqtt_client_subscribe(station->mqtt_client,
		station->properties.mac_address, 0) == -1)
	{
		ESP_LOGE(TAG, "Failed to subscribe to configuration receiver topic");
		err = ESP_FAIL;
		goto end;
	}

	char register_msg[100];
	
	sprintf(register_msg, "{\"ip\":%lu,\"mac\":\"%s\",\"sv\":%d}",
		station->properties.ip_address, station->properties.mac_address, 
		station->properties.software_version);

	if (esp_mqtt_client_publish(station->mqtt_client, "register", register_msg,
		strlen(register_msg), 2, 0) == -1)
	{
		ESP_LOGE(TAG, "Failed to send registration message");
		err = ESP_FAIL;
		goto end;
	}

	do
	{
		bits = xEventGroupWaitBits(station->event_group, EVT_CONFIG_RECEIVED,
			pdFALSE, pdTRUE, CONFIGURATION_ANTICIPATION_TIME)
			& EVT_CONFIG_RECEIVED;
		
		ESP_LOGI(TAG, "Waiting for configuration...");
	}
	while (bits == 0);

	ESP_LOGI(TAG, "Registration message receved");

	while (xTimerStart(station->period_timer, PERIOD_TIMER_ANTICIPATION_TIME)
		== pdFAIL)
	{
		ESP_LOGW(TAG, "Waiting to start station period timer...");
	}

end:
	return err;
}

void station_dump_configuration(StationHandle_t station)
{
	ESP_LOGI(TAG,
		"Station configuration:\n"
		"ip: %lu.%lu.%lu.%lu\n"
		"mac: %s\n"
		"measurement_period: %u\n"
		"mqtt: %s:%lu\n",
		
		IP_TO_STRING(station->properties.ip_address),
		station->properties.mac_address,
		station->configuration.measurement_period,
		station->configuration.mqtt.host,
		station->configuration.mqtt.port);
}

// ============================ //
// STATIC FUNCTIONS DEFINITIONS //
// ============================ //
static esp_err_t create_tasks(StationHandle_t station)
{
	BaseType_t ret;
	ret = xTaskCreate(WateringTaskCode, "Watering task",
		WATERING_TASK_STACK_DEPTH, NULL, STATION_TASKS_PRIORITY,
		&station->tasks.watering);

	if (ret == pdFALSE)
	{
		ESP_LOGE(TAG, "Failed to create watering task");
		return ESP_FAIL;
	}

	ret = xTaskCreate(measurements_task_code, "Measurements task",
		MEASUREMENTS_TASK_STACK_DEPTH, station, STATION_TASKS_PRIORITY,
		&station->tasks.measurements);

	if (ret == pdFALSE)
	{
		ESP_LOGE(TAG, "Failed to create measurements task");
		return ESP_FAIL;
	}

	ret = xTaskCreate(configuration_receiver_task_code,
		"Configuration receiver task", CONFIGURATION_RECEIVER_TASK_STACK_DEPTH,
		station, STATION_TASKS_PRIORITY,
		&station->tasks.configuration_receiver);

	if (ret == pdFALSE)
	{
		ESP_LOGE(TAG, "Failed to create configuration receiver task");
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

	memcpy(wifi_cfg.sta.ssid, station->configuration.wifi.ssid, 32);
	memcpy(wifi_cfg.sta.password, station->configuration.wifi.password, 64);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
	ESP_ERROR_CHECK(esp_wifi_start());

	return ret;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
	int32_t event_id, void *event_data)
{
	static int retry_counter = 0;
	StationHandle_t station = (StationHandle_t)arg;
	if (event_base == WIFI_EVENT)
	{
		if (event_id == WIFI_EVENT_STA_START)
		{
			esp_wifi_connect();
		}
		else if (event_id == WIFI_EVENT_STA_CONNECTED)
		{
			retry_counter = 0;
		}
		else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
		{
			++retry_counter;
			if (retry_counter > WIFI_RECONNECT_ATTEMPTS)
			{
				xEventGroupSetBits(station->event_group,
					EVT_WIFI_FAIL);
			}
			else
			{
				esp_wifi_connect();
			}
		}
	}
	else
	{
		if (event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
       		station->properties.ip_address = event->ip_info.ip.addr;
			xEventGroupSetBits(station->event_group, EVT_WIFI_CONNECTED);
		}
	}
}

static void mqtt_event_handler(void *arg, esp_event_base_t event_base, 
	int32_t event_id, void *event_data)
{
	static char *buffer = NULL;
	static size_t buffer_size = 0;

	StationHandle_t station = (StationHandle_t)arg;
	esp_mqtt_event_handle_t mqtt_event = (esp_mqtt_event_handle_t)event_data;
	
	switch (mqtt_event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		xEventGroupSetBits(station->event_group, EVT_MQTT_CONNECTED);
		break;

	case MQTT_EVENT_DATA:
		if (mqtt_event->total_data_len > buffer_size)
		{
			buffer_size = mqtt_event->total_data_len;
			buffer = realloc(buffer, buffer_size);
		}
		
		memcpy(&buffer[mqtt_event->current_data_offset], mqtt_event->data,
			mqtt_event->data_len);
		
		if (mqtt_last_chunk_of_message(mqtt_event))
		{
			cJSON *json = cJSON_ParseWithLength(buffer, buffer_size);
			
			xTaskNotify(station->tasks.configuration_receiver,
				(uint32_t)json, eSetValueWithOverwrite);
		}
		
		break;

	case MQTT_EVENT_DISCONNECTED:
		xEventGroupSetBits(station->event_group, EVT_MQTT_FAIL);
		break;

	case MQTT_EVENT_ERROR:
			
		break;

	default:
		break;
	}
	
}

static bool mqtt_last_chunk_of_message(const esp_mqtt_event_handle_t event_data)
{
	return event_data->current_data_offset + event_data->data_len
		== event_data->total_data_len;
}

static void period_timer_callback(TimerHandle_t xTimer)
{
	static int minutes_elapsed = 0;
	
	StationHandle_t station = (StationHandle_t)pvTimerGetTimerID(xTimer);
	
	int period = get_measurements_period(station);
	
	if (++minutes_elapsed >= period)
	{
		minutes_elapsed = 0;
		xTaskNotifyGive(station->tasks.measurements);
	}

	xTaskNotifyGive(station->tasks.watering);
}

static void measurements_task_code(void *pvParameters)
{
	ESP_LOGI(TAG, "Measurements task started");

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
			station->properties.mac_address, analog_readings, dht11_results,
			onewire_result, json, JSON_BUFFER_LEN);

		message_id = esp_mqtt_client_enqueue(station->mqtt_client,
			"measurements", json, message_length, 0, 0, true);
	
		if (message_id < 0)
		{
			ESP_LOGE(TAG, "Failed to send measurements");
		}
	}
}

static void configuration_receiver_task_code(void *pvParameters)
{
	ESP_LOGI(TAG, "Configuration receiver task started");

	StationHandle_t station = (StationHandle_t)pvParameters;
	cJSON *json = NULL;

	while (1)
	{
		xTaskNotifyWait(0, 0, (uint32_t*)&json, portMAX_DELAY);
		if (json == NULL)
		{
			ESP_LOGW(TAG, "Malformed configuration message");
			continue;
		}
		cJSON *period = cJSON_GetObjectItemCaseSensitive(json, "period");
		if (period)
		{
			set_measurements_period(station,
				cJSON_GetNumberValue(period));
		}
		watering_configure(cJSON_GetObjectItemCaseSensitive(json, "valves"));
		cJSON_Delete(json);
		xEventGroupSetBits(station->event_group, EVT_CONFIG_RECEIVED);
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
	--pos;
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
		--pos;
	}
	
	pos += sprintf(&buffer[pos], "],\"ds18b20\":[");
	for (int i = 0; i < onewire_result->num_readings; ++i)
	{
		pos += sprintf(&buffer[pos], "{\"addr\":%llu,\"temp\":%.2f},",
			onewire_result->readings[i].address & 0xFFFFFFFFFFFFFF,
			onewire_result->readings[i].temperature);
	}
	--pos;

	pos += sprintf(&buffer[pos], "]}");
	return pos + 1;
}

static void set_measurements_period(StationHandle_t station,
	uint8_t period)
{
	taskENTER_CRITICAL(&_spin_lock);
	station->configuration.measurement_period = period;
	taskEXIT_CRITICAL(&_spin_lock);
}

static uint8_t get_measurements_period(StationHandle_t station)
{
	uint8_t p;
	taskENTER_CRITICAL(&_spin_lock);
	p = station->configuration.measurement_period;
	taskEXIT_CRITICAL(&_spin_lock);
	return p;
}

static esp_err_t read_configuration(Configuration_t *configuration)
{
	esp_err_t err = ESP_OK;
	nvs_handle_t nvs;
	Configuration_t cfg = {0};

	err = nvs_open("storage", NVS_READWRITE, &nvs);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to open nvs storage: %s", esp_err_to_name(err));
		goto end;
	}

	err = nvs_get_u32(nvs, "mqtt_port", &cfg.mqtt.port);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read mqtt_port from nvs: %s",
			esp_err_to_name(err));
		goto end;
	}

	size_t len;
	err = nvs_get_str(nvs, "mqtt_host", NULL, &len);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read mqtt_host length from nvs: %s",
			esp_err_to_name(err));
		goto end;
	}

	cfg.mqtt.host = malloc(len);
	if (cfg.mqtt.host == NULL)
	{
		ESP_LOGE(TAG, "Failed to allocate memory for mqtt_host property");
		goto end;
	}

	err = nvs_get_str(nvs, "mqtt_host", cfg.mqtt.host, &len);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read mqtt_host from nvs: %s",
			esp_err_to_name(err));
		goto end;
	}

	len = 32;
	err = nvs_get_str(nvs, "wifi_ssid", (char*)cfg.wifi.ssid, &len);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read wifi_ssid from nvs: %s",
			esp_err_to_name(err));
		goto end;
	}

	len = 64;
	err = nvs_get_str(nvs, "wifi_password",
		(char*)cfg.wifi.password, &len);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to read wifi_password from nvs: %s",
			esp_err_to_name(err));
		goto end;
	}

	*configuration = cfg;

end:
	if (err != ESP_OK)
	{
		free(cfg.mqtt.host);
	}
	nvs_close(nvs);
	return err;
}
