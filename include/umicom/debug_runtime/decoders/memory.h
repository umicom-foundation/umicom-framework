/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/memory.h
 *
 * PURPOSE:
 *   Decode DAP readMemory response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_MEMORY_H
#define UMICOM_DEBUG_RUNTIME_DECODER_MEMORY_H
#include "umicom/debug_runtime/decoder_support.h"
#include "umicom/debug_runtime/base64.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_memory(
    const char *json,
    UmiDebugRuntimeMemoryResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
