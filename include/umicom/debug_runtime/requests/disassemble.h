/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/disassemble.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol disassemble request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_DISASSEMBLE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_DISASSEMBLE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_disassemble(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    int64_t instruction_offset,
    uint32_t instruction_count,
    int resolve_symbols,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
