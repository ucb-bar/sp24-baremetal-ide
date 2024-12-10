// CONV_utils.h
#ifndef CONV_UTILS_h
#define CONV_UTILS_h

#include "DMA_utils.h"

// Addresses for Convolution
#define BASE_ADDR 0x08800000

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C
#define ISFLOAT_ADDR   0x0880008E

#define READ_CHECK_ADDR   0x0880008D

int set_conv_params(int len, uint16_t in_dilation, uint16_t* in_kernel) {
    reg_write32(LENGTH_ADDR, len);
    reg_write8(ISFLOAT_ADDR, 0);
    reg_write16(DILATION_ADDR, in_dilation);
    reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel));         // 64 bits: 4 FP16s
    reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel + 4)));   // 64 bits: 4 FP16s (Total 8)
}

void write_conv_dma(int dma_num, int length, uint64_t* data) {
    set_DMAC(dma_num, (uint64_t*) data, INPUT_ADDR, 8, 0, length/8, 3);
    start_DMA(dma_num);
}

void read_conv_dma(int dma_num, int length, uint64_t* write_addr) {
    set_DMAC(dma_num, OUTPUT_ADDR, write_addr, 0, 8, length/4, 3);
    start_DMA(dma_num);
}

void read_conv_dma_p(int dma_num, int length, uint64_t* write_addr) {
    set_DMAP(dma_num, OUTPUT_ADDR, write_addr, READ_CHECK_ADDR, 0, 8, length/4, 3, 0);
    start_DMA(dma_num);
}

void start_conv() {
    reg_write8(START_ADDR, 1);
}

#endif
