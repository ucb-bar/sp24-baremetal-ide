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
#include <stdlib.h>

// Constants
#define MMIO_BASE   0x08808000
#define CACHELINE           64
#define WIDTH               16
#define HEIGHT              16



typedef enum {
    INT8,
    INT16
} DataType;

__attribute__((aligned(CACHELINE))) int16_t outputImage[HEIGHT-2][WIDTH-2];

void printImage(void *image, int height, int width, DataType type) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (type == INT8) {
                // For Printing Input Image 
                printf("\r\n%3d ", ((int8_t*)image)[y * width + x]);
            } else if (type == INT16) {
                // For Printing Convolution Output / Expected Output 
                printf("\r\n%5hd ", ((int16_t*)image)[y * width + x]);
            }
        }
        printf("\n");
    }
}


void populateTestImage(int8_t* testImage, int height, int width) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            
            // Generate Random Value
            int value = (rand() % 256) - 128;
            testImage[index] = (int8_t) value;
        }
    }
}

void convolve(int8_t* input, int inputWidth, int inputHeight,
              int8_t kernel[3][3], int16_t* output) {

    // Iterate through image
    for (int i = 1; i < inputHeight - 1; i++) {
        for (int j = 1; j < inputWidth - 1; j++) {
            int32_t sum = 0;

            // Iterate through kernel
            for (int m = -1; m <= 1; m++) { 
                for (int n = -1; n <= 1; n++) { 
                    int ii = i + m;
                    int jj = j + n;

                    // Perform the multiplication and addition for the convolution
                    sum += input[ii * inputWidth + jj] * kernel[m + 1][n + 1];
                }
            }

            // Saturate
            if (sum > 32767) {
                sum = 32767;
            }
            if (sum < -32768) {
                sum = -32768;
            }

            // Calculate the output position
            int outputIndex = (i - 1) * (inputWidth - 2 * 1) + (j - 1);
            output[outputIndex] = sum;
        }
    }
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
    printf("\r\nhi");
    

// TODO: Define instructions properly based on our new inst
    // Identity Kernel
    __attribute__((aligned(CACHELINE))) int8_t kernel[3][3] = {{0, 0, 0},{0, 1, 0}, {0, 0, 0}};

    __attribute__((aligned(CACHELINE))) int8_t testImage[HEIGHT][WIDTH];
    __attribute__((aligned(CACHELINE))) int16_t expectedOutputImage[HEIGHT-2][WIDTH-2];

    volatile uint64_t* statusPtr = (volatile uint64_t*)(MMIO_BASE + 0x0);
    volatile uint8_t* readyPtr = (volatile uint8_t*) (MMIO_BASE + 0x08);
    volatile uint64_t* srcAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x10);
    volatile uint64_t* destAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x20);
    volatile uint64_t* inputHeightPtr = (volatile uint64_t*)(MMIO_BASE + 0x40);
    volatile uint64_t* inputWidthPtr = (volatile uint64_t*)(MMIO_BASE + 0x60);
    volatile uint64_t* kernelRegPtr = (volatile uint64_t*)(MMIO_BASE + 0x70);

    uint8_t readyValue = 0;
    uint64_t srcAddrValue = (uint64_t) &testImage;       
    uint64_t destAddrValue = (uint64_t) &outputImage;    
    uint16_t inputHeightValue = HEIGHT;            
    uint16_t inputWidthValue = WIDTH;           

    // Create Test Image
    puts("\r\nPopulating test image");
    populateTestImage((int8_t*) &testImage, HEIGHT, WIDTH);

    puts("\r\nTest image");
    //printImage(&testImage, HEIGHT, WIDTH, INT8);

    // Calculate Expected Output
    puts("\r\nCreating expected output image");
    convolve((int8_t*) &testImage, WIDTH, HEIGHT, kernel, (int16_t*) &expectedOutputImage);

    puts("\r\nExpected output image:");
    printImage((int16_t*) &expectedOutputImage, HEIGHT-2, WIDTH-2, INT16);

    // Set MMIO Registers
    puts("\r\nInitializing memory");
    *srcAddrPtr = srcAddrValue;
    *destAddrPtr = destAddrValue;

    memcpy(kernelRegPtr, kernel, sizeof(kernel));

    printf("\r\nReady Bit and Status Bits: %d %lx\n", *readyPtr, *statusPtr);
    printf("\r\nSource Image Address: %lx\n", *srcAddrPtr);
    printf("\r\nDestination Image Address: %lx\n", *destAddrPtr);

    *inputHeightPtr = inputHeightValue; 
    *inputWidthPtr = inputWidthValue;

    printf("\r\nInput Height Value: %ld\n", *inputHeightPtr);
    printf("\r\nInput Width Value: %ld\n", *inputWidthPtr);

    // Start Convolution
    asm volatile("fence");
    uint64_t start_cycles = READ_CSR("mcycle");
    asm volatile("fence");
    *readyPtr  = readyValue;

    // Wait while convolution is busy
    puts("\r\nWaiting on convolution");
    while (*readyPtr == 0);
    puts("\r\nConvolution Complete");
    asm volatile("fence");
    uint64_t end_cycles = READ_CSR("mcycle");
    asm volatile("fence");

    // Check Output against Expected Output
    puts("\r\nChecking against expected output");
    if (memcmp(outputImage, expectedOutputImage, sizeof(outputImage)) == 0) {
        printf("\r\nConvolution output matches expected result.\n");
    } else {
        printf("\r\nTest failed: Mismatch in convolution output.\n");
    }

    // Print outputImage
    printImage(&outputImage, HEIGHT-2, WIDTH-2, INT16);
    
    puts("\r\nTest complete.");
    printf("\r\nNumber of cycles: %llu", end_cycles - start_cycles);
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