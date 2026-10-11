/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/disassembly.h
 *
 * PURPOSE:
 *   Decode DAP disassemble response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_DISASSEMBLY_H
#define UMICOM_DEBUG_RUNTIME_DECODER_DISASSEMBLY_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode disassembly operation used by this module and its
 * client applications.
 */
/* Decode a complete successful DAP response before publishing output.
 * Excess rows, invalid fields and coordinates wider than this legacy result
 * are rejected. presentation_hint preserves unavailable filler instructions.
 * Failure leaves caller output unchanged; allocate this result on the heap. */
UmiStatus umi_debug_runtime_decode_disassembly(
    const char *json,
    UmiDebugRuntimeDisassembly *out_result);
#ifdef __cplusplus
}
#endif
#endif
