/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/event.h
 *
 * PURPOSE:
 *   Decode DAP asynchronous event body.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_EVENT_H
#define UMICOM_DEBUG_RUNTIME_DECODER_EVENT_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode event operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_event(
    const char *json,
    UmiDebugRuntimeEvent *out_result);
#ifdef __cplusplus
}
#endif
#endif
