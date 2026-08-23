/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/write_memory.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol writeMemory request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_WRITE_MEMORY_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_WRITE_MEMORY_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
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
