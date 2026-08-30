/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/breakpoints.h
 *
 * PURPOSE:
 *   Decode DAP setBreakpoints-family response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_DECODER_BREAKPOINTS_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_breakpoints(
    const char *json,
    UmiDebugRuntimeBreakpointList *out_result);
#ifdef __cplusplus
}
#endif
#endif
