/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/completions.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol completions request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_COMPLETIONS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_COMPLETIONS_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_completions(
    UmiDebugRuntimeAdapter *adapter,
    const char *text,
    uint32_t column,
    uint64_t frame_id,
    uint32_t line,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
