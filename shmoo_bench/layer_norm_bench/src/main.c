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

#include "accelerator_funcs.h"
#include "rocc.h"
#include <inttypes.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#define VEC_SIZE 8
#define NUM_TESTS 100


uint64_t read_cycles() {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

int8_t sqrt(int16_t x) {
    if (x <= 1) return x; // Base case: square root of 0 or 1 is the number itself

    int16_t low = 0, high = x;
    int8_t result = 0;

    while (low <= high) {
        int16_t mid = low + (high - low) / 2;
        int32_t square = (int32_t)mid * mid; // Use int32_t to avoid overflow

        if (square == x) {
            return (int8_t)mid;
        } else if (square < x) {
            result = (int8_t)mid; // Keep track of the possible result
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return result;
}
int8_t* layer_norm_cpu(int8_t* vec_in) {
    int16_t sum = 0;

    for (int i = 0; i < VEC_SIZE; i++) {
        sum += vec_in[i];
    }

    int16_t avg = sum / VEC_SIZE;

    int32_t sum_squared_diff = 0;

    for (int i = 0; i < VEC_SIZE; i++) {
        int16_t diff = vec_in[i] - avg;
        sum_squared_diff += diff * diff;
    }

    int16_t var = (int16_t)(sum_squared_diff / VEC_SIZE);

    int8_t std = sqrt(var);

    int8_t* vec_out = malloc(VEC_SIZE * sizeof(int8_t));

    for (int i = 0; i < VEC_SIZE; i++) {
        vec_out[i] = (vec_in[i] - avg) / std;
    }

    return vec_out;
}

int8_t* layer_norm_accel(int8_t* vec_in) {
    asm volatile("fence");
    V_LOAD(1, vec_in);
    asm volatile("fence");

    asm volatile("fence");
    V_LAYERNORM(1);
    asm volatile("fence");

    int8_t* accel_layer_norm = malloc(VEC_SIZE * sizeof(int8_t));
    asm volatile("fence");
    V_STORE(0, accel_layer_norm);
    asm volatile("fence");

    return accel_layer_norm;
}



void print_vec(int8_t* vec) {
    printf("[");
    for (int i = 0; i < VEC_SIZE; i++) {
        printf("%d", vec[i]);
        if (i < VEC_SIZE - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

int vectors_equal(int8_t vector1[], int8_t vector2[]) {
    for (int i = 0; i < VEC_SIZE; i++) {
        if (vector1[i] != vector2[i]) {
            return 0; // Return false if any element is different
        }
    }
    return 1; // Return true if all elements are the same
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
    printf("hi");

// TODO: Define instructions properly based on our new inst
    unsigned long start, end;
    unsigned long cpu_cycles = 0;
    unsigned long accel_cycles = 0;

    int8_t* vec = malloc(VEC_SIZE * sizeof(int8_t));

    for (int i = 0; i < NUM_TESTS; i++) {
        for (int i = 0; i < VEC_SIZE; i++) {
            vec[i] = (int8_t)(rand() % 256 - 128);
        }

        start = read_cycles();
        int8_t* cpu_layer_norm = layer_norm_cpu(vec);
        end = read_cycles();
        cpu_cycles += end - start;

        start = read_cycles();
        int8_t* accel_layer_norm = layer_norm_accel(vec);
        end = read_cycles();
        accel_cycles += end - start;

        if (!vectors_equal(cpu_layer_norm, accel_layer_norm)) {
            printf("INPUT:\t");
            print_vec(vec);
            printf("CPU OUTPUT:\t");
            print_vec(cpu_layer_norm);
            printf("ACCEL OUTPUT:\t");
            print_vec(accel_layer_norm);
        }

        free(cpu_layer_norm);
        free(accel_layer_norm);
    }

    free(vec);

    printf("AVERAGE CPU CYCLES:\t%llu\n", (cpu_cycles / NUM_TESTS));
    printf("AVERAGE ACCEL CYCLES:\t%llu\n", (accel_cycles / NUM_TESTS));

    return 0;
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