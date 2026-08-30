/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_target_model.h
 *
 * PURPOSE:
 *   Represent one executable or attachable debug target in the workbench.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TARGET_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TARGET_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugTargetModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugTargetModel;

UmiStatus umi_debug_workbench_debug_target_model_init(UmiDebugWorkbenchDebugTargetModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_target_model_set_state(UmiDebugWorkbenchDebugTargetModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_target_model_set_selected(UmiDebugWorkbenchDebugTargetModel *model, bool selected);
UmiStatus umi_debug_workbench_debug_target_model_set_enabled(UmiDebugWorkbenchDebugTargetModel *model, bool enabled);
int umi_debug_workbench_debug_target_model_valid(const UmiDebugWorkbenchDebugTargetModel *model);

#ifdef __cplusplus
}
#endif
#endif
