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

typedef struct UmiDebugWorkbenchStackFrameFocusModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchStackFrameFocusModel;

UmiStatus umi_debug_workbench_stack_frame_focus_model_init(UmiDebugWorkbenchStackFrameFocusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_state(UmiDebugWorkbenchStackFrameFocusModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_selected(UmiDebugWorkbenchStackFrameFocusModel *model, bool selected);
UmiStatus umi_debug_workbench_stack_frame_focus_model_set_enabled(UmiDebugWorkbenchStackFrameFocusModel *model, bool enabled);
int umi_debug_workbench_stack_frame_focus_model_valid(const UmiDebugWorkbenchStackFrameFocusModel *model);

#ifdef __cplusplus
}
#endif
#endif
