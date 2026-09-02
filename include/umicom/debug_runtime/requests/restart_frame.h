/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/restart_frame.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol restartFrame request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_RESTART_FRAME_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_RESTART_FRAME_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request restart frame operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_restart_frame(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t frame_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
