/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/variables.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol variables request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_VARIABLES_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_VARIABLES_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_variables(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t reference,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
