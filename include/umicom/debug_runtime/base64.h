/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/base64.h
 *
 * PURPOSE:
 *   Decode bounded base64 memory payloads returned by DAP readMemory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_BASE64_H
#define UMICOM_DEBUG_RUNTIME_BASE64_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_base64_decode(
    const char *text,
    unsigned char *out_bytes,
    size_t capacity,
    size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
