#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "cnn.h"

/* Canvas size for MNIST images */
#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT)

/* Clear a canvas */
void clear_canvas(uint8_t* canvas) {
    memset(canvas, 0, IMAGE_SIZE);
}

/* Draw a digit '0' on the canvas */
void draw_0(uint8_t* canvas) {
    for (int y = 8; y <= 20; y++) {
        for (int x = 8; x <= 20; x++) {
            if ((x == 8 || x == 20) && (y >= 8 && y <= 20)) {
                canvas[y * IMAGE_WIDTH + x] = 255;  // Vertical lines
            }
            if ((y == 8 || y == 20) && (x > 8 && x < 20)) {
                canvas[y * IMAGE_WIDTH + x] = 255;  // Horizontal lines
            }
        }
    }
}

/* Draw a digit '1' on the canvas */
void draw_1(uint8_t* canvas) {
    for (int y = 8; y <= 20; y++) {
        canvas[y * IMAGE_WIDTH + 14] = 255;  // Vertical line
    }
}

/* Draw a digit '2' on the canvas */
void draw_2(uint8_t* canvas) {
    for (int x = 8; x <= 20; x++) {
        canvas[8 * IMAGE_WIDTH + x] = 255;  // Top line
        canvas[14 * IMAGE_WIDTH + x] = 255; // Middle line
        canvas[20 * IMAGE_WIDTH + x] = 255; // Bottom line
    }
    for (int y = 8; y <= 14; y++) {
        canvas[y * IMAGE_WIDTH + 20] = 255;  // Top right vertical
    }
    for (int y = 14; y <= 20; y++) {
        canvas[y * IMAGE_WIDTH + 8] = 255;  // Bottom left vertical
    }
}

/* Draw additional digits... */
/* For brevity, similar methods would be added for digits '3' to '9'. */

/* Draw a digit on the canvas based on its label */
void draw_digit(uint8_t* canvas, uint8_t digit) {
    clear_canvas(canvas);
    switch (digit) {
        case 0: draw_0(canvas); break;
        case 1: draw_1(canvas); break;
        case 2: draw_2(canvas); break;
        /* Add calls to `draw_3`, ..., `draw_9` */
        default: clear_canvas(canvas); break;  // Clear if invalid digit
    }
}

/* Generate valid synthetic MNIST-like data */
void generate_valid_synthetic_data(uint8_t* images, uint8_t* labels, int n_samples) {
    for (int i = 0; i < n_samples; i++) {
        uint8_t digit = rand() % 3;  // Random digit 0-9
        uint8_t* canvas = &images[i * IMAGE_SIZE];
        draw_digit(canvas, digit);
        labels[i] = digit;
    }
}

/* Display a single image (for debugging) */
void print_image(uint8_t* image) {
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            printf("%c", image[y * IMAGE_WIDTH + x] > 128 ? '#' : '.');
        }
        printf("\n");
    }
}


/* Main */
int main(int argc, char* argv[]) {
    // srand(time(NULL));  // Initialize random seed

    /* Network configuration */
    int n_train_samples = 5000;
    int n_test_samples = 5;
    int image_size = 28 * 28;

    /* Allocate memory for synthetic data */
    printf("Memory allocation starting\n");
    uint8_t* train_images = (uint8_t*)malloc(n_train_samples * image_size);
    uint8_t* train_labels = (uint8_t*)malloc(n_train_samples);
    uint8_t* test_images = (uint8_t*)malloc(n_test_samples * image_size);
    uint8_t* test_labels = (uint8_t*)malloc(n_test_samples);
    printf("Memory allocation ending\n");

    if (!train_images || !train_labels || !test_images || !test_labels) {
        printf("Memory allocation failed\n");
        return 1;
    }


    /* Generate synthetic training and testing data */
    generate_valid_synthetic_data(train_images, train_labels, n_train_samples);
    generate_valid_synthetic_data(test_images, test_labels, n_test_samples);

    /* Display one sample image and its label (debugging) */
    printf("Sample image (label = %d):\n", train_labels[0]);
    print_image(&train_images[0]);

    /* Initialize layers */

    printf("Initializing layers\n");
    Layer* linput = Layer_create_input(1, 28, 28);
    printf("Initialized linput\n");
    Layer* lconv1 = Layer_create_conv(linput, 16, 14, 14, 3, 1, 2, 0.1);
    printf("Initialized lconv1\n");
    Layer* lconv2 = Layer_create_conv(lconv1, 32, 7, 7, 3, 1, 2, 0.1);
    printf("Initialized lconv2\n");
    Layer* lfull1 = Layer_create_full(lconv2, 200, 0.1);
    printf("Initialized lfull1\n");
    Layer* lfull2 = Layer_create_full(lfull1, 200, 0.1);
    printf("Initialized lfull2\n");
    Layer* loutput = Layer_create_full(lfull2, 10, 0.1);
    printf("Initialized loutput\n");

    /* Training */
    printf("training...\n");

    double rate = 0.1;
    double etotal = 0;
    int nepoch = 10;
    int batch_size = 32;

    printf("Training for %d steps\n", nepoch * n_train_samples);
    for (int i = 0; i < nepoch * n_train_samples; i++) {
        /* Pick a random sample */
        int index = rand() % n_train_samples;

        // printf("Picking index %d\n", index);
        uint8_t* img = &train_images[index * image_size];
        uint8_t label = train_labels[index];

        double x[28 * 28], y[10];
        for (int j = 0; j < image_size; j++) {
            x[j] = img[j] / 255.0;
        }

        Layer_setInputs(linput, x);
        Layer_getOutputs(loutput, y);

        for (int j = 0; j < 10; j++) {
            y[j] = (j == label) ? 1 : 0;
        }

        Layer_learnOutputs(loutput, y);

        etotal += Layer_getErrorTotal(loutput);

        if ((i % batch_size) == 0) {
            Layer_update(loutput, rate / batch_size);
        }
        if ((i % 10) == 0) {
            printf("i=%d, error=%.4f\n", i, etotal);
            etotal = 0;
        }
    }

    /* Testing */
    printf("testing...\n");
    int ncorrect = 0;
    for (int i = 0; i < n_test_samples; i++) {
        uint8_t* img = &test_images[i * image_size];
        uint8_t label = test_labels[i];

        double x[28 * 28], y[10];
        for (int j = 0; j < image_size; j++) {
            x[j] = img[j] / 255.0;
        }

        Layer_setInputs(linput, x);
        Layer_getOutputs(loutput, y);

        int predicted_label = 0;
        for (int j = 1; j < 10; j++) {
            if (y[j] > y[predicted_label]) {
                predicted_label = j;
            }
        }
        if (predicted_label == label) {
            ncorrect++;
        }
    }
    printf("ntests=%d, ncorrect=%d, accuracy=%.2f%%\n",
            n_test_samples, ncorrect, (ncorrect * 100.0) / n_test_samples);

    /* Clean up */
    free(train_images);
    free(train_labels);
    free(test_images);
    free(test_labels);

    Layer_destroy(linput);
    Layer_destroy(lconv1);
    Layer_destroy(lconv2);
    Layer_destroy(lfull1);
    Layer_destroy(lfull2);
    Layer_destroy(loutput);

    return 0;
}
