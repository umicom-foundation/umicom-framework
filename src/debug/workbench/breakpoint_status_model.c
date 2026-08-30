/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/breakpoint_status_model.c
 *
 * PURPOSE:
 *   Represent aggregate breakpoint verification and enablement status.
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
#include "umicom/debug/workbench/breakpoint_status_model.h"

#include <string.h>

UmiStatus umi_debug_workbench_breakpoint_status_model_init(UmiDebugWorkbenchBreakpointStatusModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_breakpoint_status_model_set_state(UmiDebugWorkbenchBreakpointStatusModel *model, uint32_t state, uint64_t value)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_breakpoint_status_model_set_selected(UmiDebugWorkbenchBreakpointStatusModel *model, bool selected)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_breakpoint_status_model_set_enabled(UmiDebugWorkbenchBreakpointStatusModel *model, bool enabled)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_breakpoint_status_model_valid(const UmiDebugWorkbenchBreakpointStatusModel *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
