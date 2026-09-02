/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/memory.h
 *
 * PURPOSE:
 *   Decode DAP readMemory response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_MEMORY_H
#define UMICOM_DEBUG_RUNTIME_DECODER_MEMORY_H
#include "umicom/debug_runtime/decoder_support.h"
#include "umicom/debug_runtime/base64.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode memory operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_memory(
    const char *json,
    UmiDebugRuntimeMemoryResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
