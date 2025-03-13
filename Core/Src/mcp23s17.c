#include "mcp23s17.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

// Écriture d'un registre du MCP23S17
static void MCP23S17_WriteRegister(MCP23S17_HandleTypeDef *dev, uint8_t reg, uint8_t value) {
	uint8_t data[3] = {MCP23S17_OPCODE_WRITE, reg, value};
	HAL_GPIO_WritePin(dev->CS_Port, dev->CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(dev->hspi, data, 3, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(dev->CS_Port, dev->CS_Pin, GPIO_PIN_SET);
}

// Initialisation du MCP23S17
void MCP23S17_Init(MCP23S17_HandleTypeDef *dev) {
	MCP23S17_WriteRegister(dev, MCP23S17_IODIRA, 0x00);  // GPA en sortie
	MCP23S17_WriteRegister(dev, MCP23S17_IODIRB, 0x00);  // GPB en sortie
	MCP23S17_ClearAllLEDs(dev);
}

void MCP23S17_SetLED(MCP23S17_HandleTypeDef *dev, uint8_t gpio, uint8_t led) {
	// Éteindre toutes les LEDs du port en cours
	if (gpio == 0) {
		MCP23S17_WriteRegister(dev, MCP23S17_GPIOA, (1 << led));  // Allume une seule LED sur GPA
		MCP23S17_WriteRegister(dev, MCP23S17_GPIOB, 0x00);        // Éteint toutes les LEDs sur GPB
	} else {
		MCP23S17_WriteRegister(dev, MCP23S17_GPIOB, (1 << led));  // Allume une seule LED sur GPB
		MCP23S17_WriteRegister(dev, MCP23S17_GPIOA, 0x00);        // Éteint toutes les LEDs sur GPA
	}
}

// Éteindre toutes les LEDs
void MCP23S17_ClearAllLEDs(MCP23S17_HandleTypeDef *dev) {
	MCP23S17_WriteRegister(dev, MCP23S17_GPIOA, 0x00);
	MCP23S17_WriteRegister(dev, MCP23S17_GPIOB, 0x00);
}

void MCP23S17_Chenillard(MCP23S17_HandleTypeDef *dev, uint8_t delay_ms) {
	uint8_t step = 0;
	uint8_t gpio = 0;

	while (1) {
		MCP23S17_ClearAllLEDs(dev);  // Éteindre toutes les LEDs avant d'en allumer une

		MCP23S17_SetLED(dev, gpio, step);
		HAL_Delay(delay_ms);

		if (gpio == 0) {
			step++;
			if (step > 7) {
				step = 7;
				gpio = 1;
			}
		} else {
			if (step == 0) {
				gpio = 0;
			} else {
				step--;
			}
		}
	}
}
