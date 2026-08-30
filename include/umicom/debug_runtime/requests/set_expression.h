/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_expression.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setExpression request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_EXPRESSION_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_EXPRESSION_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_set_expression(
    UmiDebugRuntimeAdapter *adapter,
    const char *expression,
    const char *value,
    uint64_t frame_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
