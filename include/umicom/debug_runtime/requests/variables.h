/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/variables.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol variables request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_VARIABLES_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_VARIABLES_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request variables operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_variables(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t reference,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
