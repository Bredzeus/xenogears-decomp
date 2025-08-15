#include "common.h"
#include "libspu_internal.h"
#include "psyq/libdma.h"

long SpuSetIRQ(long on_off) {
    u_long dmaTimer;

    if ((on_off == SPU_OFF) || (on_off == SPU_RESET)) {
        _spu_RXX->rxx.spucnt &= ~SPU_STAT_MASK_IRQ9_FLAG;
        dmaTimer = 0;
        while (_spu_RXX->rxx.spucnt & SPU_STAT_MASK_IRQ9_FLAG) {
            if (++dmaTimer > DMA_TIMEOUT) {
                printf("SPU:T/O [%s]\n", "wait (IRQ/ON)");
                return SPU_ERROR;
            }
        }

    }
    if ((on_off == SPU_ON) || (on_off == SPU_RESET)) {
        _spu_RXX->rxx.spucnt |= SPU_STAT_MASK_IRQ9_FLAG;
        dmaTimer = 0;
        while ( !(_spu_RXX->rxx.spucnt & SPU_STAT_MASK_IRQ9_FLAG) ) {
            if (++dmaTimer > DMA_TIMEOUT) {
                printf("SPU:T/O [%s]\n", "wait (IRQ/OFF)");
                return SPU_ERROR;
            }
        }
    }
    return on_off;
}