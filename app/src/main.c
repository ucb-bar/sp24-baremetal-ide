/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
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
#include "chip_config.h"
#include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */

void app_init() {

  I2C_InitType I2C_init_config;
  I2C_init_config.clock = 40000;

  i2c_init(I2C0, &I2C_init_config);

  sleep(3);
  
}

void app_main() {
  while (1) {
    printf("Hello World\r\n");
    // i2c_master_probe(I2C0, 0x69, 10000);
    i2c_master_probe(I2C0, 0x20, 10000);
    // i2c_master_probe(I2C0, 0x69, 10000);
    // uint8_t buf[20] = {0x00};
    // uint8_t buffer[2] = {0, 0};
    // i2c_master_transmit(I2C0, 0x20, buf,  sizeof(buf), 100);
    // i2c_master_receive(I2C0, 0x20, buffer, 2, 100);
    // i2c_master_probe(I2C0, 0x20, 10000);
    sleep(3);

    

  }
}
/* USER CODE END PUC */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Configure the system clock */
  /* Configure the system clock */

  /* USER CODE BEGIN SysInit */
  /* USER CODE BEGIN SysInit */
  // Initialize UART0 for Serial Monitor
  UART_InitType UART0_init_config;
  UART0_init_config.baudrate = 115200;
  UART0_init_config.mode = UART_MODE_TX_RX;
  UART0_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART0_init_config);

  UART_InitType UART1_init_config;
  UART1_init_config.baudrate = 115200;
  UART1_init_config.mode = UART_MODE_TX_RX;
  UART1_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART1, &UART0_init_config);


  /* Initialize all configured peripherals */
  /* USER CODE BEGIN Init */
  printf("-----Initialize App-----\r\n");
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("-----Start Main-----\r\n");
  while (1) {
    app_main();
  }
  return 0;
  /* USER CODE END WHILE */
}

/*
 * Main function for secondary harts
 *
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  uint64_t mhartid = READ_CSR("mhartid");
  while (1) {
    asm volatile("wfi");
  }
}