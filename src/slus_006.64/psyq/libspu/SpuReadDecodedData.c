#include "common.h"

#include "libspu_internal.h"

// TODO(jperos): Replace these constants as soon as the docs make sense
long SpuReadDecodedData(SpuDecodedData *d_data, long flag) {
    switch (flag) {
        case SPU_CDONLY:
            _spu_Fr_(d_data->cd_left, 0, 0x20);
            break;
        case SPU_VOICEONLY:
            _spu_Fr_(d_data->voice1, 0x100, 0x20);
            break;
        default:
            _spu_Fr_(d_data->cd_left, 0, 0x40);
            break;
    }

    if (_spu_RXX->rxx.spustat & SPU_STAT_CAPTURE_SECONDHALF) {
        return SPU_DECODE_SECONDHALF;
    } else {
        return SPU_DECODE_FIRSTHALF;
    }
}