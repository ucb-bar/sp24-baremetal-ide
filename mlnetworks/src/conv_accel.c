#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//MMIO Base Address
#define MMIO_BASE   0x08808000
#define MAX_TEMP_BUFFER_SIZE 1024


// Implementation of the conv_accelarator function
void hardware_convolution2d(int8_t *source, int16_t *dest, int width, int height, int8_t *kernel) {
    
    volatile uint64_t* statusPtr = (volatile uint64_t*)(MMIO_BASE + 0x0);
    volatile uint8_t* readyPtr = (volatile uint8_t*) (MMIO_BASE + 0x08);
    volatile uint64_t* srcAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x10);
    volatile uint64_t* destAddrPtr = (volatile uint64_t*)(MMIO_BASE + 0x20);
    volatile uint64_t* inputHeightPtr = (volatile uint64_t*)(MMIO_BASE + 0x40);
    volatile uint64_t* inputWidthPtr = (volatile uint64_t*)(MMIO_BASE + 0x60);
    volatile uint64_t* kernelRegPtr = (volatile uint64_t*)(MMIO_BASE + 0x70);

    uint8_t readyValue = 0;
    uint64_t srcAddrValue = (uint64_t) source;       
    uint64_t destAddrValue = (uint64_t) dest;    
    uint16_t inputHeightValue = height;            
    uint16_t inputWidthValue = width;           
    //unsigned long start, end;   
    asm volatile("fence");
    //Set MMIO registers
    *srcAddrPtr = srcAddrValue;
    *destAddrPtr = destAddrValue;
    *inputHeightPtr = inputHeightValue; 
    *inputWidthPtr = inputWidthValue;

    memcpy((void*)kernelRegPtr, kernel, sizeof(int8_t) * 9);
    asm volatile("fence");
    // Start Convolution
    *readyPtr  = readyValue;
    asm volatile("fence");
    while (*readyPtr == 0) {};
    asm volatile("fence");
    //puts("Accel Convolution Complete");

}

// Software fallback for regular convolution on smaller regions
void software_convolution2d(int8_t *source, int16_t *dest, int width, int height, int8_t *kernel, int start_col, int end_col) {
    int output_width = width - 2;

    // Perform convolution for each pixel in the range
    for (int row = 1; row < height - 1; row++) {  // Exclude the boundary rows
        for (int col = start_col + 1; col < end_col; col++) {
            
            int32_t sum = 0;
            for (int kr = 0; kr < 3; kr++) {
                for (int kc = 0; kc < 3; kc++) {
                    int src_row = row + kr - 1;
                    int src_col = col + kc - 1;
                    sum += source[src_row * width + src_col] * kernel[kr * 3 + kc];
                }
                // Saturate
                if (sum > 32767) {
                    sum = 32767;
                }
                if (sum < -32768) {
                    sum = -32768;
                }
            }
            dest[(row - 1) * output_width + (col - 1)] = sum;
        }
    }
}

void convolution2d(int8_t *source, int16_t *dest, int width, int height, int8_t *kernel) {
    int output_width = width - 2;
    int output_height = height - 2;

    // Fixed-size buffers for the accelerator
    // Assuming maximum segment width is 64 and max height is reasonable
    // assert(width <= 1024 && height <= 1024); // Ensure the size is within the predefined limits
    int8_t temp_src[1024];  // 64 columns by maximum height
    int16_t temp_dest[62 * 1022];  // 62 columns by (maximum height - 2)

    // Process the image in overlapping 64-column-wide segments
    for (int col = 0; col < width; col += 62) {
        int segment_width = (col + 64 <= width) ? 64 : (width - col);

        if (segment_width == 64) {
            // Prepare segment for the hardware accelerator
            for (int row = 0; row < height; row++) {
                memcpy(&temp_src[row * 64], &source[row * width + col], 64);
            }

            // Perform hardware acceleration or fallback to software implementation
            // hardware_convolution(temp_src, temp_dest, 64, height, kernel);
            software_convolution2d(temp_src, temp_dest, 64, height, kernel, 0, 64);

            // Copy results back to the correct position in the destination
            for (int row = 0; row < output_height; row++) {
                memcpy(&dest[row * output_width + col], &temp_dest[row * 62], 62 * sizeof(int16_t));
            }

        } else {
            // Use software convolution for remaining columns
            int start_col = col;
            int end_col = width - 1;
            software_convolution2d(source, dest, width, height, kernel, start_col, end_col);
        }
    }
}


// void convolution2d(int8_t *source, int16_t *dest, int width, int height, int8_t *kernel) {
//     int output_width = width - 2;
//     int output_height = height - 2;

//     // Temporary buffers for the accelerator
//     int8_t *temp_src = (int8_t *) malloc(sizeof(int8_t) * 64 * height);  // 64 columns by full height
//     int16_t *temp_dest = (int16_t *) malloc(sizeof(int16_t) * 62 * (height - 2));

//     if (!temp_src || !temp_dest) {
//         printf("Memory allocation failed\n");
//         free(temp_src);
//         free(temp_dest);
//         return;
//     }

//     // Process the image in overlapping 64-column-wide segments
//     for (int col = 0; col < width; col += 62) {
//         int segment_width = (col + 64 <= width) ? 64 : (width - col);
//         if (segment_width == 64) {

//             // Prepare segment for the hardware accelerator
//             for (int row = 0; row < height; row++) {
//                 memcpy(&temp_src[row * 64], &source[row * width + col], 64);
//             }
            
//             //hardware_convolution(temp_src, temp_dest, 64, height, kernel);
//             software_convolution2d(temp_src, temp_dest, 64, height, kernel, 0, 64);

//             // Copy results back to the correct position in the destination
//             for (int row = 0; row < output_height; row++) {
//                 memcpy(&dest[row * output_width + col], &temp_dest[row * 62], 62 * sizeof(int16_t));
//             }

//         } else {
//             // Use software convolution for remaining columns
//             int start_col = col;
//             int end_col = width - 1;
//             software_convolution2d(source, dest, width, height, kernel, start_col, end_col);
//         }
//     }
// }

