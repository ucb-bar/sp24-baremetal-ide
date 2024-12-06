#ifndef __HARDWARE_CONF_H_
#define __HARDWARE_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif


// #include "bearly24.h"

/**
 * This section controls which peripheral device is included in the application program.
 * To save the memory space, the unused peripheral device can be commented out.
 */

// Hardware Enable //

/**
 * Enables the Quantized Transformer V_DOTPROD function.
 */
#define ENABLE_QT_DOTPROD

/**
 * Enables the DMA MatVec functionality to speed up matmul computations through DMA0.
 */
#define ENABLE_DMA_MATVEC


// Accelerator library inclusions //
#include "riscv.h"

#ifdef ENABLE_QT_DOTPROD
#include "hal_qt.h"
#endif

#ifdef ENABLE_DMA_MATVEC
#include "hal_dma.h"
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_CONF_H */
