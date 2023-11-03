#ifndef ADS1115_H
#define ADS1115_H

#define CONVERSION_REG 0
#define CONFIG_REG 1
#define LO_THRESH_REG 2
#define HI_THRESH_REG 3

#define MUX_MASK 0x8FFF
#define PGA_MASK 0xF1FF
#define DATA_RATE_MASK 0xFF1F
#define COMP_QUEUE_MASK 0xFFFC

#define DEFAULT_CONFIG_REGISTER_VALUE 0x8583

#define MUX_ADDR_A0 0x4000
#define MUX_ADDR_A1 0x5000
#define MUX_ADDR_A2 0x6000
#define MUX_ADDR_A3 0x7000

#define PGA_6_144V 0
#define PGA_4_096V 0x200
#define PGA_2_048V 0x400
#define PGA_1_024V 0x600
#define PGA_0_512V 0x800
#define PGA_0_256V 0xA00

#define DATA_RATE_8		0
#define DATA_RATE_16	0x20
#define DATA_RATE_32	0x40
#define DATA_RATE_64	0x60
#define DATA_RATE_128	0x80
#define DATA_RATE_250	0xA0
#define DATA_RATE_475	0xC0
#define DATA_RATE_860	0xE0

#define COMP_QUEUE_1		0x0
#define COMP_QUEUE_2		0x1
#define COMP_QUEUE_4		0x2
#define COMP_QUEUE_DISABLE	0x3

#define OS_BIT 15

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

esp_err_t ADS1115_AddressMux(ADS1115_t *ads1115, uint16_t addr);

esp_err_t ADS1115_SetPGA(ADS1115_t *ads1115, uint16_t pga);

esp_err_t ADS1115_StartConversion(ADS1115_t *ads1115);

esp_err_t ADS1115_ComparatorEnable(ADS1115_t *ads1115);

esp_err_t ADS1115_SetDataRate(ADS1115_t *ads1115, uint16_t data_rate);

#endif
