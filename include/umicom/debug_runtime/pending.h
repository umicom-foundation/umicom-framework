/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/pending.h
 *
 * PURPOSE:
 *   Track outstanding DAP requests by sequence, command and optional context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PENDING_H
#define UMICOM_DEBUG_RUNTIME_PENDING_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugRuntimePendingRequest {
    uint64_t sequence;
    char command[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char context[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    uint64_t created_sequence;
    int active;
} UmiDebugRuntimePendingRequest;

typedef struct UmiDebugRuntimePendingTable {
    UmiDebugRuntimePendingRequest items[UMI_DEBUG_RUNTIME_MAX_PENDING];
    size_t active_count;
    uint64_t sequence;
} UmiDebugRuntimePendingTable;

void umi_debug_runtime_pending_init(UmiDebugRuntimePendingTable *table);
UmiStatus umi_debug_runtime_pending_add(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    const char *command,
    const char *context);
UmiStatus umi_debug_runtime_pending_take(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request);
UmiStatus umi_debug_runtime_pending_find(
    const UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
