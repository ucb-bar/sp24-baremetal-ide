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

#define ENABLE_QT_DOTPROD


#include "riscv.h"
#include "hal_dma.h"

#ifdef ENABLE_QT_DOTPROD
#include "hal_qt.h"
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __HAL_CONF_H */