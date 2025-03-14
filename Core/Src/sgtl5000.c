#include "sgtl5000.h"
#include "stdio.h"

// Fonction d'écriture d'un registre
HAL_StatusTypeDef SGTL5000_WriteRegister(uint16_t reg, uint16_t value) {
    uint8_t data[2];
    data[0] = (value >> 8) & 0xFF;  // MSB
    data[1] = value & 0xFF;         // LSB

    return HAL_I2C_Mem_Write(&hi2c2, (SGTL5000_I2C_ADDRESS << 1), reg,
                             I2C_MEMADD_SIZE_16BIT, data, 2, HAL_MAX_DELAY);
}

// Fonction de lecture d'un registre
HAL_StatusTypeDef SGTL5000_ReadRegister(uint16_t reg, uint16_t *value) {
    uint8_t data[2];
    if (HAL_I2C_Mem_Read(&hi2c2, (SGTL5000_I2C_ADDRESS << 1), reg,
                         I2C_MEMADD_SIZE_16BIT, data, 2, HAL_MAX_DELAY) == HAL_OK) {
        *value = (data[0] << 8) | data[1];
        return HAL_OK;
    }
    return HAL_ERROR;
}

// Fonction d'initialisation du SGTL5000
void SGTL5000_Init(void) {
    printf("Initialisation du SGTL5000...\r\n");

    if (SGTL5000_WriteRegister(CHIP_LINREG_CTRL, 0x0008) != HAL_OK)
        printf("Erreur configuration CHIP_LINREG_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_ANA_POWER, 0x6AFF) != HAL_OK)
        printf("Erreur configuration CHIP_ANA_POWER\r\n");

    if (SGTL5000_WriteRegister(CHIP_REF_CTRL, 0x004E) != HAL_OK)
        printf("Erreur configuration CHIP_REF_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_LINE_OUT_CTRL, 0x0322) != HAL_OK)
        printf("Erreur configuration CHIP_LINE_OUT_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_SHORT_CTRL, 0x1106) != HAL_OK)
        printf("Erreur configuration CHIP_SHORT_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_ANA_CTRL, 0x0133) != HAL_OK)
        printf("Erreur configuration CHIP_ANA_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_DIG_POWER, 0x0073) != HAL_OK)
        printf("Erreur configuration CHIP_DIG_POWER\r\n");

    if (SGTL5000_WriteRegister(CHIP_LINE_OUT_VOL, 0x0505) != HAL_OK)
        printf("Erreur configuration CHIP_LINE_OUT_VOL\r\n");

    if (SGTL5000_WriteRegister(CHIP_CLK_CTRL, 0x0008) != HAL_OK)
        printf("Erreur configuration CHIP_CLK_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_I2S_CTRL, 0x0010) != HAL_OK)
        printf("Erreur configuration CHIP_I2S_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_ADCDAC_CTRL, 0x0000) != HAL_OK)
        printf("Erreur configuration CHIP_ADCDAC_CTRL\r\n");

    if (SGTL5000_WriteRegister(CHIP_DAC_VOL, 0x3C3C) != HAL_OK)
        printf("Erreur configuration CHIP_DAC_VOL\r\n");

    printf("SGTL5000 initialisé avec succès !\r\n");
}
