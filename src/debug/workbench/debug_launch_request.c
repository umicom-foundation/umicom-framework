/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_launch_request.c
 *
 * PURPOSE:
 *   Capture a validated workbench launch request before dispatch to the canonical debug runtime.
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
#include "umicom/debug/workbench/debug_launch_request.h"

#include <string.h>

UmiStatus umi_debug_workbench_debug_launch_request_init(UmiDebugWorkbenchDebugLaunchRequest *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_launch_request_set_state(UmiDebugWorkbenchDebugLaunchRequest *model, uint32_t state, uint64_t value)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_launch_request_set_selected(UmiDebugWorkbenchDebugLaunchRequest *model, bool selected)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_launch_request_set_enabled(UmiDebugWorkbenchDebugLaunchRequest *model, bool enabled)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_launch_request_valid(const UmiDebugWorkbenchDebugLaunchRequest *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
