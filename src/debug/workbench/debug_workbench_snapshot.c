/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workbench_snapshot.c
 *
 * PURPOSE:
 *   Capture the aggregate debugger workbench selection and visible-state summary.
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
#include "umicom/debug/workbench/debug_workbench_snapshot.h"
#include <string.h>

void umi_debug_workbench_debug_workbench_snapshot_init(UmiDebugWorkbenchDebugWorkbenchSnapshot *model)
{ if (model != NULL) { memset(model, 0, sizeof *model); model->generation = 1U; } }

UmiStatus umi_debug_workbench_debug_workbench_snapshot_capture(UmiDebugWorkbenchDebugWorkbenchSnapshot *model, const char *session_id, const char *item_id, uint32_t sessions, uint32_t breakpoints, uint32_t threads, uint32_t watches)
{
    if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_debug_workbench_copy_text(model->active_session_id, sizeof model->active_session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (item_id != NULL && item_id[0] != '\0' && umi_debug_workbench_copy_text(model->active_item_id, sizeof model->active_item_id, item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->session_count = sessions; model->breakpoint_count = breakpoints; model->thread_count = threads; model->watch_count = watches; model->generation++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_workbench_snapshot_valid(const UmiDebugWorkbenchDebugWorkbenchSnapshot *model)
{ return model != NULL && model->generation > 0U && (model->active_session_id[0] == '\0' || umi_debug_workbench_id_valid(model->active_session_id)); }
