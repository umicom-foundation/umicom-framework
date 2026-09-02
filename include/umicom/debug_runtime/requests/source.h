/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/source.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol source request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SOURCE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SOURCE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request source operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_source(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint64_t source_reference,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
