/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/exception_info.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol exceptionInfo request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_EXCEPTION_INFO_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_EXCEPTION_INFO_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_exception_info(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
