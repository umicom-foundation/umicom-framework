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

typedef struct UmiDebugWorkbenchWatchResultModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchWatchResultModel;

UmiStatus umi_debug_workbench_watch_result_model_init(UmiDebugWorkbenchWatchResultModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_watch_result_model_set_state(UmiDebugWorkbenchWatchResultModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_watch_result_model_set_selected(UmiDebugWorkbenchWatchResultModel *model, bool selected);
UmiStatus umi_debug_workbench_watch_result_model_set_enabled(UmiDebugWorkbenchWatchResultModel *model, bool enabled);
int umi_debug_workbench_watch_result_model_valid(const UmiDebugWorkbenchWatchResultModel *model);

#ifdef __cplusplus
}
#endif
#endif
