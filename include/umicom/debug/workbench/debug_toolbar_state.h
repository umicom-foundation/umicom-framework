/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_toolbar_state.h
 *
 * PURPOSE:
 *   Model debugger toolbar command availability independently of any GUI toolkit.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TOOLBAR_STATE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TOOLBAR_STATE_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug toolbar state data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugToolbarState {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugToolbarState;

/**
 * Initialise debug workbench debug toolbar state from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_toolbar_state_init(UmiDebugWorkbenchDebugToolbarState *model);
/**
 * Provide the debug workbench debug toolbar state set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_toolbar_state_set_enabled(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command, bool enabled);
/**
 * Provide the debug workbench debug toolbar state is enabled operation used by this module
 * and its client applications.
 */
int umi_debug_workbench_debug_toolbar_state_is_enabled(const UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command);
/**
 * Provide the debug workbench debug toolbar state set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_toolbar_state_set_primary(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command);
/**
 * Check that debug workbench debug toolbar state satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_toolbar_state_valid(const UmiDebugWorkbenchDebugToolbarState *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_toolbar_state_archive_encode(const UmiDebugWorkbenchDebugToolbarState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_toolbar_state_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugToolbarState *value);

#ifdef __cplusplus
}
#endif
#endif
