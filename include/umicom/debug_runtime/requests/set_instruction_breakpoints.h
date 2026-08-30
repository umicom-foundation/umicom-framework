/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_instruction_breakpoints.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setInstructionBreakpoints request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_INSTRUCTION_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_INSTRUCTION_BREAKPOINTS_H
#include "umicom/debug_runtime/request_support.h"
#include "umicom/debug_runtime/instruction_breakpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_set_instruction_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const UmiDebugRuntimeInstructionBreakpoint *breakpoints,
    size_t breakpoint_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
