/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/goto_targets.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol gotoTargets request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_GOTO_TARGETS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_GOTO_TARGETS_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_goto_targets(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint32_t line,
    uint32_t column,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
