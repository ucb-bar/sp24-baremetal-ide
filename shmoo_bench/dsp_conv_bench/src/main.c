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

#include "float16.h"
#include "float16.c"
// #include "../../../tests/mmio.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define BASE_ADDR 0x08800000

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C

void app_main() {

    puts("\r\nStarting test");
    int8_t in_arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint32_t in_len[1] = {16};
    uint16_t in_dilation[1] = {1};
    uint16_t in_kernel[8] = {0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}; // {2, 0, 0, 0, 0, 0, 0, 0} in FP16                                    

    puts("\r\nSetting values of MMIO registers");
    asm volatile ("fence");
    reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr)));
    asm volatile ("fence");
    reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + 8)));
    asm volatile ("fence");

    reg_write32(LENGTH_ADDR, 16);
    reg_write16(DILATION_ADDR, in_dilation[0]);

    asm volatile ("fence");
    reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel));         // 64 bits: 4 FP16s
    asm volatile ("fence");
    reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel + 4)));   // 64 bits: 4 FP16s (Total 8)
    asm volatile ("fence");

    uint64_t cpu_start_cycles = READ_CSR("mcycle");
    puts("\r\nStarting Convolution");
    asm volatile ("fence");
    reg_write8(START_ADDR, 1);

    puts("\r\nWaiting for convolution to complete");
    
    printf("\r\nInput (INT8): ");
    for (int i = 0; i < 16; i++) {
        printf("%d ", in_arr[i]);
    }

    asm volatile ("fence");

    uint64_t cpu_end_cycles = READ_CSR("mcycle");

    uint16_t test_out[32];
    printf("\r\nTest Output (FP16 binary): ");
    for (int i = 0; i < 4; i++) {
        uint64_t current_out = reg_read64(OUTPUT_ADDR);
        uint16_t* unpacked_out = (uint16_t*) &current_out;
        for (int j = 0; j < 4; j++) {
            test_out[i*4 + j] = unpacked_out[j];
            printf("0x%"PRIx64" ", test_out[i*4 + j]);
        }
    }

    uint16_t ref_out[32];
    for (int i = 0; i < 16; i++) {
        ref_out[i] = f16_from_int((int32_t) (in_arr[i] * 2));
    }
    printf("\r\nReference Output (FP16 binary): ");
    for (int i = 0; i < 16; i++) {
        printf("%#x ", ref_out[i]);
    }
    printf("\r\n");

    if (memcmp(test_out, ref_out, 16) == 0) {
        printf("\r\n[TEST PASSED]: Test Output matches Reference Output.");
        printf("\r\nCycles elapsed: %llu", cpu_end_cycles - cpu_start_cycles);
    } else {
        printf("\r\n[TEST FAILED]: Test Output does not match Reference Output.");
        printf("\r\nCycles elapsed: %llu", cpu_end_cycles - cpu_start_cycles);
    }
    printf("\r\n\r\n");

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
  while (1) {
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