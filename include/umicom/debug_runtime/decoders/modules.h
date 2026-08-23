/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/modules.h
 *
 * PURPOSE:
 *   Decode DAP modules response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_MODULES_H
#define UMICOM_DEBUG_RUNTIME_DECODER_MODULES_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_modules(
    const char *json,
    UmiDebugRuntimeModuleList *out_result);
#ifdef __cplusplus
}
#endif
#endif
