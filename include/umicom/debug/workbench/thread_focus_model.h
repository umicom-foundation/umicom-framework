/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/thread_focus_model.h
 *
 * PURPOSE:
 *   Track the focused debug thread and user-versus-runtime focus provenance.
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
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_FOCUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_FOCUS_MODEL_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench thread focus model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchThreadFocusModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchThreadFocusModel;

/**
 * Initialise debug workbench thread focus model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_thread_focus_model_init(UmiDebugWorkbenchThreadFocusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench thread focus model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_thread_focus_model_set_state(UmiDebugWorkbenchThreadFocusModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench thread focus model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_thread_focus_model_set_selected(UmiDebugWorkbenchThreadFocusModel *model, bool selected);
/**
 * Provide the debug workbench thread focus model set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_thread_focus_model_set_enabled(UmiDebugWorkbenchThreadFocusModel *model, bool enabled);
/**
 * Check that debug workbench thread focus model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_thread_focus_model_valid(const UmiDebugWorkbenchThreadFocusModel *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_thread_focus_model_archive_encode(const UmiDebugWorkbenchThreadFocusModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_thread_focus_model_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchThreadFocusModel *value);

#ifdef __cplusplus
}
#endif
#endif
