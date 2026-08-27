/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/logpoint_model.h
 *
 * PURPOSE:
 *   Represent non-stopping logpoint message state and source binding.
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
#ifndef UMICOM_DEBUG_WORKBENCH_LOGPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_LOGPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchLogpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchLogpointModel;

UmiStatus umi_debug_workbench_logpoint_model_init(UmiDebugWorkbenchLogpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_logpoint_model_set_state(UmiDebugWorkbenchLogpointModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_logpoint_model_set_selected(UmiDebugWorkbenchLogpointModel *model, bool selected);
UmiStatus umi_debug_workbench_logpoint_model_set_enabled(UmiDebugWorkbenchLogpointModel *model, bool enabled);
int umi_debug_workbench_logpoint_model_valid(const UmiDebugWorkbenchLogpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
