/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/restart.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol restart request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_RESTART_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_RESTART_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request restart operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_restart(
    UmiDebugRuntimeAdapter *adapter,
    const char *arguments_json,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
