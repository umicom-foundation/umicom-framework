/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/launch.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol launch request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_LAUNCH_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_LAUNCH_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_launch(
    UmiDebugRuntimeAdapter *adapter,
    const char *arguments_json,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
