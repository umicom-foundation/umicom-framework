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

/**
 * Represent the debug workbench debug target model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchDebugTargetModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugTargetModel;

/**
 * Initialise debug workbench debug target model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_target_model_init(UmiDebugWorkbenchDebugTargetModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug target model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_target_model_set_state(UmiDebugWorkbenchDebugTargetModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug target model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_target_model_set_selected(UmiDebugWorkbenchDebugTargetModel *model, bool selected);
/**
 * Provide the debug workbench debug target model set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_target_model_set_enabled(UmiDebugWorkbenchDebugTargetModel *model, bool enabled);
/**
 * Check that debug workbench debug target model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_target_model_valid(const UmiDebugWorkbenchDebugTargetModel *model);

#ifdef __cplusplus
}
#endif
#endif
