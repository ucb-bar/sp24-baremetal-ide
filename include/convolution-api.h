#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

// Constants
#define MMIO_BASE   0x08808000
#define CACHELINE           64
#define WIDTH               64
#define HEIGHT              3

typedef enum {
    INT8,
    INT16
} DataType;


// Near Memory-Convolution
// this is the method that will call the accelerator
void nearmem_2d(int8_t* input, int8_t inputWidth, int8_t inputHeight,
              int8_t* kernel, int8_t kernelSize, int16_t* output) {



    int8_t outputWidth = inputWidth - kernelSize + 1;
    int8_t outputHeight = inputHeight - kernelSize + 1;

    // for(int i = 0; i < inputHeight; i++) {
    //     for(int j = 0; j < inputWidth; j++) {
    //         printf("%d ", input[i * inputWidth + j]);
    //     }
    //     printf("\n");
    // }

    // printf("In Nearmem 2D: w=%d h=%d, k=%d\n", inputWidth, inputHeight, kernelSize);
    for (int i = 0; i < outputHeight; i++) {
            for (int j = 0; j < outputWidth; j++) {
                int32_t sum = 0;

                // Iterate through kernel
                for (int m = 0; m < kernelSize; m++) {
                    for (int n = 0; n < kernelSize; n++) {
                        int ii = i + m;
                        int jj = j + n;

                        // Perform the multiplication and addition for the convolution
                        // printf("%d %d %d %d %d %d\n", ii, jj, m, n, input[ii * inputWidth + jj], kernel[m * kernelSize + n]);
                        sum += input[ii * inputWidth + jj] * kernel[m * kernelSize + n];
                    }
                }

                // Saturate
                if (sum > 32767) {
                    sum = 32767;
                }
                if (sum < -32768) {
                    sum = -32768;
                }

                // Store the result in the output matrix
                output[i * outputWidth + j] = sum;
                // printf("\n");
            }
    }

    // printf("\n\n2D-Output\n");
    // for(int i = 0; i < outputHeight; i++) {
    //     for(int j = 0; j < outputWidth; j++) {
    //         printf("%d ", output[i * outputWidth + j]);
    //     }
    //     printf("\n");
    // }
}

int8_t* pad_3d_matrix(int8_t* input, int8_t depth, int8_t width, int8_t height, int8_t padding) {
    // Calculate the new dimensions with padding
    int8_t paddedWidth = width + 2 * padding;
    int8_t paddedHeight = height + 2 * padding;
    
    // Calculate the size of the padded matrix
    int paddedSize = depth * paddedWidth * paddedHeight;
    
    // Allocate memory for the padded matrix
    int8_t* paddedOutput = (int8_t*)malloc(paddedSize * sizeof(int8_t));
    if (paddedOutput == NULL) {
        // Memory allocation failed
        return NULL;
    }
    
    // Iterate over each depth slice of the input matrix
    for (int d = 0; d < depth; d++) {
        // Pointer to the current depth slice of the input matrix
        int8_t* currentDepthSliceInput = input + d * width * height;
        
        // Pointer to the current depth slice of the padded output matrix
        int8_t* currentDepthSliceOutput = paddedOutput + d * paddedWidth * paddedHeight;
        
        // Pad each depth slice individually
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Copy elements from the input matrix to the padded output matrix
                currentDepthSliceOutput[(i + padding) * paddedWidth + (j + padding)] = currentDepthSliceInput[i * width + j];
            }
        }
        
        // Fill the top rows with zeros for padding
        for (int i = 0; i < padding; i++) {
            for (int j = 0; j < paddedWidth; j++) {
                currentDepthSliceOutput[i * paddedWidth + j] = 0;
            }
        }
        
        // Fill the bottom rows with zeros for padding
        for (int i = paddedHeight - padding; i < paddedHeight; i++) {
            for (int j = 0; j < paddedWidth; j++) {
                currentDepthSliceOutput[i * paddedWidth + j] = 0;
            }
        }
        
        // Fill the left columns with zeros for padding
        for (int i = 0; i < paddedHeight; i++) {
            for (int j = 0; j < padding; j++) {
                currentDepthSliceOutput[i * paddedWidth + j] = 0;
            }
        }
        
        // Fill the right columns with zeros for padding
        for (int i = 0; i < paddedHeight; i++) {
            for (int j = paddedWidth - padding; j < paddedWidth; j++) {
                currentDepthSliceOutput[i * paddedWidth + j] = 0;
            }
        }

    }
    
    return paddedOutput;
}

// 3D Convolution function
// 3D Convolution function
void nearmem_convolve(int8_t* input_no_padding, int8_t imageDepth, int8_t imageWidth, int8_t imageHeight,
                      int8_t* kernel, int8_t numKernels, int8_t kernelSize, int8_t stride, int8_t padding, int16_t* output) {

    // Calculate the output dimensions after convolution

    int8_t* input = pad_3d_matrix(input_no_padding, imageDepth, imageWidth, imageHeight, padding);
    int8_t outputHeightNoStride = (imageHeight + 2 * padding - kernelSize + 1);
    int8_t outputWidthNoStride  = (imageWidth + 2 * padding  - kernelSize + 1);

    int8_t outputHeight = (imageHeight + 2 * padding - kernelSize) / stride + 1;
    int8_t outputWidth = (imageWidth + 2 * padding - kernelSize) / stride + 1;

    // Ensure output buffer is properly allocated
    int outputSize = numKernels * outputHeight * outputWidth;
    memset(output, 0, outputSize * sizeof(int16_t)); // Initialize output to zero

    // printf("Number of kernels %d\n", numKernels);
    // printf("Image width: %d\n", imageWidth);
    // printf("Image height: %d\n", imageHeight);
    // printf("Image depth: %d\n", imageDepth);
    // printf("Output height: %d %d\n", outputHeight, imageHeight - kernelSize + 1);
    // printf("Output width: %d %d\n", outputWidth, imageHeight - kernelSize + 1);

    // for(int i = 0; i < imageDepth; i++) {
    //     for(int j = 0; j < imageWidth + 2 * padding; j++) {
    //         for(int k = 0; k < imageHeight + 2 * padding; k++) {
    //             printf("%d ", input[i * (imageWidth + 2 * padding) * (imageHeight + 2 * padding) + j * (imageWidth + 2 * padding) + k]);
    //         }
    //         printf("\n");
    //     }
    //     printf("\n");
    // }
    // printf("Done.\n");

    // printf("Printing Kernels: \n");
    // for(int a = 0; a < numKernels; a++){ 
    //     printf("\nKernel %d \n",a);
    //     for(int i = 0; i < imageDepth; i++) {
    //         for(int j = 0; j < kernelSize; j++) {
    //             for(int k = 0; k < kernelSize; k++) {
    //                 printf("%d ", kernel[a * kernelSize * kernelSize * imageDepth + i * kernelSize * kernelSize + j * kernelSize + k]);
    //             }
    //             printf("\n");
    //         }
    //         printf("\n");
    //     }
    // }
    // printf("Done.\n");

    // Iterate through each kernel
    for (int k = 0; k < numKernels; k++) {
        // Calculate the starting address of the current kernel
        int8_t* currentKernel = kernel + k * imageDepth * kernelSize * kernelSize;
        
        // printf("\n Printing Current Kernel: %d \n", k);
        // for(int i = 0; i < imageDepth; i++) {
        //     for(int j = 0; j < kernelSize; j++) {
        //         for(int k = 0; k < kernelSize; k++) {
        //             printf("%d ", currentKernel[i * kernelSize * kernelSize + j * kernelSize + k]);
        //         }
        //         printf("\n");
        //     }
        //     printf("\n");
        // }

        // Perform 2D convolution for each depth slice of the input
        for (int d = 0; d < imageDepth; d++) {
            // Calculate the starting address of the current depth slice
            // printf("Slice %d, imageDepth %d \n", d, imageDepth);

            int8_t* currentInput = input + d * imageWidth * imageHeight;
            int8_t* current2DKernel = currentKernel + d * kernelSize * kernelSize;

            int16_t tempOutput[outputHeightNoStride * outputWidthNoStride]; // Temporary output buffer
            // Perform 2D convolution using the nearmem_2d subroutine and accumulate results - no stride, no padding
            nearmem_2d(currentInput, imageWidth + 2 * padding, imageHeight + 2 * padding, current2DKernel, kernelSize, tempOutput);

            // printf("\nTemp Output for depth slice %d\n", d);
            // for(int i = 0; i < outputHeightNoStride; i++) {
            //     for(int j = 0; j < outputWidthNoStride; j++) {
            //         printf("%d ", tempOutput[i * outputWidthNoStride + j]);
            //     }
            //     printf("\n");
            // }
            // printf("\n");

            // Accumulate the 2D convolution results into the output for the current kernel's depth
            for (int i = 0; i < outputHeight; i++) {
                for (int j = 0; j < outputWidth; j++) {
                    int16_t val = tempOutput[i * outputWidthNoStride * stride + j * stride]; // Downsample based on stride
                    // printf("%d %d \n", i * outputWidthNoStride * stride, j + stride);
                    output[k * outputHeight * outputWidth + i * outputWidth + j] += val;
                }
            }

            // if(d == imageDepth - 1) {
            //     for (int i = 0; i < outputHeight; i++) {
            //         for (int j = 0; j < outputWidth; j++) {
            //             printf("%d ",output[k * outputHeight * outputWidth + i * outputWidth + j]);
            //         }
            //         printf("\n");
            //     }
            // }
        }
    }

    // printf("Output shape %d %d %d\n", numKernels, outputHeight, outputWidth);
}
