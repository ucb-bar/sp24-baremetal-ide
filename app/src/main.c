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
#include "hal_conv.h"
#include <math.h>
#include "test_image.h"
// #include "../../../tests/mmio.h"
// #include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.14159265359
#define HEIGHT 24
#define WIDTH 32
#define MAX_KERNEL_SIZE 31  // This will support sigma up to 5.0

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C
#define INPUT_TYPE_ADDR 0x0880008E
#define RESET_ADDR      0x0880008E

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
  // torch::executor::runtime_init();
}

int my_strcmp(const char *str1, const char *str2) {
    // Loop through each character of both strings
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            // Return the difference if characters don't match
            return (unsigned char)*str1 - (unsigned char)*str2;
        }
        str1++;
        str2++;
    }

    // If we exit the loop, check the remaining characters
    return (unsigned char)*str1 - (unsigned char)*str2;
}

void read_pixels(uint8_t* image_data) { 
    // Pixel data transfer
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        uart_receive(UART0, &image_data[i], sizeof(char), 100000);
    }

}
void capture_image(uint8_t* image_data) {

    printf("r\n");
    printf("x\n");

    int index = 0;
    int start_found = 0; // false
    char received; 
    char buffer[60];

    while (1) {
        uart_receive(UART0, &received, sizeof(char), 100000);
        if (received == '\n' || index >= sizeof(buffer) - 1) {
            buffer[index] = '\0';
            index = 0;

            if (my_strcmp(buffer, "START\r") == 0) {
                read_pixels(image_data);
            }
            
            if (my_strcmp(buffer, "END\r") == 0) {
                break;
            }
        } else {
            buffer[index++] = received;
        }
    }
}

// Double threshold implementation
void doubleThreshold_opt(uint8_t* src, uint8_t* dst, int size, float highThreshold, float lowThreshold) {
    const uint8_t high = highThreshold * 255;
    const uint8_t low = lowThreshold * 255;
    register uint8_t strong_edge = 255;
    register uint8_t weak_edge = 128;
    
    uint8_t* const src_end = src + size;
    
    while (src + 16 < src_end) {
        asm volatile(
            "vsetvli zero, %0, e8, m8, ta, ma\n\t"
            "vle8.v v8, (%1)\n\t"
            // Compare with high threshold into v0
            "vmsgeu.vx v0, v8, %2\n\t"         
            // Initialize and set strong edges
            "vmv.v.x v16, x0\n\t"              
            "vmerge.vxm v16, v16, %3, v0\n\t"  
            // Compare with low threshold
            "vmsgeu.vx v1, v8, %4\n\t"         
            // Save strong edge mask
            "vmv.v.v v2, v0\n\t"               
            // Set v0 = low AND NOT high directly
            "vmandn.mm v0, v1, v2\n\t"         
            // Set weak edges
            "vmerge.vxm v16, v16, %5, v0\n\t"  
            // Store results
            "vse8.v v16, (%6)\n\t"
            "vle8.v v8, (%7)"                  
            :
            : "r"(16), "r"(src), 
              "r"(high), "r"(strong_edge), 
              "r"(low), "r"(weak_edge), 
              "r"(dst), "r"(src + 16)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );

        src += 16;
        dst += 16;
    }

    // Handle remaining elements
    if (src < src_end) {
        size_t vl;
        asm volatile(
            "vsetvli %0, %1, e8, m8, ta, ma\n\t"
            "vle8.v v8, (%2)\n\t"
            // Compare with high threshold into v0
            "vmsgeu.vx v0, v8, %3\n\t"
            // Initialize and set strong edges
            "vmv.v.x v16, x0\n\t"
            "vmerge.vxm v16, v16, %4, v0\n\t"
            // Compare with low threshold
            "vmsgeu.vx v1, v8, %5\n\t"
            // Save strong edge mask
            "vmv.v.v v2, v0\n\t"
            // Set v0 = low AND NOT high directly
            "vmandn.mm v0, v1, v2\n\t"
            // Set weak edges
            "vmerge.vxm v16, v16, %6, v0\n\t"
            // Store results
            "vse8.v v16, (%7)"
            : "=r"(vl)
            : "r"((size_t)(src_end - src)), "r"(src),
              "r"(high), "r"(strong_edge),
              "r"(low), "r"(weak_edge), "r"(dst)
            : "v0", "v1", "v2", "v8", "v16", "memory"
        );
    }
}

// Non-maximum suppression implementation
void nonMaxSuppression(uint8_t* gradientMagnitude, uint8_t* gradientDirection, uint8_t* nms) {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int idx = y * WIDTH + x;
            float angle = gradientDirection[idx];
            float mag = gradientMagnitude[idx];
            
            if (angle < 0) angle += 180;
            
            float mag1, mag2;
            
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                mag1 = gradientMagnitude[idx-1];
                mag2 = gradientMagnitude[idx+1];
            }
            else if (angle >= 22.5 && angle < 67.5) {
                mag1 = gradientMagnitude[(y-1)*WIDTH + (x+1)];
                mag2 = gradientMagnitude[(y+1)*WIDTH + (x-1)];
            }
            else if (angle >= 67.5 && angle < 112.5) {
                mag1 = gradientMagnitude[(y-1)*WIDTH + x];
                mag2 = gradientMagnitude[(y+1)*WIDTH + x];
            }
            else {
                mag1 = gradientMagnitude[(y-1)*WIDTH + (x-1)];
                mag2 = gradientMagnitude[(y+1)*WIDTH + (x+1)];
            }
            
            nms[idx] = (mag >= mag1 && mag >= mag2) ? mag : 0;
        }
    }
}

// 1D conv
void convolution_1D(uint8_t *arr, size_t arr_len, float *kernel, size_t kernel_len, size_t dilation, uint8_t *output) {
    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        float sum = 0;
            for (int kx = -1; kx <= 1; kx++) {
                sum += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = (uint8_t)sum;
    }
}

// Gaussian Blur implementation
void gaussian_blur(uint8_t* src, uint8_t* dst) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    uint8_t test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    uint8_t test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    uint8_t test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        float in_kernel1[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel1, 8, in_dilation[0], test_out1 + y*WIDTH);
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        float in_kernel2[8] = {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel2, 8, in_dilation[0], test_out2 + y*WIDTH);
    }

    for (int y = 2; y < HEIGHT; y++) {
        float in_kernel3[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
        convolution_1D(src + y*WIDTH, in_len[0], in_kernel3, 8, in_dilation[0], test_out3 + y*WIDTH);
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 0; x < WIDTH; x++) {
            dst[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

// Sobel operator implementation
void sobelOperator(uint8_t* src, uint8_t* gradientMagnitude, uint8_t* gradientDirection) {
    const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            float gx = 0, gy = 0;
            
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int idx = (y + i) * WIDTH + (x + j);
                    gx += src[idx] * Gx[i+1][j+1];
                    gy += src[idx] * Gy[i+1][j+1];
                }
            }

            int idx = y * WIDTH + x;
            gradientMagnitude[idx] = (uint8_t)sqrt(gx*gx + gy*gy);
            gradientDirection[idx] = (uint8_t)(atan2(gy, gx) * 180.0 / PI);
        }
    }
}

// Hysteresis implementation
void hysteresis(uint8_t* src, uint8_t* dst) {
    memcpy(dst, src, WIDTH * HEIGHT);
    int changed;
    
    do {
        changed = 0;
        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                int idx = y * WIDTH + x;
                if (dst[idx] == 128) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            int neighborIdx = (y + dy) * WIDTH + (x + dx);
                            if (dst[neighborIdx] == 255) {
                                dst[idx] = 255;
                                changed = 1;
                                break;
                            }
                        }
                        if (dst[idx] == 255) break;
                    }
                    if (dst[idx] == 128) {
                        dst[idx] = 0;
                    }
                }
            }
        }
    } while (changed);
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
  app_init();
  /* USER CODE END Init */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // while (1) {
  //   app_main();
  //   return 0;
  // }


  uint8_t image_data[WIDTH * HEIGHT]; // Stack array for image data

  capture_image(image_data);
  
  uint8_t blurred[WIDTH * HEIGHT] = {0};
  uint8_t gradientMagnitude[WIDTH * HEIGHT] = {0};
  uint8_t gradientDirection[WIDTH * HEIGHT] = {0};
  uint8_t nms[WIDTH * HEIGHT] = {0};
  uint8_t threshold[WIDTH * HEIGHT] = {0};
  uint8_t output[WIDTH * HEIGHT] = {0};

  gaussian_blur(image_data, blurred);
  sobelOperator(blurred, gradientMagnitude, gradientDirection);
  nonMaxSuppression(gradientMagnitude, gradientDirection, nms);
  doubleThreshold_opt(nms, threshold, WIDTH * HEIGHT, 0.2, 0.1);  // High threshold = 0.2, Low threshold = 0.1
  hysteresis(threshold, output);

  for (size_t x = 0; x < sizeof(output) / sizeof(output[0]); x++) {
    printf("%c", (unsigned char)output[x]); // Send exactly 1 byte
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
  while (1) {
   asm volatile ("wfi");
  }
}