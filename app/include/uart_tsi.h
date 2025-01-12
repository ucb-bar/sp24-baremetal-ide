// UART_TSI.h
#ifndef UART_TSI_h
#define UART_TSI_h
#include <stdint.h>
#include "uart.h"

void uart_tsi_init(void);

int uart_tsi_read(uint64_t address);
void uart_tsi_write(uint64_t address, uint32_t data);

#endif