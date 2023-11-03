#ifndef ADS1115_H
#define ADS1115_H

#define CONVERSION_REG 0
#define CONFIG_REG 1
#define LO_THRESH_REG 2
#define HI_THRESH_REG 3

#define OS_BIT 15
#define MUX_MASK 0x8FFF
#define MUX_BITS 12
#define PGA_MASK 0xF1FF
#define PGA_BITS 9 
#define MODE_BIT 8
#define DATA_RATE_MASK 0xFF1F
#define DATA_RATE_BITS 5
#define COMP_MODE_BIT 4
#define COMP_POL_BIT 3
#define COMP_LAT_BIT 2
#define COMP_QUEUE_MASK 0xFFFC
#define COMP_QUEUE_BITS 0

#define MUX_ADDR_SINGLE_ENDED(addr) (addr | 0b100)

#define DEFAULT_CONFIG_REGISTER_VALUE 0x8583

#define PGA_6_144V 0
#define PGA_4_096V 1
#define PGA_2_048V 2
#define PGA_1_024V 3
#define PGA_0_512V 4
#define PGA_0_256V 5

#define DATA_RATE_8		0
#define DATA_RATE_16	1
#define DATA_RATE_32	2
#define DATA_RATE_64	3
#define DATA_RATE_128	4
#define DATA_RATE_250	5
#define DATA_RATE_475	6
#define DATA_RATE_860	7

#include "esp_check.h"

typedef struct
{
	uint16_t configuration;
	uint8_t address;
}
ADS1115_t;

esp_err_t ADS1115_Init(ADS1115_t *ads1115);

esp_err_t ADS1115_WriteRegister(ADS1115_t *ads1115, uint8_t register_address, uint16_t value);

esp_err_t ADS1115_ReadRegister(ADS1115_t *ads1115, int8_t register_address, uint16_t *value);

esp_err_t ADS1115_AddressMux(ADS1115_t *ads1115, uint8_t addr);

esp_err_t ADS1115_SetPGA(ADS1115_t *ads1115, uint8_t pga);

esp_err_t ADS1115_StartConversion(ADS1115_t *ads1115);

esp_err_t ADS1115_ComparatorEnable(ADS1115_t *ads1115);

esp_err_t ADS1115_SetDataRate(ADS1115_t *ads1115, uint8_t data_rate);

#endif
