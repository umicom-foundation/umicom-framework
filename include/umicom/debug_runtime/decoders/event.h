/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/event.h
 *
 * PURPOSE:
 *   Decode DAP asynchronous event body.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_EVENT_H
#define UMICOM_DEBUG_RUNTIME_DECODER_EVENT_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_event(
    const char *json,
    UmiDebugRuntimeEvent *out_result);
#ifdef __cplusplus
}
#endif
#endif
