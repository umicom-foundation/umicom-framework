/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/write_memory.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol writeMemory request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_WRITE_MEMORY_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_WRITE_MEMORY_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request write memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_write_memory(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    const char *base64_data,
    int allow_partial,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
