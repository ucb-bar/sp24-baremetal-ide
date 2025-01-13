#include <stdint.h>
#include "uart_tsi.h"
#include "joints.h"

#define BASE_ADDR 0x81000000

#define MOTOR_TARGET_POS    BASE_ADDR + 0x00
#define MOTOR_TARGET_VEL    BASE_ADDR + 0x04

#define JOINT_STATE         BASE_ADDR + 0x08
#define JOINT_EN            BASE_ADDR + 0x0c
#define JOINT_DIR           BASE_ADDR + 0x10
#define MOTOR_PRESC         BASE_ADDR + 0x14

#define ENCODER_COUNT       BASE_ADDR + 0x20
#define ENCODER_SPEED       BASE_ADDR + 0x28
#define ENCODER_RESET       BASE_ADDR + 0x30

void joints_init(void) {
  uart_tsi_init();

  for(int i=0; i<8; i++) {
    // Set motor pre-scaler and direction
    uart_tsi_write(MOTOR_PRESC + (0x100 * i), 80);    
    uart_tsi_write(JOINT_DIR + (0x100 * i), 1);

    // Set targets/speeds to 0
    uart_tsi_write(MOTOR_TARGET_POS + (0x100 * i), 0);
    uart_tsi_write(MOTOR_TARGET_VEL + (0x100 * i), 0);

    // Enable motors
    uart_tsi_write(JOINT_EN + (0x100 * i), 1);          
  }

  // We reset the encoder values
  reset_encoders();
}

// Reset encoder counter values
void reset_encoders(void) {
  for (int i = 0; i < 8; i++) {
    uart_tsi_write(ENCODER_RESET + (0x100 * i), (uint32_t) 1);
  }
}

// Reset encoder counter values
void reset_encoder(int num) {
  uart_tsi_write(ENCODER_RESET + (0x100 * num), (uint32_t)1);
}

// Change the motor enable
void motor_set_en(int state) {
  for (int i = 0; i < 8; i++) {
    uart_tsi_write(JOINT_EN + (0x100 * i), (uint32_t) state);
  }
}

// Change the motor control state
void motor_set_state(int state) {
  for (int i = 0; i < 8; i++) {
    uart_tsi_write(JOINT_STATE + (0x100 * i), (uint32_t) state);
  }
}

// Set a single motor speed
void set_motor_speed(int num, int speed) {
  uart_tsi_write(MOTOR_TARGET_VEL + (0x100 * num), (uint32_t)(speed));
}

// Set a single motor target position
void set_motor_pos(int num, int pos) {
  uart_tsi_write(MOTOR_TARGET_POS + (0x100 * num), (uint32_t)(pos));
}

// Get encoder value
int get_encoder(int num) {
  return ((int)uart_tsi_read(ENCODER_COUNT + (0x100 * num)));
}