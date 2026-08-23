/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/disassembly.h
 *
 * PURPOSE:
 *   Decode DAP disassemble response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_DISASSEMBLY_H
#define UMICOM_DEBUG_RUNTIME_DECODER_DISASSEMBLY_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_disassembly(
    const char *json,
    UmiDebugRuntimeDisassembly *out_result);
#ifdef __cplusplus
}
#endif
#endif
