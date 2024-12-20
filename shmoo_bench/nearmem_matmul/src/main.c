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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "nearmemdma.h"

#define CACHELINE         64
#define REGION_SIZE_LINES 16 
#define STRIDE (1.5 * CACHELINE)

__attribute__((aligned(CACHELINE))) int8_t mem1[REGION_SIZE_LINES][CACHELINE / sizeof(int8_t)];

int16_t saturate(int32_t x) {
    int16_t max = (1 << 15) - 1;
    int16_t min = -(1 << 15);
    if (x > max) {
        return max;
    } else if (x < min) {
        return min;
    } else {
        return x;
    }
}

void pll_setup() {
  printf("PLL Test\r\n");
  reg_write8(0x130000, 0x0);          // reset clock selector
  PLL->LDO_ENABLE = 0x0;
  PLL->PLLEN = 0x0;
  PLL->MDIV_RATIO = 0x1;
  printf("Set MDIV_RATIO\r\n");
  PLL->RATIO = 0x6;       // 300MHz?
  printf("Set RATIO\r\n");
  PLL->FRACTION = 0x0;
  printf("Set FRACTION\r\n");
  PLL->VCODIV_RATIO = 0x0;
  printf("Set VCODIV_RATIO\r\n");
  PLL->ZDIV0_RATIO = 0x1;
  printf("Set ZDIV0_RATIO\r\n");
  PLL->ZDIV0_RATIO_P5 = 0x0;
  printf("Set ZDIV0_RATIO_P5\r\n");
  PLL->ZDIV1_RATIO = 0x1;
  printf("Set ZDIV1_RATIO\r\n");
  PLL->ZDIV1_RATIO_P5 = 0x0;
  printf("Set ZDIV1_RATIO_P5\r\n");
  sleep(1);
  printf("slept\r\n");
  PLL->LDO_ENABLE = 0x1;
  printf("Set LDO_EN\r\n");
  PLL->POWERGOOD_VNN = 0x1;
  PLL->PLLEN = 0x1;
  printf("Set PLLEN\r\n");
  sleep(1);
  // // printf("PLLEN: %d\r\n", reg_read8(0x10000060));
  // // Write the index of the clock you want to use to the base address
  // // Ref: https://bwrcrepo.eecs.berkeley.edu/ee290c_ee194_intech22/sp24-chips/-/blob/main/generators/chipyard/src/main/scala/BearlyChipTop.scala#L52
  printf("PLL Select\r\n");
  PLL->PLLFWEN_B = 0x1;
  reg_write8(0x130000, 0x1);
}


void app_main() {

    int offset = 15;

    // puts("\r\nInitializing memory");
    for (size_t i = 0; i < REGION_SIZE_LINES; i++) {
        for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
            mem1[i][j] = i + j;
        }
    }

    int8_t operandReg[sizeof(DMA1->operandReg)/sizeof(uint8_t)];
    for (size_t i = 0; i < sizeof(operandReg)/sizeof(uint8_t); i++) {
        operandReg[i] = (i + 3);
    }

    _Static_assert(sizeof(operandReg) == 64, "opreg size");

    // _Static_assert(STRIDE % CACHELINE == 0, "stride not aligned");
    uint32_t count = REGION_SIZE_LINES * CACHELINE / STRIDE;
    int16_t expected[sizeof(DMA1->destReg)/sizeof(int16_t)];
    for (size_t i = 0; i < count; i++) {
        int sum = 0;
        for (size_t j = 0; j < sizeof(operandReg)/sizeof(uint8_t) - offset; j++) {
            sum += (int16_t)operandReg[j] * (int16_t)mem1[(int) (i*STRIDE / CACHELINE)][(int) (i*STRIDE) % CACHELINE + j + offset];
        }
        for (size_t j = sizeof(operandReg)/sizeof(uint8_t) - offset; j < sizeof(operandReg)/sizeof(uint8_t); j++) {
            sum += (int16_t)operandReg[j] * (int16_t)mem1[(int) (i*STRIDE / CACHELINE) + 1][(int) (i*STRIDE) % CACHELINE + j - (sizeof(operandReg)/sizeof(uint8_t) - offset)];
        }
        expected[i] = saturate(sum);
    }
 
    void* src_addr = &mem1[0][offset];
    // printf("\r\nsrc_addr: %p\n", src_addr);
    uint64_t stride = STRIDE;
    // printf("\r\nstride: %ld\n", stride);
    // printf("\r\noffset: %d\n", offset);

    // puts("\r\nWaiting for DMA");

    while (DMA1->status.inProgress); // wait for ready

    // puts("\r\nPerforming DMA");

    DMA1->srcAddr = src_addr;
    DMA1->srcStride = stride;
    DMA1->mode = MODE_MAC;
    memcpy(DMA1->operandReg, operandReg, sizeof(operandReg));
    uint64_t cpu_start_cycles = READ_CSR("mcycle");
    __asm__ ("" ::: "memory");

    // wait for peripheral to complete
    DMA1->count = count;
    while (!DMA1->status.completed) {
        if (DMA1->status.error) {
            puts("\r\nError");
            return 1;
        }
    }
    uint64_t cpu_end_cycles = READ_CSR("mcycle");
    for (size_t i = 0; i < count; i++) {
        // if (expected[i] != ((volatile int16_t *)DMA1->destReg)[i]) {
        //     printf("\r\nExpected %d at index %ld, got %d\n", expected[i], i, ((volatile int16_t *)DMA1->destReg)[i]);
        // }
        printf("%d ", ((volatile int16_t *)DMA1->destReg)[i]);
    }
    printf("\r\n");
    // printf("\r\nMemory contents:\n");

    // for (size_t i = 0; i < REGION_SIZE_LINES; i++) {
    //     for (size_t j = 0; j < CACHELINE/sizeof(int8_t); j++) {
    //         printf("\r\n\t%4" PRIx16, mem1[i][j]);
    //     }
    //     printf("\r\n\n");
    // }

    // printf("\r\nExpected output:\n");

    // for (size_t i = 0; i < 8; i++) {
    //     printf("\r\n\t%4" PRIx16 "\n", expected[i]);
    // }
    
    // printf("\r\nDumping...\n");

    // for (size_t i = 0; i < 8; i++) {
    //     printf("\r\n\t%016" PRIx64 "\n", DMA1->destReg[i]);
    // }
    

    // puts("\r\nTest complete");
    // printf("\r\nmcycle: %llu", cpu_end_cycles - cpu_start_cycles);


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

//   pll_setup();
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