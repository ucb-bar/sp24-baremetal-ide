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


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "chip_config.h"
#include <math.h>

void print_encoders();
void print_home_buttons();
void passthrough_speeds();
void passthrough_positions();

int motor_speeds[8];
int motor_positions[8];

void app_init() {

  // +------------------------------------------------+
  // | Initialize the controller
  // +------------------------------------------------+
  printf("[START INIT]\n");
  switches_init();
  joints_init();

  // +------------------------------------------------+
  // | Start initial homing sequence
  // +------------------------------------------------+
  printf("[START HOME]\n");
  // home_motors();

  // motor_set_state(1);

  

  sleep(3);
  
}

void app_main() {
  uint64_t mhartid = READ_CSR("mhartid");
  while (1) {
    // step();
    print_home_buttons();
    // print_encoders();
    // printf("sadly unalive myself from hart : %d\r\n", mhartid);
    msleep(100);

  }
}


void print_encoders() {
  int enc[8];

  for (int i = 0; i < 8; i++) {
    enc[i] = get_encoder(i);
  }

  printf("[%d, %d, %d, %d, %d, %d, %d, %d]\n", enc[0], enc[1], enc[2], enc[3], enc[4], enc[5], enc[6], enc[7]);
}

void print_home_buttons() {
  printf("[%d, %d, %d, %d, %d, %d, %d, %d]\n", read_switch(0), read_switch(1), read_switch(2), read_switch(3), read_switch(4), read_switch(5), read_switch(6), read_switch(7));
}

void passthrough_speeds() {
  for (int i = 0; i < 8; i++) {
    set_motor_speed(i, motor_speeds[i]);
  }
}

void passthrough_positions() {
  for (int i = 0; i < 8; i++) {
    set_motor_pos(i, motor_positions[i]);
  }
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(int argc, char **argv) {
  /* MCU Configuration--------------------------------------------------------*/
  // Initialize UART0 for Serial Monitor
  UART_InitType UART0_init_config;
  UART0_init_config.baudrate = 115200;
  UART0_init_config.mode = UART_MODE_TX_RX;
  UART0_init_config.stopbits = UART_STOPBITS_2;
  uart_init(UART0, &UART0_init_config);


  /* Initialize all configured peripherals */
  printf("-----Initialize App-----\r\n");
  app_init();

  /* Infinite loop */
  printf("-----Start Main-----\r\n");
  while (1) {
    app_main();
  }
  return 0;
}

/*
 * Main function for secondary harts
 *
 * Multi-threaded programs should provide their own implementation.
 */
void __attribute__((weak, noreturn)) __main(void) {
  uint64_t mhartid = READ_CSR("mhartid");
  while (1) {
    asm volatile("wfi");
  }
}