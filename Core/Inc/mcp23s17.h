#ifndef MCP23S17_H
#define MCP23S17_H

#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

// Définition des registres MCP23S17
#define MCP23S17_OPCODE_WRITE  0x40
#define MCP23S17_IODIRA        0x00
#define MCP23S17_IODIRB        0x01
#define MCP23S17_GPIOA         0x12
#define MCP23S17_GPIOB         0x13

// Structure de configuration du MCP23S17
typedef struct {
	SPI_HandleTypeDef *hspi;     // SPI utilisé
	GPIO_TypeDef *CS_Port;       // Port Chip Select
	uint16_t CS_Pin;             // Pin Chip Select
} MCP23S17_HandleTypeDef;

// Prototypes des fonctions
void MCP23S17_Init(MCP23S17_HandleTypeDef *dev);
void MCP23S17_SetLED(MCP23S17_HandleTypeDef *dev, uint8_t gpio, uint8_t led);
void MCP23S17_ClearAllLEDs(MCP23S17_HandleTypeDef *dev);
void MCP23S17_Chenillard(MCP23S17_HandleTypeDef *dev, uint8_t delay_ms);

#endif // MCP23S17_H
