/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_timeline_model.h
 *
 * PURPOSE:
 *   Maintain debugger timeline events and active checkpoint selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug timeline model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugTimelineModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugTimelineModel;

/**
 * Initialise debug workbench debug timeline model from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_timeline_model_init(UmiDebugWorkbenchDebugTimelineModel *model);
/**
 * Add debug workbench debug timeline model only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_debug_timeline_model_add(UmiDebugWorkbenchDebugTimelineModel *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug timeline model while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_timeline_model_remove(UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
/**
 * Provide the debug workbench debug timeline model select operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_timeline_model_select(UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
/**
 * Find debug workbench debug timeline model while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_timeline_model_find(const UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
/**
 * Check that debug workbench debug timeline model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_timeline_model_valid(const UmiDebugWorkbenchDebugTimelineModel *model);

#ifdef __cplusplus
}
#endif
#endif
