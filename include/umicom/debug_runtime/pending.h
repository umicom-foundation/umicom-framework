/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/pending.h
 *
 * PURPOSE:
 *   Track outstanding DAP requests by sequence, command and optional context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PENDING_H
#define UMICOM_DEBUG_RUNTIME_PENDING_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime pending request data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimePendingRequest {
    uint64_t sequence;
    char command[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char context[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    uint64_t created_sequence;
    int active;
} UmiDebugRuntimePendingRequest;

/**
 * Represent the debug runtime pending table data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimePendingTable {
    UmiDebugRuntimePendingRequest items[UMI_DEBUG_RUNTIME_MAX_PENDING];
    size_t active_count;
    uint64_t sequence;
} UmiDebugRuntimePendingTable;

/**
 * Initialise debug runtime pending from caller-provided values so later operations receive
 * a known state.
 */
void umi_debug_runtime_pending_init(UmiDebugRuntimePendingTable *table);
/**
 * Add debug runtime pending only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_runtime_pending_add(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    const char *command,
    const char *context);
/**
 * Provide the debug runtime pending take operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_pending_take(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request);
/**
 * Find debug runtime pending while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_runtime_pending_find(
    const UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
