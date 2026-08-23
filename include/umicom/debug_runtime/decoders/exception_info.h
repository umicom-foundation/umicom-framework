/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/exception_info.h
 *
 * PURPOSE:
 *   Decode DAP exceptionInfo response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_EXCEPTION_INFO_H
#define UMICOM_DEBUG_RUNTIME_DECODER_EXCEPTION_INFO_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_exception_info(
    const char *json,
    UmiDebugRuntimeExceptionInfo *out_result);
#ifdef __cplusplus
}
#endif
#endif
