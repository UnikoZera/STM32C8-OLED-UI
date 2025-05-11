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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "oled_ui.h"
#include "oled_driver.h"
// #include "oled_benchmark.h"
#include "oled_optimize.h"
#include "pid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t count = 0; // 旋钮数据
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init(); // 初始化OLED
  OLED_InitBuffer(); // 初始化双缓冲
  OLED_ClearBuffer(); // 清空缓冲区
  
  OLED_EnableDiffMode(1);  // 启用差分更新
  OLED_EnableFastUpdate(1); // 启用快速更新

  
  // HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // 启动PWM
  // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 启动PWM
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 启动PWM
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); // 启动编码器
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2); // 启动编码器

  OLED_InitAnimationManager(&g_AnimationManager); // 初始化动画管理器
  OLED_InitAnimationManager(&Menu_AnimationManager); // 初始化菜单动画管理器
  OLED_InitAnimationManager(&Games_AnimationManager); // 初始化游戏动画管理器
  OLED_InitAnimationManager(&Tools_AnimationManager); // 初始化工具动画管理器
  OLED_InitAnimationManager(&Settings_AnimationManager); // 初始化设置动画管理器
  OLED_InitAnimationManager(&About_AnimationManager); // 初始化关于动画管理器
  OLED_InitAnimationManager(&Status_AnimationManager); // 初始化状态动画管理器
  OLED_InitAnimationManager(&g_Title_AnimationManager); // 初始化标题动画管理器
  __HAL_TIM_SET_COUNTER(&htim3, 32767);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {    
    #pragma region OLED_UI_SETTINGS // UI设置
    OLED_ClearBuffer();
    
    System_UI_Loop(); // UI循环
    
    
    
    OLED_OptimizedDisplayFPS(80, 56); // 显示帧率
    count = __HAL_TIM_GET_COUNTER(&htim3); // 获取编码器计数值 1圈40
    OLED_UpdateAnimationManager(&Menu_AnimationManager); // 更新菜单动画管理器
    OLED_UpdateAnimationManager(&g_AnimationManager); // 更新动画管理器
    OLED_UpdateAnimationManager(&Games_AnimationManager); // 更新游戏动画管理器
    OLED_UpdateAnimationManager(&Tools_AnimationManager); // 更新工具动画管理器
    OLED_UpdateAnimationManager(&Settings_AnimationManager); // 更新设置动画管理器
    OLED_UpdateAnimationManager(&About_AnimationManager); // 更新关于动画管理器
    OLED_UpdateAnimationManager(&Status_AnimationManager); // 更新状态动画管理器
    OLED_UpdateAnimationManager(&g_Title_AnimationManager); // 更新标题动画管理器
    OLED_SmartUpdate(); // 智能更新显示

    #pragma endregion OLED_UI_SETTINGS


    //  // HAL_Delay(1000); // 延时1秒
    //  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 点亮LED
    //  // HAL_Delay(1000); // 延时1秒
    //  // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // 熄灭LED
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
