/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/watch_result_model.h
 *
 * PURPOSE:
 *   Represent the latest result of a watch-expression evaluation.
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
#ifndef UMICOM_DEBUG_WORKBENCH_WATCH_RESULT_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_WATCH_RESULT_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench watch result model data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchWatchResultModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchWatchResultModel;

/**
 * Initialise debug workbench watch result model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_watch_result_model_init(UmiDebugWorkbenchWatchResultModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench watch result model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_watch_result_model_set_state(UmiDebugWorkbenchWatchResultModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench watch result model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_watch_result_model_set_selected(UmiDebugWorkbenchWatchResultModel *model, bool selected);
/**
 * Provide the debug workbench watch result model set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_watch_result_model_set_enabled(UmiDebugWorkbenchWatchResultModel *model, bool enabled);
/**
 * Check that debug workbench watch result model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_watch_result_model_valid(const UmiDebugWorkbenchWatchResultModel *model);

#ifdef __cplusplus
}
#endif
#endif
