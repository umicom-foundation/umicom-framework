/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_session_restore.c
 *
 * PURPOSE:
 *   Describe restoration intent and validation state for a saved debug session.
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
#include "umicom/debug/workbench/debug_session_restore.h"

#include <string.h>

/*
 * Initialise debug workbench debug session restore from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_session_restore_init(UmiDebugWorkbenchDebugSessionRestore *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
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
 * Provide the debug workbench debug session restore set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_restore_set_state(UmiDebugWorkbenchDebugSessionRestore *model, uint32_t state, uint64_t value)
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
 * Find debug workbench debug session restore set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_session_restore_set_selected(UmiDebugWorkbenchDebugSessionRestore *model, bool selected)
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
 * Provide the debug workbench debug session restore set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_restore_set_enabled(UmiDebugWorkbenchDebugSessionRestore *model, bool enabled)
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
 * Check that debug workbench debug session restore satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_session_restore_valid(const UmiDebugWorkbenchDebugSessionRestore *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
