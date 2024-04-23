#include <stdio.h>
#include <stdint.h>
#include "convolution-api.h"

int simple_pad_test() {
    // Example usage
    int8_t input[3][3] = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
    };

    int8_t kernel[2][2] = {
            {1, 1},
            {1, 1}
    };

    int16_t expected_matrix[2][2] = {
            {12, 16},
            {24, 28}
    };

    int16_t output[2][2];

    nearmem_2d((int8_t* ) &input, 3, 3, (int8_t*) &kernel, 2, (int16_t*) &output);

    // Print out complete output
    printf("\nPrinting Output\n");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            printf("%d ", output[i][j]);
        }
        printf("\n");
    }

    printf("\nVerifying ...\n");
    // Verify output
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            if(output[i][j] != expected_matrix[i][j]) {
                printf("i = %d j = %d\n", i, j);
                printf("Fail: %d %d\n", output[i][j], expected_matrix[i][j]);
                return 0;
            }
        }
    }
    
    return 1;

}

int simple_pad_test_two() {
    // Example usage
    int8_t input[3][3] = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
    };

    int8_t kernel[3][3] = {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
    };

    int16_t expected_matrix[1][1] = {
            {45},
    };

    int16_t output[1][1];

    nearmem_2d(&input, 3, 3, &kernel, 3, &output);

    // Print out complete output
    printf("\nPrinting Output\n");
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 1; j++) {
            printf("%d ", output[i][j]);
        }
        printf("\n");
    }

    printf("\nVerifying ...\n");
    // Verify output
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 1; j++) {
            if(output[i][j] != expected_matrix[i][j]) {
                printf("i = %d j = %d\n", i, j);
                printf("Fail: %d %d\n", output[i][j], expected_matrix[i][j]);
                return 0;
            }
        }
    }
    
    return 1;

}


int main() {

    if(simple_pad_test() == 0) {
        printf("Test 1 Failed\n");
    } else {
        printf("Test 1 Passed.\n");
    }

    if(simple_pad_test_two() == 0) {
        printf("Test 2 Failed\n");
    } else {
        printf("Test 2 Passed.\n");
    }
    return 0;
}