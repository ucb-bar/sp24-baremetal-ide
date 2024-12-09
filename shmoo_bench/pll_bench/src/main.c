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
#include <unistd.h>

void app_main() {


    // uint8_t mdiv_ratio = reg_read8(0x10000074);
    // reg_write8(0x10000074, 0x1);        // mdiv_ratio: lower two bits set to 1

    // uint16_t ratio_integer = reg_read16(0x1000006C);
    // reg_write16(0x1000006C, 0x000);          // ratio_integer: lower ten bits set to 0

    // // For 0.5, set ratio_fractional to 8388608 = 0x800000
    // uint32_t ratio_fractional = reg_read32(0x10000070);
    // reg_write32(0x10000070, 0x000000);   // ratio_fractional: lower twenty-four bits set to 0

    // uint8_t post_vco_div = reg_read8(0x10000088);
    // reg_write8(0x10000088, 0x0);              // post_vco_div: lower two bits set to 0

    // uint16_t zdiv0_ratio = reg_read16(0x10000078);
    // reg_write16(0x10000078, 0x001);    // zdiv0_ratio: lower ten bits set to 1

    // uint8_t zdiv0_ratio_p5 = reg_read8(0x1000007C);
    // reg_write8(0x1000007C, 0x0)             // zdiv0_ratio_p5: lower bit set to 0

    // uint16_t zdiv1_ratio = reg_read16(0x10000080);
    // reg_write16(0x10000080, 0x001);    // zdiv0_ratio: lower ten bits set to 1

    // uint8_t zdiv1_ratio_p5 = reg_read8(0x10000084);
    // reg_write8(0x10000084, 0x000);            // zdiv0_ratio_p5: lower bit set to 0

    // uint8_t pllen = reg_read8(0x10000060);
    // reg_write8(0x10000060, 0x1);                // pllen: set 1 to enable

    // Make sure we select PLL in clock selector

    // Read PLL Lock pad
    

    // Write the index of the clock you want to use to the base address
    // Ref: https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/sp24-chips/-/blob/main/generators/chipyard/src/main/scala/BearlyChipTop.scala#L52
    // reg_write8(0x130000, 0x1);

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
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  // app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("PLL Test\r\n");
  PLL->MDIV_RATIO = 0x1;
  PLL->RATIO = 0xF;           // 750MHz?
  PLL->FRACTION = 0x0;
  PLL->VCODIV_RATIO = 0x0;
  PLL->ZDIV0_RATIO = 0x1;
  PLL->ZDIV0_RATIO_P5 = 0x0;
  PLL->ZDIV1_RATIO = 0x1;
  PLL->ZDIV1_RATIO_P5 = 0x0;
  PLL->PLLEN = 0x1;
  time.sleep(7);
  // printf("PLLEN: %d\r\n", reg_read8(0x10000060));
  // Write the index of the clock you want to use to the base address
  // Ref: https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/sp24-chips/-/blob/main/generators/chipyard/src/main/scala/BearlyChipTop.scala#L52
  printf("PLL Select\r\n");
  reg_write8(0x130000, 0x1);
  while (1) {
    // app_main();
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