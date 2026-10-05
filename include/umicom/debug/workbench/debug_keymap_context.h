/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_keymap_context.h
 *
 * PURPOSE:
 *   Represent debugger keymap activation state and command-context precedence.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_KEYMAP_CONTEXT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_KEYMAP_CONTEXT_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug keymap context data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugKeymapContext {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugKeymapContext;

/**
 * Initialise debug workbench debug keymap context from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_keymap_context_init(UmiDebugWorkbenchDebugKeymapContext *model);
/**
 * Provide the debug workbench debug keymap context set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_keymap_context_set_enabled(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command, bool enabled);
/**
 * Provide the debug workbench debug keymap context is enabled operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_keymap_context_is_enabled(const UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command);
/**
 * Provide the debug workbench debug keymap context set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_keymap_context_set_primary(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command);
/**
 * Check that debug workbench debug keymap context satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_keymap_context_valid(const UmiDebugWorkbenchDebugKeymapContext *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_keymap_context_archive_encode(const UmiDebugWorkbenchDebugKeymapContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_keymap_context_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugKeymapContext *value);

#ifdef __cplusplus
}
#endif
#endif
