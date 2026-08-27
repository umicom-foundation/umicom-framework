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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugTimelineModel {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugTimelineModel;

void umi_debug_workbench_debug_timeline_model_init(UmiDebugWorkbenchDebugTimelineModel *model);
UmiStatus umi_debug_workbench_debug_timeline_model_add(UmiDebugWorkbenchDebugTimelineModel *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_timeline_model_remove(UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
UmiStatus umi_debug_workbench_debug_timeline_model_select(UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_timeline_model_find(const UmiDebugWorkbenchDebugTimelineModel *model, const char *id);
int umi_debug_workbench_debug_timeline_model_valid(const UmiDebugWorkbenchDebugTimelineModel *model);

#ifdef __cplusplus
}
#endif
#endif
