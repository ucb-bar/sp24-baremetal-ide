#include <stdio.h>
#include <stdint.h>
#include "convolution-api.h"

int simple_pad_test() {
    int8_t matrix [2][3][3] = {
        {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        },
        {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        },
    };

    int8_t expected_matrix [2][6][6] = {
        {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
        },
        {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
        },
    };

    // Call our padding function 
    int8_t* padded_matrix = pad_3d_matrix(&matrix, 2, 3, 3, 2);

    printf("\nExpected Output\n");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 7; j++) {
            for(int k = 0; k < 7; k++) {
                printf("%d ", padded_matrix[i * 7 * 7 + j * 7 + k]);
            }        
            printf("\n");
        }
        printf("\n");
    }

    printf("\nVerifying ...\n");
    // Verify output
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 7; j++) {
            for(int k = 0; k < 7; k++) {
                if(padded_matrix[i * 7 * 7 + j * 7 + k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", padded_matrix[i * 7 * 7 + j * 7 + k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;
}

int simple_two_pad_test() {
    int8_t matrix [2][3][3] = {
        {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        },
        {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        },
    };

    int8_t expected_matrix [2][5][5] = {
        {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 0, 0, 0}
        }
    };

    // Call our padding function 
    int8_t* padded_matrix = pad_3d_matrix(&matrix, 2, 3, 3, 1);

    printf("\nExpected Output\n");
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 5; j++) {
            for(int k = 0; k < 5; k++) {
                printf("%d ", padded_matrix[i * 5 * 5 + j * 5 + k]);
            }        
            printf("\n");
        }
        printf("\n");
    }

    printf("\nVerifying ...\n");
    // Verify output
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 5; j++) {
            for(int k = 0; k < 5; k++) {
                if(padded_matrix[i * 5 * 5 + j * 5 + k] != expected_matrix[i][j][k]) {
                    printf("i = %d j = %d k = %d\n", i, j, k);
                    printf("Fail: %d %d\n", padded_matrix[i * 5 * 5 + j * 5 + k], expected_matrix[i][j][k]);
                    return 0;
                }
            }        
        }
    }
    
    return 1;
}

int main()
{
    if(simple_pad_test() == 0) {
        printf("Test 1 Failed.\n");
    } else {
        printf("Test 1 Passed.\n");
    }

    if(simple_two_pad_test() == 0) {
        printf("Test 2 Failed.\n");
    } else {
        printf("Test 2 Passed.\n");
    }
    return 0;
}