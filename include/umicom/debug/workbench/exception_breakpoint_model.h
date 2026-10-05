/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/exception_breakpoint_model.h
 *
 * PURPOSE:
 *   Represent exception-filter breakpoint selection and presentation state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_EXCEPTION_BREAKPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_EXCEPTION_BREAKPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench exception breakpoint model data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchExceptionBreakpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchExceptionBreakpointModel;

/**
 * Initialise debug workbench exception breakpoint model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_init(UmiDebugWorkbenchExceptionBreakpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench exception breakpoint model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_state(UmiDebugWorkbenchExceptionBreakpointModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench exception breakpoint model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_selected(UmiDebugWorkbenchExceptionBreakpointModel *model, bool selected);
/**
 * Provide the debug workbench exception breakpoint model set enabled operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_exception_breakpoint_model_set_enabled(UmiDebugWorkbenchExceptionBreakpointModel *model, bool enabled);
/**
 * Check that debug workbench exception breakpoint model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_exception_breakpoint_model_valid(const UmiDebugWorkbenchExceptionBreakpointModel *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_exception_breakpoint_model_archive_encode(const UmiDebugWorkbenchExceptionBreakpointModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_exception_breakpoint_model_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchExceptionBreakpointModel *value);

#ifdef __cplusplus
}
#endif
#endif
