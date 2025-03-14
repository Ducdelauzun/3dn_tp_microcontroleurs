#ifndef SGTL5000_H
#define SGTL5000_H

#include "i2c.h"

#define SGTL5000_I2C_ADDRESS 0x0A  // Adresse I2C du SGTL5000

// Adresses des registres
#define CHIP_ANA_POWER      0x0030
#define CHIP_LINREG_CTRL    0x0026
#define CHIP_REF_CTRL       0x0028
#define CHIP_LINE_OUT_CTRL  0x002C
#define CHIP_SHORT_CTRL     0x003C
#define CHIP_ANA_CTRL       0x0024
#define CHIP_DIG_POWER      0x0002
#define CHIP_LINE_OUT_VOL   0x002E
#define CHIP_CLK_CTRL       0x0004
#define CHIP_I2S_CTRL       0x0006
#define CHIP_ADCDAC_CTRL    0x000E
#define CHIP_DAC_VOL        0x0010

// Prototypes des fonctions
void SGTL5000_Init(void);
HAL_StatusTypeDef SGTL5000_WriteRegister(uint16_t reg, uint16_t value);
HAL_StatusTypeDef SGTL5000_ReadRegister(uint16_t reg, uint16_t *value);

#endif // SGTL5000_H
