/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/breakpoint_locations.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol breakpointLocations request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_BREAKPOINT_LOCATIONS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_BREAKPOINT_LOCATIONS_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request breakpoint locations operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_request_breakpoint_locations(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint32_t line,
    uint32_t column,
    uint32_t end_line,
    uint32_t end_column,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
