/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/modules.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol modules request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_MODULES_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_MODULES_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request modules operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_modules(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t start_module,
    uint32_t module_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
