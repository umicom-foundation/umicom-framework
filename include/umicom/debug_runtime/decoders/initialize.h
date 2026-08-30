/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/initialize.h
 *
 * PURPOSE:
 *   Decode DAP initialize response capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_INITIALIZE_H
#define UMICOM_DEBUG_RUNTIME_DECODER_INITIALIZE_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_initialize(
    const char *json,
    UmiDebugRuntimeCapabilities *out_result);
#ifdef __cplusplus
}
#endif
#endif
