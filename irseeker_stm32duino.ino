/**
*  Used Libraries
*  Arduino_Core_STM32 - https://github.com/stm32duino/Arduino_Core_STM32
*  elapsedMillis - https://github.com/pfeerick/elapsedMillis - 
*
*
*/



#include <elapsedMillis.h>
elapsedMillis milis;


/* Copy of STM32 functions from STM32CubeIDE  */
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C2_Init(void);




/* Setup for use of Serial with PA3 and PA2, to be sure,
* but not 5V tolerant, but for sending info doesn't matter, in case of need 5V tolerant, use serial connection over Serial3
*/
  #ifndef SERIAL_UART_INSTANCE 
    #define SERIAL_UART_INSTANCE  2 
  #endif 
    
  // Default pin used for generic 'Serial' instance 
  // Mandatory for Firmata 
  #ifndef PIN_SERIAL_RX 
    #define PIN_SERIAL_RX         PA3 
  #endif 
  #ifndef PIN_SERIAL_TX 
    #define PIN_SERIAL_TX         PA2 
  #endif 



HardwareSerial Serial3(PC11, PC10); // Serial3 with pins PC11 and PC10 is 5V tolerant


uint32_t num_cycles = 0;

uint32_t inputs[12];
int current_max = 0;


uint32_t masks[] = {
		  0,  // 0
		  1,  // 1
		  2,  // 2
		  3,  // 3                                             
		  4,  // 4
		  5,  // 5
		  6,  // 6
		  7,  // 7
		  9,  // 8
		  10, // 9
		  11, //10
		  12, //11
};
void reset_inputs(){
	for(int i=0;i<12;i++){
		inputs[i] = 0;
	}
  num_cycles = 0;
}
void read_inputs(){

	milis = 0;

  
	while(milis <2){
		uint32_t value =  (GPIOB->IDR & 0b0001111011111111);
    num_cycles++;
		for(int i=0;i<12;i++){
      
			inputs[i] += 1 - ((value & (1 << masks[i])) >> masks[i]);
		}
	}

}

unsigned int find_max(){


  current_max = 0;
	unsigned int c = 0;
	
  inputs[0] = inputs[0] * 1.10; // prefer front sensor
	
  for(int i=0;i<12;i++){
    if(inputs[i] == num_cycles){
      continue;
    }
		if(inputs[i] > current_max){
			c = i+1;
			current_max = inputs[i];

		}
	}
  //Serial3.println();

	return c;
}

void setup() {
  
  /* base setup input and output pins
     PC6 AND PC13 
  */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  //MX_I2C2_Init();

  

  Serial.begin(115200);
  Serial3.begin(115200);

  //led blinking for if chip is working on start
  for(int x=0;x<10;x++){
    digitalWrite(PC13, HIGH);
    delay(50);
    digitalWrite(PC13, LOW);
    delay(50);
  }
  digitalWrite(PC13, HIGH);
}

void loop() {
  

  reset_inputs();
	read_inputs();

  int position = find_max();

  Serial3.println(position,HEX);
  delay(100);
  
}

/**
*  Functions copied from STM32CubeIde
**/


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB3 PB4
                           PB5 PB6 PB7 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}



I2C_HandleTypeDef hi2c2;


UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;



/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{


  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10B0DCFB;
  hi2c2.Init.OwnAddress1 = 16;
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


}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}
