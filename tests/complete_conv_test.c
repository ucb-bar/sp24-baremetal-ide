#include <stdio.h>
#include <stdint.h>
#include "convolution-api.h"

int simple_pad_test() {
    // Example usage
    int8_t input[2][3][3] = {
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        },
        {
            {9, 8, 7},
            {6, 5, 4},
            {3, 2, 1}
        }
    };

    int8_t kernel[2][2][2] = {
        {
            {1, 1},
            {1, 1}
        },
        {
            {1, 0},
            {0, 1}
        }
    };

    int8_t expected_matrix[1][2][2] = {
        {
            {26, 28},
            {32, 34}
        },
    };

    int16_t output[1][2][2];

    nearmem_convolve(&input, 2, 3, 3, &kernel, 1, 2, 1, 0, &output);

    printf("\nVerifying ...\n");
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                printf("%d ", output[i][j][k]);
            }        
            printf("\n");
        }
        printf("\n");
    }
    
    // Verify output
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                if(output[i][j][k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", output[i][j][k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;

}

int simple_pad_test_two() {
    // Example usage
    int8_t input[2][3][3] = {
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        },
        {
            {9, 8, 7},
            {6, 5, 4},
            {3, 2, 1}
        }
    };

    int8_t kernel[2][2][2][2] = {
        {
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 0},
                {0, 1}
            }
        },
        {
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 0},
                {0, 1}
            }
        },
    };

    int8_t expected_matrix[2][2][2] = {
        {
            {26, 28},
            {32, 34}
        },
        {
            {26, 28},
            {32, 34}
        },
    };

    int16_t output[2][2][2];

    nearmem_convolve(&input, 2, 3, 3, &kernel, 2, 2, 1, 0, &output);

    printf("\nVerifying ...\n");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                printf("%d ", output[i][j][k]);
            }        
            printf("\n");
        }
        printf("\n");
    }
    
    // Verify output
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                if(output[i][j][k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", output[i][j][k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;
}

int small_stride_test() {
    // Example usage
    int8_t input[1][3][3] = {
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        },
    };

    int8_t kernel[1][2][2] = {
        {
            {1, 1},
            {1, 1}
        },
    };

    int8_t expected_matrix[1][1][1] = {
        {
            {12},
        },
    };

    int16_t output[1][1][1];

    nearmem_convolve(&input, 1, 3, 3, &kernel, 1, 2, 2, 0, &output);

    printf("\nVerifying ...\n");
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 1; j++) {
            for(int k = 0; k < 1; k++) {
                printf("%d ", output[i][j][k]);
            }        
            printf("\n");
        }
        printf("\n");
    }
    
    // Verify output
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 1; j++) {
            for(int k = 0; k < 1; k++) {
                if(output[i][j][k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", output[i][j][k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;

}

int small_stride_pad_test() {
    // Example usage
    int8_t input[1][3][3] = {
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        },
    };

    int8_t kernel[1][2][2] = {
        {
            {1, 1},
            {1, 1}
        },
    };

    int8_t expected_matrix[1][2][2] = {
        {
            {1, 5},
            {11, 28},
        },
    };

    int16_t output[1][2][2];

    nearmem_convolve(&input, 1, 3, 3, &kernel, 1, 2, 2, 1, &output);

    printf("\nVerifying ...\n");
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                printf("%d ", output[i][j][k]);
            }        
            printf("\n");
        }
        printf("\n");
    }
    
    // Verify output
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 2; k++) {
                if(output[i][j][k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", output[i][j][k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;

}

int main() {

    // if(simple_pad_test() == 0) {
    //     printf("Test 1 Failed\n");
    // } else {
    //     printf("Test 1 Passed.\n");
    // }

    // if(simple_pad_test_two() == 0) {
    //     printf("Test 2 Failed\n");
    // } else {
    //     printf("Test 2 Passed.\n");
    // }

    // if(small_stride_test() == 0) {
    //     printf("Test 3 - stride - Failed\n");
    // } else {
    //     printf("Test 3 Passed.\n");
    // }

    if(small_stride_pad_test() == 0) {
        printf("Test 4 - stride + pad - Failed\n");
    } else {
        printf("Test 4 Passed.\n");
    }
    return 0;
}