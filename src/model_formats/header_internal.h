/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/header_internal.h
 *
 * PURPOSE:
 *   Share bounded Safetensors length decoding between memory-prefix and checked-file inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MODEL_FORMATS_HEADER_INTERNAL_H
#define UMICOM_MODEL_FORMATS_HEADER_INTERNAL_H
#include "umicom/model_formats/safetensors.h"
/* Call only after all eight bytes have been obtained from readable storage.
 * Decode explicitly so host alignment and endianness never affect admission. */
static inline UmiStatus SafetensorsReadHeaderLength(const unsigned char *bytes,uint64_t *out)
{
    uint64_t length=0U;
    for(size_t i=0U;i<8U;++i) length|=(uint64_t)bytes[i]<<(8U*i);
    if(length>UMI_SAFETENSORS_MAX_HEADER_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length<2U) return UMI_STATUS_PARSE_ERROR;
    *out=length;return UMI_STATUS_OK;
}
#endif
