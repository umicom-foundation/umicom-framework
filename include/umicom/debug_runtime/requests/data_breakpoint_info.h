/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/data_breakpoint_info.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol dataBreakpointInfo request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_DATA_BREAKPOINT_INFO_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_DATA_BREAKPOINT_INFO_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_data_breakpoint_info(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t variables_reference,
    const char *name,
    uint64_t frame_id,
    const char *bytes,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
