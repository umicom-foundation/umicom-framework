/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/data_breakpoint_info.h
 *
 * PURPOSE:
 *   Decode DAP dataBreakpointInfo response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_DATA_BREAKPOINT_INFO_H
#define UMICOM_DEBUG_RUNTIME_DECODER_DATA_BREAKPOINT_INFO_H
#include "umicom/debug_runtime/decoder_support.h"
#include "umicom/language_runtime/json_writer.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode data breakpoint info operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_decode_data_breakpoint_info(
    const char *json,
    UmiDebugRuntimeDataBreakpointInfo *out_result);
#ifdef __cplusplus
}
#endif
#endif
