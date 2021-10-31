/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include "bme280.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int* intTemp; //Contains live temperature data
int* intHum; //Contains live humidity data
int* intPress; // Contains live air pressure data
int intError; // contains live error code
char time[10][9] = {"", "", "", "", "", "", "", "", "", ""}; // Contains time from measurement (2D array)
char NTPdateTime[] = ""; // Contains internet time
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* Definitions for sendESPtask */
osThreadId_t sendESPtaskHandle;
const osThreadAttr_t sendESPtask_attributes = {
  .name = "sendESPtask",
  .stack_size = 300 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for readDataTask */
osThreadId_t readDataTaskHandle;
const osThreadAttr_t readDataTask_attributes = {
  .name = "readDataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for getESPtimeTask */
osThreadId_t getESPtimeTaskHandle;
const osThreadAttr_t getESPtimeTask_attributes = {
  .name = "getESPtimeTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
void sendESP(void *argument);
void readData(void *argument);
void getESPtime(void *argument);

/* USER CODE BEGIN PFP */

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
  //Reset sensor data.
  //memory allocation
  intTemp = (int*)calloc(10, sizeof(int));
  intHum = (int*)calloc(10, sizeof(int));
  intPress = (int*)calloc(10, sizeof(int));
  //senDT = calloc(10, sizeof(*senDT));
  intError = 0;
  /* ERRORS
   *  1 = ESP-ERROR
   *
   */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  connectESPtoWifi();
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of sendESPtask */
  sendESPtaskHandle = osThreadNew(sendESP, NULL, &sendESPtask_attributes);

  /* creation of readDataTask */
  readDataTaskHandle = osThreadNew(readData, NULL, &readDataTask_attributes);

  /* creation of getESPtimeTask */
  getESPtimeTaskHandle = osThreadNew(getESPtime, NULL, &getESPtimeTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.Timing = 0x2000090E;
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x20303E5D;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void debugPrint(UART_HandleTypeDef *huart, char _out[]){
 HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
}

void debugPrintln(UART_HandleTypeDef *huart, char _out[]){
 HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
 char newline[2] = "\r\n";
 HAL_UART_Transmit(huart, (uint8_t *) newline, 2, 10);
}

char *sendToESP(char *data)
{
	char rxData[50] = {0}; //Contains data send from the ESP over UART1
	char *str = malloc(50);
	debugPrintln(&huart2, data); //Print end result AT-command for debugging
	HAL_UART_Transmit(&huart1, (uint8_t *) data, strlen(data), 100); //Send AT-command
	HAL_UART_Receive(&huart1, (uint8_t *)rxData, 50, 100); //Get response (like OK or ERROR)
	//HAL_UART_Transmit(&huart2, (uint8_t*)rxData, strlen(rxData) , 100); //Print response for debugging
	//debugPrintln(&huart2, "msg: \n"); // Message for debugging
	debugPrintln(&huart2, rxData); // Message for debugging
	//debugPrintln(&huart2, "\n"); // Message for debugging
	for(int i = 0; i < 51; i++){
		str[i] = rxData[i];
	}
	free(str);
    return str;

}

void connectESPtoWifi()
{
	char resetESP[] = "AT+RST\r\n";
	char disableEcho[] = "ATE0\r\n";
	char checkWifiConnected[] = "AT+CWJAP?\r\n";
	char setWifiMode[] = "AT+CWMODE=1\r\n";
	char connectToAP[] = "AT+CWJAP=\"Hammer-Network_Mobile\",\"5652HN7327\"\r\n";
	char response[50];

	strcpy(response, sendToESP(resetESP));
	strcpy(response, sendToESP(disableEcho));

	//Check if connected to Wifi
	strcpy(response, sendToESP(checkWifiConnected));
	if (strstr(response, "No AP") != NULL)
	{
		//Check if there was an error
		strcpy(response, sendToESP(setWifiMode));
		if (strstr(response, "ERROR") != NULL)
		{
			intError = 1; //change error code to '1' for ESP related error
			//debugPrintln(&huart2, "ERROR1"); // Message for debugging
		}
		//Check if there was an error
		strcpy(response, sendToESP(connectToAP));
		if (strstr(response, "ERROR") != NULL)
		{
			intError = 1; //change error code to '1' for ESP related error
			//debugPrintln(&huart2, "ERROR1"); // Message for debugging
		}
		else
		{
			//debugPrintln(&huart2, response); // Message for debugging
		}
	}
}

/* USER CODE END 4 */


/* USER CODE BEGIN Header_sendESP */
/**
  * @brief  Function implementing the sendESPtask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_sendESP */
void sendESP(void *argument)
{
  /* USER CODE BEGIN 5 */
	debugPrintln(&huart2, "sendESP FUNC \n"); // Message for debugging
  /* Infinite loop */
	//Local var. declaration.
	char response[10]; //Containt data send from the ESP over UART1
	char initCon[] = "AT+CIPSTART=\"TCP\",\"server03.hammer-tech.eu\",80";
	char initSize[] = "AT+CIPSEND=177";
	char data[] = "POST /weerstation/index.php HTTP/1.1r\nHost: server03.hammer-tech.eu\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 37\r\n\n";
	int i;
	for(i = 0;i != sizeof(intTemp);i++)
	{
	if(intTemp[i] != 0 && intHum[i] != 0 && intPress[i] != 0 && time[i][0-15] != 0)
	{
		sprintf(data, "temperatuur=%d&vochtigheid=%d&luchtdruk=%dtime=%d\r\n", intTemp[i], intHum[i], intPress[i], time[i][0-15]);
		strcpy(response, sendToESP(initCon));
		//Check if there was an error
		if (strstr(response, "ERROR") != NULL)
		{
			intError = 1; //change error code to '1' for ESP related error
			debugPrintln(&huart2, "ERROR1"); // Message for debugging
		}
		strcpy(response, sendToESP(initSize));
		//Check if there was an error
		if (strstr(response, "ERROR") != NULL)
		{
			intError = 1; //change error code to '1' for ESP related error
			debugPrintln(&huart2, "ERROR1"); // Message for debugging
		}
		strcpy(response, sendToESP(data));
		//Check if there was an error
		if (strstr(response, "ERROR") != NULL)
		{
			intError = 1; //change error code to '1' for ESP related error
			debugPrintln(&huart2, "ERROR1"); // Message for debugging
		}
	}
	//osDelay(6000); //Delay for sending #1min
	}
	i = 0;
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_readData */
/**
* @brief Function implementing the readDataTask thread.
* @param argument: Not used
* @retval None
*/

struct bme280_dev dev;
struct bme280_data comp_data;
int8_t rslt;

char line1[16];
char line2[16];
char line3[16];

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if(HAL_I2C_Master_Transmit(&hi2c1, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
  if(HAL_I2C_Master_Receive(&hi2c1, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

  return 0;
}

void user_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);

  if(HAL_I2C_Master_Transmit(&hi2c1, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

  free(buf);
  return 0;
}

/* USER CODE END Header_readData */
void readData(void *argument)
{
  /* USER CODE BEGIN readData */
	debugPrintln(&huart2, "readData FUNC \n"); // Message for debugging
	/* BME280 초기화 */
	  dev.dev_id = BME280_I2C_ADDR_SEC;
	  dev.intf = BME280_I2C_INTF;
	  dev.read = user_i2c_read;
	  dev.write = user_i2c_write;
	  dev.delay_ms = user_delay_ms;

	  rslt = bme280_init(&dev);

	  /* BME280 설정 */
	  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	  dev.settings.filter = BME280_FILTER_COEFF_16;
	  rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);

	  /* USER CODE BEGIN 3 */
	  /* FORCED 모드 설정, 측정 후 SLEEP 모드로 전환�?� */
	  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	  dev.delay_ms(40);
	  /* �?��?�터 취�? */
	  rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
	  if(rslt == BME280_OK)
	  {
		*intTemp = comp_data.intTemp / 100.0;      /* °C  */
		*intHum = comp_data.intHum / 1024.0;           /* %   */
		*intPress = comp_data.intPress / 10000.0;          /* hPa */
	  }

  /* USER CODE END readData */
}

/* USER CODE BEGIN Header_getESPtime */
/**
* @brief Function implementing the getESPtimeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_getESPtime */
void getESPtime(void *argument)
{
  /* USER CODE BEGIN getESPtime */
 debugPrintln(&huart2, "getESPtime FUNC \n"); // Message for debugging
  /* Infinite loop */
	//NTPdateTime

//Local var.
char response[10]; //Containt data send from the ESP over UART1
char setNTP[] = "AT+CIPSNTPCFG=1,2,\"0.nl.pool.ntp.org\",\"1.nl.pool.ntp.org\"\r\n";
char getDT[] = "AT+CIPSNTPTIME?\r\n";
char disableEcho[] = "ATE0\r\n";


//debugPrintln(&huart2, "Echo disabled: "); // Message for debugging
//debugPrintln(&huart2, sendToESP(disableEcho)); // Message for debugging (disable echo from ESP)
//debugPrintln(&huart2, "\n"); // Message for debugging
sendToESP(disableEcho);
//Check if there was an error
strcpy(response, sendToESP(setNTP));
if (strstr(response, "ERROR") != NULL)
{
	intError = 1; //change error code to '1' for ESP related error
	//debugPrintln(&huart2, "ERROR1"); // Message for debugging
}

osDelay(100);
//Check if there was an error
strcpy(response, sendToESP(getDT));
if (strstr(response, "ERROR") != NULL)
{
	intError = 1; //change error code to '1' for ESP related error
	//debugPrintln(&huart2, "ERROR1"); // Message for debugging
}else{
	//Remove date
	//char *tmp = response + 10;
	memmove(response, response+24, strlen(response));
	response[strlen(response)-10] = '\0';
	//debugPrintln(&huart2, "TIJD: \n"); // Message for debugging
	debugPrintln(&huart2, response); // Message for debugging
	strcpy(response,NTPdateTime);
}

 	osDelay(2000); //Delay for sending #1min
  /* USER CODE END getESPtime */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
