/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/stack_frame_focus_model.h
 *
 * PURPOSE:
 *   Track active frame focus and source synchronization state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_STACK_FRAME_FOCUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_STACK_FRAME_FOCUS_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench stack frame focus model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchStackFrameFocusModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchStackFrameFocusModel;

/**
 * Initialise debug workbench stack frame focus model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_stack_frame_focus_model_init(UmiDebugWorkbenchStackFrameFocusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench stack frame focus model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_state(UmiDebugWorkbenchStackFrameFocusModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench stack frame focus model set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_selected(UmiDebugWorkbenchStackFrameFocusModel *model, bool selected);
/**
 * Provide the debug workbench stack frame focus model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_enabled(UmiDebugWorkbenchStackFrameFocusModel *model, bool enabled);
/**
 * Check that debug workbench stack frame focus model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_stack_frame_focus_model_valid(const UmiDebugWorkbenchStackFrameFocusModel *model);

#ifdef __cplusplus
}
#endif
#endif
