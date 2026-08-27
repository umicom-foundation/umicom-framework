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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_THREAD_FOCUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_THREAD_FOCUS_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchThreadFocusModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchThreadFocusModel;

UmiStatus umi_debug_workbench_thread_focus_model_init(UmiDebugWorkbenchThreadFocusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_thread_focus_model_set_state(UmiDebugWorkbenchThreadFocusModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_thread_focus_model_set_selected(UmiDebugWorkbenchThreadFocusModel *model, bool selected);
UmiStatus umi_debug_workbench_thread_focus_model_set_enabled(UmiDebugWorkbenchThreadFocusModel *model, bool enabled);
int umi_debug_workbench_thread_focus_model_valid(const UmiDebugWorkbenchThreadFocusModel *model);

#ifdef __cplusplus
}
#endif
#endif
