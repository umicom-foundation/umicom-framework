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

/*
 * Initialise debug workbench debug launch request from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_launch_request_init(UmiDebugWorkbenchDebugLaunchRequest *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug launch request set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_state(UmiDebugWorkbenchDebugLaunchRequest *model, uint32_t state, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find debug workbench debug launch request set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_selected(UmiDebugWorkbenchDebugLaunchRequest *model, bool selected)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug launch request set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_enabled(UmiDebugWorkbenchDebugLaunchRequest *model, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug launch request satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_launch_request_valid(const UmiDebugWorkbenchDebugLaunchRequest *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
