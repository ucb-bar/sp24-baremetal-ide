// switch.h
#ifndef SWITCH_h
#define SWITCH_h
#include <stdint.h>
#include "chip_config.h"
#include "i2c.h"


void switches_init();
uint8_t get_switches();
uint8_t read_switch(int num);

#endif