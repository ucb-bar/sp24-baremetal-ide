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


#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN PUC */


// void app_init() {
//   // torch::executor::runtime_init();
// }



void app_main() {

// TODO: Define instructions properly based on our new inst

    uint8_t two = 1+1;
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
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);

  printf("hello");
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  // app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

    // adjust PLL? verify that PLL is active too
    // clk_in_ref = 100MHz
    // uint8_t mdiv_ratio = reg_read8(0x10000074);
    // reg_write8(0x10000074, (mdiv_ratio & 0xFC) | 0x1);        // mdiv_ratio: lower two bits set to 1

    // uint16_t ratio_integer = reg_read16(0x1000006C);
    // reg_write16(0x1000006C, ratio_integer & 0xFC00);          // ratio_integer: lower ten bits set to 0

    // // For 0.5, set ratio_fractional to 8388608 = 0x800000
    // uint32_t ratio_fractional = reg_read32(0x10000070);
    // reg_write32(0x10000070, ratio_fractional & 0xFF000000);   // ratio_fractional: lower twenty-four bits set to 0

    // uint8_t post_vco_div = reg_read8(0x10000088);
    // reg_write8(0x10000088, post_vco_div & 0xFC);              // post_vco_div: lower two bits set to 0

    // uint16_t zdiv0_ratio = reg_read16(0x10000078);
    // reg_write16(0x10000078, (zdiv0_ratio & 0xFC00) | 0x1);    // zdiv0_ratio: lower ten bits set to 1

    // uint8_t zdiv0_ratio_p5 = reg_read8(0x1000007C);
    // reg_write8(0x1000007C, zdiv0_ratio_p5 & 0xFE);             // zdiv0_ratio_p5: lower bit set to 0

    // uint16_t zdiv1_ratio = reg_read16(0x10000080);
    // reg_write16(0x10000078, (zdiv0_ratio & 0xFC00) | 0x1);    // zdiv0_ratio: lower ten bits set to 1

    // uint8_t zdiv1_ratio_p5 = reg_read8(0x10000084);
    // reg_write8(0x1000007C, zdiv0_ratio_p5 & 0xFE);            // zdiv0_ratio_p5: lower bit set to 0

    app_main();
    return 0;
  }
  /* USER CODE END WHILE */
}

/*
 * Main function for secondary harts
 * 
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  while (1) {
   asm volatile ("wfi");
  }
}