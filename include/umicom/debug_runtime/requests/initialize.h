/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/initialize.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol initialize request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_INITIALIZE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_INITIALIZE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise debug runtime request from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_runtime_request_initialize(
    UmiDebugRuntimeAdapter *adapter,
    const char *adapter_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
