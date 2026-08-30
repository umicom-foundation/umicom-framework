/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/read_memory.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol readMemory request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_READ_MEMORY_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_READ_MEMORY_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_read_memory(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    uint32_t count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
