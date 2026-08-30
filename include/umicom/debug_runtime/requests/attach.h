/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/attach.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol attach request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_ATTACH_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_ATTACH_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_attach(
    UmiDebugRuntimeAdapter *adapter,
    const char *arguments_json,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
