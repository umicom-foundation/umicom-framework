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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_LOGPOINT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_LOGPOINT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench logpoint model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchLogpointModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchLogpointModel;

/**
 * Initialise debug workbench logpoint model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_logpoint_model_init(UmiDebugWorkbenchLogpointModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench logpoint model set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_logpoint_model_set_state(UmiDebugWorkbenchLogpointModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench logpoint model set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_workbench_logpoint_model_set_selected(UmiDebugWorkbenchLogpointModel *model, bool selected);
/**
 * Provide the debug workbench logpoint model set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_logpoint_model_set_enabled(UmiDebugWorkbenchLogpointModel *model, bool enabled);
/**
 * Check that debug workbench logpoint model satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_logpoint_model_valid(const UmiDebugWorkbenchLogpointModel *model);

#ifdef __cplusplus
}
#endif
#endif
