/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "sai.h"
#include "spi.h"
#include "usart.h"

#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "mcp23s17.h"
#include "sgtl5000.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
MCP23S17_HandleTypeDef mcp23s17;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CODEC_I2C_ADDRESS  0x0A // Adresse I2C du CODEC
#define CHIP_ID_REG        0x0000 // Adresse du registre CHIP_ID
#define AUDIO_BUFFER_SIZE  256  // Taille du buffer audio
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t rxBuffer[AUDIO_BUFFER_SIZE];
uint16_t txBuffer[AUDIO_BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int chr)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&chr, 1, HAL_MAX_DELAY);
	return chr;
}

void Read_CODEC_ChipID(void) {
	uint8_t chip_id[2] = {0}; // Stockage du registre CHIP_ID (16 bits)

	// Lecture du registre CHIP_ID
	if (HAL_I2C_Mem_Read(&hi2c2, (CODEC_I2C_ADDRESS << 1), CHIP_ID_REG,
			I2C_MEMADD_SIZE_16BIT, chip_id, 2, HAL_MAX_DELAY) == HAL_OK) {
		uint16_t chip_id_value = (chip_id[0] << 8) | chip_id[1];
		printf("CHIP ID: 0x%04X\r\n", chip_id_value);
	} else {
		printf("Erreur lors de la lecture du CHIP ID\r\n");
	}
}

void Start_SAI_Audio(void) {
	HAL_StatusTypeDef status_tx = HAL_SAI_Transmit_DMA(&hsai_BlockA2, (uint8_t*)txBuffer, AUDIO_BUFFER_SIZE);

	if (status_tx != HAL_OK) {
		printf("Erreur SAI TX: %d\r\n", status_tx);
	} else {
		printf("SAI TX démarré avec succès\r\n");
	}

	if (HAL_SAI_Receive_DMA(&hsai_BlockB2, (uint8_t*)rxBuffer, AUDIO_BUFFER_SIZE) != HAL_OK) {
		printf("Erreur démarrage SAI RX\r\n");
	} else {
		printf("SAI RX démarré\r\n");
	}
}

void Fill_Triangle_Wave(uint16_t *buffer, int length) {
	int cycles = 10; // 10 cycles dans le buffer
	int samples_per_cycle = length / cycles;
	int half = samples_per_cycle / 2;

	for (int i = 0; i < length; i++) {
		int pos = i % samples_per_cycle;
		int16_t val = (pos < half)
			? (-0x3FFF + (2 * 0x3FFF * pos) / half)
			: (0x3FFF - (2 * 0x3FFF * (pos - half)) / half);
		buffer[i] = (uint16_t)(val + 0x8000); // Décalage pour DAC
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_SPI3_Init();
	MX_I2C2_Init();
	MX_SAI2_Init();
	/* USER CODE BEGIN 2 */
	__HAL_SAI_ENABLE(&hsai_BlockA2);
	HAL_Delay(10);
	SGTL5000_Init();
	mcp23s17.hspi = &hspi3;
	mcp23s17.CS_Port = SPI3_nCS_GPIO_Port;
	mcp23s17.CS_Pin = SPI3_nCS_Pin;
	MCP23S17_Init(&mcp23s17);

	Read_CODEC_ChipID();

	Fill_Triangle_Wave(txBuffer, AUDIO_BUFFER_SIZE);
	for (int i = 0; i < 10; i++) {
		printf("txBuffer[%d] = 0x%04X\r\n", i, txBuffer[i]);
	}
	printf("Dernier échantillon : txBuffer[%d] = 0x%04X\r\n", AUDIO_BUFFER_SIZE - 1, txBuffer[AUDIO_BUFFER_SIZE - 1]);
	Start_SAI_Audio();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(100);
		printf("Test printf bien\r\n");

		MCP23S17_Chenillard(&mcp23s17, 200);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
	PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
	PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
	PeriphClkInit.PLLSAI1.PLLSAI1N = 13;
	PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV17;
	PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
