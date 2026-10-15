/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_command_set.h
 *
 * PURPOSE:
 *   Expose context-sensitive debugger commands for Studio and other thin frontends.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_COMMAND_SET_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_COMMAND_SET_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug command set data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchDebugCommandSet {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugCommandSet;

/**
 * Initialise debug workbench debug command set from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_command_set_init(UmiDebugWorkbenchDebugCommandSet *model);
/**
 * Provide the debug workbench debug command set set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_command_set_set_enabled(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command, bool enabled);
/**
 * Provide the debug workbench debug command set is enabled operation used by this module
 * and its client applications.
 */
int umi_debug_workbench_debug_command_set_is_enabled(const UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command);
/**
 * Provide the debug workbench debug command set set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_command_set_set_primary(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command);
/**
 * Check that debug workbench debug command set satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_command_set_valid(const UmiDebugWorkbenchDebugCommandSet *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_command_set_archive_encode(const UmiDebugWorkbenchDebugCommandSet *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_command_set_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugCommandSet *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_debug_workbench_debug_command_set_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDebugWorkbenchDebugCommandSet *expected, UmiDebugWorkbenchDebugCommandSet *value);


#ifdef __cplusplus
}
#endif
#endif
