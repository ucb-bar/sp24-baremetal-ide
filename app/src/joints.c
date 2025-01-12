#include <stdint.h>
#include "uart_tsi.h"
#include "joints.h"

#define MOTOR_BASE_ADDR 0x13000000

#define MOTOR_TARGET        MOTOR_BASE_ADDR + 0x00
#define JOINT_STATE         MOTOR_BASE_ADDR + 0x04
#define JOINT_EN            MOTOR_BASE_ADDR + 0x08
#define JOINT_DIR           MOTOR_BASE_ADDR + 0x0C

#define MOTOR_SPEED         MOTOR_BASE_ADDR + 0x20
#define MOTOR_PRESC         MOTOR_BASE_ADDR + 0x24

#define ENCODER_COUNT       MOTOR_BASE_ADDR + 0x30
#define ENCODER_RESET       MOTOR_BASE_ADDR + 0x34

// int DIR[8] = {0, 1, 0, 1, 0, 1, 1, 1};
// int MOVE_DIR[8] = {-1, 1, 1, -1, -1, 1, -1, 1};

int DIR[8] = {1, 0, 1, 0, 1, 0, 0, 0};
int MOVE_DIR[8] = {1, -1, -1, 1, 1, -1, 1, -1};

void joints_init(void) {
  uart_tsi_init();

  for(int i=0; i<8; i++) {
    // Set motor pre-scaler and direction
    uart_tsi_write(MOTOR_PRESC + (0x100 * i), 200);    
    uart_tsi_write(JOINT_DIR + (0x100 * i), DIR[i]);

    // Set targets/speeds to 0
    uart_tsi_write(MOTOR_TARGET + (0x100 * i), 0);
    uart_tsi_write(MOTOR_SPEED + (0x100 * i), 0);

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
  uart_tsi_write(MOTOR_SPEED + (0x100 * num), (uint32_t)(speed * MOVE_DIR[num]));
}

// Set a single motor target position
void set_motor_pos(int num, int pos) {
  uart_tsi_write(MOTOR_TARGET + (0x100 * num), (uint32_t)(pos * MOVE_DIR[num]));
}

// Get encoder value
int get_encoder(int num) {
  return ((int)uart_tsi_read(ENCODER_COUNT + (0x100 * num))) * MOVE_DIR[num];
}