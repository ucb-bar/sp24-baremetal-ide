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

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif


#define STR1(x) #x
#ifndef STR
#define STR(x) STR1(x)
#endif

#define CAT_(A, B) A##B
#define CAT(A, B) CAT_(A, B)

// TODO: Define instructions properly based on our new inst

#define ROCC_INSTRUCTION_R_R_R(x, rd, rs1, rs2, func7)                               \
  {                                                                                  \
    asm volatile(                                                                    \
        ".insn r " STR(CAT(CUSTOM_, x)) ", " STR(0x7) ", " STR(func7) ", %0, %1, %2" \
        : "=r"(rd)                                                                   \
        : "r"(rs1), "r"(rs2));                                                       \
  }

#define ROCC_INSTRUCTION_0_R_R(x, rs1, rs2, func7)                                   \
  {                                                                                  \
    asm volatile(                                                                    \
        ".insn r " STR(CAT(CUSTOM_, x)) ", " STR(0x3) ", " STR(func7) ", x0, %0, %1" \
        :                                                                            \
        : "r"(rs1), "r"(rs2));                                                       \
  }

#define CACHELINE 64

void MAC_model(int8_t A[16][16], int8_t B[16][16], int8_t C[16][16]) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            C[i][j] = 0;

            for (int k = 0; k < 16; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void spMac(uint8_t* A_ptr, uint8_t A_stride, uint8_t* B_ptr, uint8_t B_stride, uint8_t* C_ptr, uint8_t C_stride, uint8_t* D_ptr, uint8_t D_stride, uint8_t* CSR_ptr) {
    ROCC_INSTRUCTION_0_R_R(0, A_ptr, A_stride , 0);
    ROCC_INSTRUCTION_0_R_R(0, B_ptr, B_stride , 1);
    ROCC_INSTRUCTION_0_R_R(0, C_ptr, C_stride , 2);
    ROCC_INSTRUCTION_0_R_R(0, D_ptr, D_stride , 3);
    ROCC_INSTRUCTION_0_R_R(0, CSR_ptr, 0, 4);
    asm volatile ("fence");
}

// Equal probability of generating 0 or 1
int spar50() {
    return rand() & 1;
}

int spar25() {
    return spar50() && spar50();
}

int spar12() {
    return spar50() && spar50() && spar50();
}

int spar3() {
    return spar12() && spar25();
}

int spar08() {
    return spar3() && spar25();
}

static uint64_t read_cycles() {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

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

/* This test is a sanity test and simply tests multiplication of 2 values in the whole matrix*/

    __attribute__((aligned(CACHELINE))) uint8_t A[16][16];
    uint8_t *a_ptr = A[0]; 

    __attribute__((aligned(CACHELINE))) uint8_t B[16][16];
    uint8_t *b_ptr = B[0]; 

    __attribute__((aligned(CACHELINE))) uint8_t C[16][16];
    uint8_t *c_ptr = C[0];

    __attribute__((aligned(CACHELINE))) uint8_t CSR[384] = {0};
    uint8_t *csr_ptr = CSR;
 
    __attribute__((aligned(CACHELINE))) uint8_t correct[16][16] = {0};
    uint8_t *correct_ptr = correct[0];

    // __attribute__((aligned(CACHELINE))) uint8_t D[16][16];
    // uint8_t *d_ptr = D[0];

    // printf("A: %p, B: %p, C: %p, CSR: %p, Correct: %p, D: %p \n", a_ptr, b_ptr, c_ptr, csr_ptr, correct_ptr, d_ptr);

    static int iterations = 5;
    srand(time(NULL));

    for (int num=0; num < iterations; num++) {

        // Generate matrices
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                A[i][j] = (uint8_t)((rand()) % 255);
                B[i][j] = (uint8_t)((rand()) % 255);
                }
            }

        //RoCC
        uint64_t accel_start_cycles = read_cycles();
        spMac(a_ptr, 16, b_ptr, 16, c_ptr, 16, NULL, 16, csr_ptr);
        uint64_t accel_end_cycles = read_cycles();


        //CPU
        uint64_t cpu_start_cycles = read_cycles();
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                correct[i][j] = 0;
                for (int k = 0; k < 16; k++) {
                    correct[i][j] += (uint8_t)((uint8_t) A[i][k] * (uint8_t) B[k][j]);
                }
            }
        }
        uint64_t cpu_end_cycles = read_cycles();

        // Checking Correctness
        bool failed = false;

        for (int i=0; i<16; i++) {
            for (int j=0; j<16; j++) {
                if (((uint8_t)C[i][j]) != ((uint8_t)(correct[i][j]))) {
                    failed = true;
                    // printf("Failed @(%d,%d), Expected:%3d, Got:%3d\n",i,j,correct[i][j],C[i][j]);
                }
            }
        }

        if (!failed) {
            printf("================ Passed %d/%d================\n",num+1,iterations);
            printf("ACCEL perf: %" PRIu64 "\n", accel_end_cycles-accel_start_cycles);
            printf("CPU perf: %" PRIu64 "\n", cpu_end_cycles-cpu_start_cycles);

        }else{
            printf("================ Failed @ %d (%d/%d)================\n",num%10, num+1,iterations);
            printf("A Matrix\n");
            for (int i=0; i<16; i++) {
                for (int j=0; j<16; j++) {
                    printf("%6u", (A[i][j]));
            }
            printf("\n");
            }
            
            printf("B Matrix\n");
            for (int i=0; i<16; i++) {
                for (int j=0; j<16; j++) {
                    printf("%6u", (B[i][j]));
            }
            printf("\n");
            }
            printf("correct:\n");
            for(int i=0; i<16; i++){
                for (int j=0; j<16; j++) {
                    printf("%6u", (correct[i][j]));
            }
            printf("\n");
            }

            printf("Output Matrix\n");
            for (int i=0; i<16; i++) {
                for (int j=0; j<16; j++) {
                    printf("%6u", (C[i][j]));
            }
            printf("\n");
            }
        }
    }

}

/* USER CODE END PUC */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  printf("hello");
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