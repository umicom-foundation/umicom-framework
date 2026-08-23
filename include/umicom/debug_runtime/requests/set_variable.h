/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_variable.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setVariable request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_VARIABLE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_VARIABLE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
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
