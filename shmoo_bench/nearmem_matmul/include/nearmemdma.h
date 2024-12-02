#ifndef NEARMEMDMA_H
#define NEARMEMDMA_H

#include <stdint.h>

typedef struct dma_status {
    uint64_t error : 1;
    uint64_t completed : 1;
    uint64_t inProgress : 1;
} dma_status;

_Static_assert(sizeof(dma_status) == 8, "dma_status size check");
#define MODE_COPY 0
#define MODE_MAC 2

typedef struct dma {
    dma_status status;
    uint64_t mode;
    void* srcAddr;
    void* destAddr;
    uint64_t srcStride;
    uint32_t count;
    uint64_t unused0[2];
    uint64_t operandReg[8];
    uint64_t destReg[8];
} dma;

#define DMA1 ((volatile dma*) 0x8800000)
#define DMA2 ((volatile dma*) 0x8802000)
#define DMA3 ((volatile dma*) 0x8804000)
#define DMA4 ((volatile dma*) 0x8806000)

#endif // NEARMEMDMA_H
