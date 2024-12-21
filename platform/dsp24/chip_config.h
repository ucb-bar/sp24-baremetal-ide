#ifndef __CHIP_CONFIG_H
#define __CHIP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv.h"
#include "clint.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "freq.h"
#include "pll.h"

// ================================
//  MMIO devices
// ================================
#define DEBUG_CONTROLLER_BASE   0x00000000U
#define BOOTROM_BASE            0x00010000U
#define RCC_BASE                0x00100000U
#define CLINT_BASE              0x02000000U
#define CACHE_CONTROLLER_BASE   0x02010000U
#define SCRATCH_BASE            0x08000000U
#define PLIC_BASE               0x0C000000U
#define CLOCK_SELECTOR_BASE     0x00130000U
#define PLL_BASE                0x00140000U
#define UART_BASE               0x10020000U
#define QSPI_FLASH_BASE         0x20000000U
#define DRAM_BASE               0x80000000U

#define FFT_BASE                0x08700000U
#define CONV_BASE               0x08800000U
#define DMA_BASE                0x08810000U
#define I2S_BASE                0x10042000U

/* Peripheral Pointer Definition */
#define UART0_BASE              (UART_BASE)

/* Peripheral Structure Definition */
#define RCC                     ((RCC_Type *)RCC_BASE)
#define CLOCK_SELECTOR          ((ClockSel_Type*)CLOCK_SELECTOR_BASE)
#define PLL                     ((PLL_Type *)PLL_BASE)
#define CLINT                   ((CLINT_Type *)CLINT_BASE)
#define PLIC                    ((PLIC_Type *)PLIC_BASE)
#define PLIC_CC                 ((PLIC_ContextControl_Type *)(PLIC_BASE + 0x00200000U))
#define UART0                   ((UART_Type *)UART0_BASE)




#ifdef __cplusplus
}
#endif

#endif // __CHIP_CONFIG_H