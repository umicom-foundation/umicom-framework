/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/source.h
 *
 * PURPOSE:
 *   Decode DAP source response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_SOURCE_H
#define UMICOM_DEBUG_RUNTIME_DECODER_SOURCE_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_source(
    const char *json,
    UmiDebugRuntimeSourceResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
