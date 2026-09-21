/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "app_types.h"
#include "sensor_task.h"
#include "input_task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Only run one test at a time*/
#define HW_BRINGUP_TEST_LED       0   /* 1 = isolated LED blink test only */
#define HW_BRINGUP_TEST_SWITCHES  0   /* 1 = run switch test, bypass RTOS. Set to 0 to resume normal startup. */
#define HW_BRINGUP_TEST_I2C_SCAN  0   /* 1 = run I2C scan test, bypass RTOS. Set to 0 to resume normal startup. */
#define HW_BRINGUP_TEST_LCD_MESSAGE 0 /* 1 = run LCD message test, bypass RTOS. Set to 0 to resume normal startup. */
#define HW_BRINGUP_TEST_RELAYS      0 /* 1 = run relay test, bypass RTOS. Set to 0 to resume normal startup. */
#define HW_BRINGUP_TEST_NTC         0 /* 1 = run NTC sensor test, bypass RTOS. Set to 0 to resume normal startup. */
#define HW_BRINGUP_TEST_UART_LOOPBACK 0  /* 1 = run UART loopback test, bypass RTOS. Set to 0 to resume normal startup. */

#if HW_BRINGUP_TEST_LED + HW_BRINGUP_TEST_SWITCHES + HW_BRINGUP_TEST_I2C_SCAN + HW_BRINGUP_TEST_LCD_MESSAGE + HW_BRINGUP_TEST_RELAYS + HW_BRINGUP_TEST_NTC > 1
#error "Only one HW_BRINGUP_TEST_* flag may be enabled at a time."
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* Definitions for SenseTask */
osThreadId_t SenseTaskHandle;
const osThreadAttr_t SenseTask_attributes = {
  .name = "SenseTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ConnectTask */
osThreadId_t ConnectTaskHandle;
const osThreadAttr_t ConnectTask_attributes = {
  .name = "ConnectTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for HeatTask */
osThreadId_t HeatTaskHandle;
const osThreadAttr_t HeatTask_attributes = {
  .name = "HeatTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh3,
};
/* Definitions for InputTask */
osThreadId_t InputTaskHandle;
const osThreadAttr_t InputTask_attributes = {
  .name = "InputTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for qSenseToHeat */
osMessageQueueId_t qSenseToHeatHandle;
const osMessageQueueAttr_t qSenseToHeat_attributes = {
  .name = "qSenseToHeat"
};
/* Definitions for qInputToDisplay */
osMessageQueueId_t qInputToDisplayHandle;
const osMessageQueueAttr_t qInputToDisplay_attributes = {
  .name = "qInputToDisplay"
};
/* Definitions for qDisplayToHeat */
osMessageQueueId_t qDisplayToHeatHandle;
const osMessageQueueAttr_t qDisplayToHeat_attributes = {
  .name = "qDisplayToHeat"
};
/* Definitions for qHeatToDisplay */
osMessageQueueId_t qHeatToDisplayHandle;
const osMessageQueueAttr_t qHeatToDisplay_attributes = {
  .name = "qHeatToDisplay"
};
/* Definitions for qUartRxToConnect */
osMessageQueueId_t qUartRxToConnectHandle;
const osMessageQueueAttr_t qUartRxToConnect_attributes = {
  .name = "qUartRxToConnect"
};
/* USER CODE BEGIN PV */
#if HW_BRINGUP_TEST_SWITCHES
volatile uint8_t g_upPressed    = 0;
volatile uint8_t g_downPressed  = 0;
volatile uint8_t g_modePressed  = 0;
volatile uint8_t g_enterPressed = 0;

#endif
#if HW_BRINGUP_TEST_I2C_SCAN
volatile uint8_t g_i2cFoundAddresses[16] = {0};
volatile uint8_t g_i2cFoundCount = 0;
#endif
#if HW_BRINGUP_TEST_RELAYS
volatile uint8_t g_heatRelayOn  = 0;
volatile uint8_t g_fanRelayOn   = 0;
volatile uint8_t g_humidRelayOn = 0;
#endif

#if HW_BRINGUP_TEST_NTC
volatile uint32_t g_ntcRawAdc         = 0;
volatile uint32_t g_ntcResistanceOhms = 0;
volatile float    g_ntcTempC          = 0.0f;
volatile float    g_ntcTempF          = 0.0f;
volatile uint8_t  g_ntcSensorFault    = 0;   /* 1 = open circuit / disconnected */
#endif

#if HW_BRINGUP_TEST_UART_LOOPBACK
volatile uint8_t          g_uartTxByte      = 0;
volatile uint8_t          g_uartRxByte      = 0;
volatile uint8_t          g_uartLoopbackOk  = 0;
volatile uint32_t         g_uartRxErrorCount = 0;
volatile HAL_StatusTypeDef g_uartTxResult;
volatile HAL_StatusTypeDef g_uartRxResult;
#endif

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
void StartSenseTask(void *argument);
void StartDisplayTask(void *argument);
void StartConnectTask(void *argument);
void StartHeatTask(void *argument);
void StartInputTask(void *argument);

/* USER CODE BEGIN PFP */
#if HW_BRINGUP_TEST_LCD_MESSAGE
static void lcd_i2c_write(uint8_t data);
static void lcd_pulse_enable(uint8_t data);
static void lcd_write4(uint8_t nibble, uint8_t rs);
static void lcd_send(uint8_t value, uint8_t rs);
static void lcd_command(uint8_t cmd);
static void lcd_data(uint8_t data);
static void lcd_init(void);
static void lcd_set_cursor(uint8_t col, uint8_t row);
static void lcd_print(const char *str);
static void lcd_backlight_on(void);
static void lcd_backlight_off(void);
#endif

#if HW_BRINGUP_TEST_NTC
static float ntc_resistance_to_celsius(float r_ohms);
#endif
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  #if HW_BRINGUP_TEST_LED
  while (1)
  {
    HAL_GPIO_WritePin(OnOffLed_GPIO_Port, OnOffLed_Pin, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(OnOffLed_GPIO_Port, OnOffLed_Pin, GPIO_PIN_RESET);
    HAL_Delay(1000);
  }
#endif

  #if HW_BRINGUP_TEST_SWITCHES
  while (1)
  {
    g_upPressed    = (HAL_GPIO_ReadPin(UpArrowSwitch_GPIO_Port,   UpArrowSwitch_Pin)   == GPIO_PIN_RESET);
    g_downPressed  = (HAL_GPIO_ReadPin(DownArrowSwitch_GPIO_Port, DownArrowSwitch_Pin) == GPIO_PIN_RESET);
    g_modePressed  = (HAL_GPIO_ReadPin(ModeSwitch_GPIO_Port,      ModeSwitch_Pin)      == GPIO_PIN_RESET);
    g_enterPressed = (HAL_GPIO_ReadPin(EnterSwitch_GPIO_Port,     EnterSwitch_Pin)     == GPIO_PIN_RESET);

    if (g_upPressed || g_downPressed || g_modePressed || g_enterPressed)
    {
      HAL_GPIO_WritePin(OnOffLed_GPIO_Port, OnOffLed_Pin, GPIO_PIN_SET);   /* any press -> LED on */
    }
    else
    {
      HAL_GPIO_WritePin(OnOffLed_GPIO_Port, OnOffLed_Pin, GPIO_PIN_RESET); /* nothing pressed -> LED off */
    }
  }
#endif

#if HW_BRINGUP_TEST_I2C_SCAN
  for (uint8_t addr = 0x08; addr <= 0x77; addr++)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 2, 5) == HAL_OK)
    {
      if (g_i2cFoundCount < 16)
      {
        g_i2cFoundAddresses[g_i2cFoundCount] = addr;
        g_i2cFoundCount++;
      }
    }
  }

  while (1)
  {
    /* Halt here — inspect g_i2cFoundAddresses / g_i2cFoundCount in Live Watch. */
  }
#endif

#if HW_BRINGUP_TEST_LCD_MESSAGE
  lcd_init();
  lcd_set_cursor(0, 0);
  lcd_print("Proofing Oven");
  lcd_set_cursor(0, 1);
  lcd_print("LCD Test OK");

  while (1)
  {
    lcd_backlight_on();
    HAL_Delay(1000);
    lcd_backlight_off();
    HAL_Delay(1000);
  }
#endif

#if HW_BRINGUP_TEST_RELAYS
  while (1)
  {
    uint8_t upPressed    = (HAL_GPIO_ReadPin(UpArrowSwitch_GPIO_Port,   UpArrowSwitch_Pin)   == GPIO_PIN_RESET);
    uint8_t downPressed  = (HAL_GPIO_ReadPin(DownArrowSwitch_GPIO_Port, DownArrowSwitch_Pin) == GPIO_PIN_RESET);
    uint8_t modePressed  = (HAL_GPIO_ReadPin(ModeSwitch_GPIO_Port,      ModeSwitch_Pin)      == GPIO_PIN_RESET);
    uint8_t enterPressed = (HAL_GPIO_ReadPin(EnterSwitch_GPIO_Port,     EnterSwitch_Pin)      == GPIO_PIN_RESET);

    if (enterPressed)
    {
      /* Panic override: force everything off, ignore other switches this pass. */
      upPressed   = 0;
      downPressed = 0;
      modePressed = 0;
    }

    g_heatRelayOn  = upPressed;
    g_fanRelayOn   = downPressed;
    g_humidRelayOn = modePressed;

    HAL_GPIO_WritePin(HeatRelay_GPIO_Port,  HeatRelay_Pin,  g_heatRelayOn  ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(FanRelay_GPIO_Port,   FanRelay_Pin,   g_fanRelayOn   ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(HumidRelay_GPIO_Port, HumidRelay_Pin, g_humidRelayOn ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }
#endif

#if HW_BRINGUP_TEST_NTC
  while (1)
  {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    g_ntcRawAdc = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    if (g_ntcRawAdc >= 4090)   /* near ADC_MAX -> open circuit / disconnected sensor */
    {
      g_ntcSensorFault = 1;
    }
    else
    {
      g_ntcSensorFault = 0;

      /* R_ntc = R1 * ADC / (ADC_MAX - ADC), R1 = 10K, ADC_MAX = 4095 for 12-bit */
      g_ntcResistanceOhms = (uint32_t)(10000.0f * (float)g_ntcRawAdc / (float)(4095 - g_ntcRawAdc));

      g_ntcTempC = ntc_resistance_to_celsius((float)g_ntcResistanceOhms);
      g_ntcTempF = (g_ntcTempC * 9.0f / 5.0f) + 32.0f;
    }

    HAL_Delay(500);
  }
#endif

#if HW_BRINGUP_TEST_UART_LOOPBACK
  while (1)
  {
    g_uartTxByte = 0x55;

    g_uartTxResult = HAL_UART_Transmit(&huart2, (uint8_t *)&g_uartTxByte, 1, 100);
    g_uartRxResult = HAL_UART_Receive(&huart2, (uint8_t *)&g_uartRxByte, 1, 100);

    if (g_uartRxResult == HAL_OK)
    {
      g_uartLoopbackOk = (g_uartRxByte == g_uartTxByte);
    }
    else
    {
      g_uartLoopbackOk = 0;
      g_uartRxErrorCount++;
    }

    HAL_Delay(500);
  }
#endif

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qSenseToHeat */
  qSenseToHeatHandle = osMessageQueueNew (1, sizeof(uint16_t), &qSenseToHeat_attributes);

  /* creation of qInputToDisplay */
  qInputToDisplayHandle = osMessageQueueNew (8, sizeof(uint8_t), &qInputToDisplay_attributes);

  /* creation of qDisplayToHeat */
  qDisplayToHeatHandle = osMessageQueueNew (4, sizeof(HeatCommand_t), &qDisplayToHeat_attributes);

  /* creation of qHeatToDisplay */
  qHeatToDisplayHandle = osMessageQueueNew (1, sizeof(HeatStatus_t), &qHeatToDisplay_attributes);

  /* creation of qUartRxToConnect */
  qUartRxToConnectHandle = osMessageQueueNew (1, sizeof(uint8_t), &qUartRxToConnect_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SenseTask */
  SenseTaskHandle = osThreadNew(StartSenseTask, NULL, &SenseTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of ConnectTask */
  ConnectTaskHandle = osThreadNew(StartConnectTask, NULL, &ConnectTask_attributes);

  /* creation of HeatTask */
  HeatTaskHandle = osThreadNew(StartHeatTask, NULL, &HeatTask_attributes);

  /* creation of InputTask */
  InputTaskHandle = osThreadNew(StartInputTask, NULL, &InputTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, HeatRelay_Pin|FanRelay_Pin|HumidRelay_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OnOffLed_Pin|OnOffHeatLed_Pin|OnOffHumidLed_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : HeatRelay_Pin FanRelay_Pin HumidRelay_Pin OnOffLed_Pin
                           OnOffHeatLed_Pin OnOffHumidLed_Pin */
  GPIO_InitStruct.Pin = HeatRelay_Pin|FanRelay_Pin|HumidRelay_Pin|OnOffLed_Pin
                          |OnOffHeatLed_Pin|OnOffHumidLed_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : UpArrowSwitch_Pin DownArrowSwitch_Pin ModeSwitch_Pin EnterSwitch_Pin */
  GPIO_InitStruct.Pin = UpArrowSwitch_Pin|DownArrowSwitch_Pin|ModeSwitch_Pin|EnterSwitch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* Relay PCBA is active-low (opto-isolated, IN=LOW energizes the coil).
     CubeMX's auto-generated Output Level default for these pins is
     GPIO_PIN_RESET (LOW), which energizes all three relays at boot.
     Force them HIGH (de-energized) here, immediately after HAL_GPIO_Init
     configures them as outputs, until this is fixed at the source by
     setting each pin's individual GPIO output level in the CubeMX
     Pinout view. */
  HAL_GPIO_WritePin(GPIOA, HeatRelay_Pin|FanRelay_Pin|HumidRelay_Pin, GPIO_PIN_SET);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#if HW_BRINGUP_TEST_LCD_MESSAGE

#define LCD_I2C_ADDR   (0x27 << 1)
#define LCD_BACKLIGHT  0x08
#define LCD_ENABLE_BIT 0x04
#define LCD_RS_BIT     0x01

static void lcd_i2c_write(uint8_t data)
{
  HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, &data, 1, 10);
}

static void lcd_pulse_enable(uint8_t data)
{
  lcd_i2c_write(data | LCD_ENABLE_BIT);
  HAL_Delay(1);
  lcd_i2c_write(data & (uint8_t)~LCD_ENABLE_BIT);
  HAL_Delay(1);
}
static uint8_t lcd_backlight_state = LCD_BACKLIGHT;   /* current backlight bit, ORed into every write */
static void lcd_write4(uint8_t nibble, uint8_t rs)
{
  uint8_t data = (uint8_t)((nibble << 4) | lcd_backlight_state | rs);
  lcd_i2c_write(data);
  lcd_pulse_enable(data);
}

static void lcd_send(uint8_t value, uint8_t rs)
{
  lcd_write4((uint8_t)(value >> 4), rs);
  lcd_write4((uint8_t)(value & 0x0F), rs);
}

static void lcd_command(uint8_t cmd) { lcd_send(cmd, 0); }
static void lcd_data(uint8_t data)   { lcd_send(data, LCD_RS_BIT); }

static void lcd_init(void)
{
  HAL_Delay(50);           /* power-on settle */

  lcd_write4(0x03, 0);     /* force 8-bit mode, unknown-state reset sequence */
  HAL_Delay(5);
  lcd_write4(0x03, 0);
  HAL_Delay(1);
  lcd_write4(0x03, 0);
  HAL_Delay(1);

  lcd_write4(0x02, 0);     /* switch to 4-bit mode */
  HAL_Delay(1);

  lcd_command(0x28);       /* function set: 4-bit, 2 line, 5x8 font */
  lcd_command(0x0C);       /* display ON, cursor off, blink off */
  lcd_command(0x06);       /* entry mode: increment, no shift */
  lcd_command(0x01);       /* clear display */
  HAL_Delay(2);            /* clear needs >1.6ms, generously covered */
}

static void lcd_set_cursor(uint8_t col, uint8_t row)
{
  uint8_t rowOffset = (row == 0) ? 0x00 : 0x40;
  lcd_command((uint8_t)(0x80 | (col + rowOffset)));
}

static void lcd_print(const char *str)
{
  while (*str) { lcd_data((uint8_t)(*str)); str++; }
}

static void lcd_backlight_on(void)
{
  lcd_backlight_state = LCD_BACKLIGHT;
  lcd_i2c_write(lcd_backlight_state);   /* backlight is just a static output pin — no Enable pulse needed */
}

static void lcd_backlight_off(void)
{
  lcd_backlight_state = 0x00;
  lcd_i2c_write(lcd_backlight_state);
}

#endif /* HW_BRINGUP_TEST_LCD_MESSAGE */

#if HW_BRINGUP_TEST_NTC
static float ntc_resistance_to_celsius(float r_ohms)
{
  const float R25   = 10000.0f;
  const float BETA  = 3950.0f;
  const float T25_K = 298.15f;

  float tempK = 1.0f / ((1.0f / T25_K) + (1.0f / BETA) * logf(r_ohms / R25));
  return tempK - 273.15f;
}
#endif

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartSenseTask */
/**
  * @brief  Function implementing the SenseTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSenseTask */
void StartSenseTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  SenseTask_Run(argument);
  /* USER CODE END 5 */
}
/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  uint8_t event;

  (void)argument;

  for (;;)
  {
    if (osMessageQueueGet(qInputToDisplayHandle,
                          &event, NULL, 100U) == osOK)
    {
      g_lastInputEvent = event;
      ++g_inputEventCount;
    }
  }
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartConnectTask */
/**
* @brief Function implementing the ConnectTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartConnectTask */
void StartConnectTask(void *argument)
{
  /* USER CODE BEGIN StartConnectTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartConnectTask */
}

/* USER CODE BEGIN Header_StartHeatTask */
/**
* @brief Function implementing the HeatTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHeatTask */
void StartHeatTask(void *argument)
{
  /* USER CODE BEGIN StartHeatTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartHeatTask */
}

/* USER CODE BEGIN Header_StartInputTask */
/**
* @brief Function implementing the InputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInputTask */
void StartInputTask(void *argument)
{
  /* USER CODE BEGIN StartInputTask */
  InputTask_Run(argument);
  /* USER CODE END StartInputTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
