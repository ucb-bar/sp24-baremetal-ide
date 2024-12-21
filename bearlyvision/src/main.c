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
#include "bearlyconv.h"
#include "image_data.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Constants
// #define MMIO_BASE   0x08808000
#define CACHELINE           64
// #define WIDTH               64
// #define HEIGHT              256
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


void app_init() {
  UART_InitType UART_init_config;
  UART_init_config.baudrate = 115200;
  UART_init_config.mode = UART_MODE_TX_RX;
  UART_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART_init_config);
}


static unsigned long read_cycles() {
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

void benchmark_convolution(int8_t* input, int inputWidth, int inputHeight,
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

void app_main() {
  puts("Test Start");
  
  __attribute__((aligned(CACHELINE))) int8_t kernel[3][3] = {{-1, -1, -1},{-1, 8, -1}, {-1, -1, -1}};
  __attribute__((aligned(CACHELINE))) int16_t outputImage[HEIGHT-2][WIDTH-2];
  //__attribute__((aligned(CACHELINE))) int8_t testImage[HEIGHT][WIDTH];
  __attribute__((aligned(CACHELINE))) int16_t expectedOutputImage[HEIGHT-2][WIDTH-2];
    
    // Create Test Image
    //puts("Populating test image");
    //populateTestImage((int8_t*) &testImage, HEIGHT, WIDTH);

    // Print Test Image
    //puts("Test image");
    //printImage(&testImage, HEIGHT, WIDTH, INT8);

    // Calculate Expected Output
    puts("benchmark conv");
    benchmark_convolution((int8_t*) &testImage, WIDTH, HEIGHT, kernel, (int16_t*) &outputImage);

    puts("Running conv accelrator");
    convolution((int8_t*) &testImage, (int16_t*) &expectedOutputImage, WIDTH, HEIGHT, (int8_t*) &kernel);

    puts("Checking against expected output");
    if (memcmp(outputImage, expectedOutputImage, sizeof(outputImage)) == 0) {
        printf("Convolution output matches expected result.\n");
    } else {
        printf("Test failed: Mismatch in convolution output.\n");
        for (int i = 0; i < HEIGHT-2; i ++) {
            for (int j = 0; j < WIDTH-2; j ++) {
                //if (outputImage[i][j] != expectedOutputImage[i][j]) {
                    printf (" Index %d, %d. Normal = %d, Chucked = %d \n", i, j, outputImage[i][j], expectedOutputImage[i][j]);
                //}
            }
        }
        
    }

    // Print outputImage
    //printImage(&outputImage, HEIGHT-2, WIDTH-2);

    //writeOutputImageToFile("output_image.txt", outputImage);
    
    puts("Test complete.");

    return 0;
    
}

/* USER CODE END PUC */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Configure the system clock */
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */  
  /* USER CODE BEGIN Init */
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  app_main();

  while (1) {
    // return 0;
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
