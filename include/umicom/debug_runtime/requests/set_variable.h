/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_variable.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setVariable request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_VARIABLE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_VARIABLE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request set variable operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_set_variable(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t variables_reference,
    const char *name,
    const char *value,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
