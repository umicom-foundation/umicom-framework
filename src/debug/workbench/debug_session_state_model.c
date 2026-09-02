/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_session_state_model.c
 *
 * PURPOSE:
 *   Track debugger lifecycle state and enforce legal high-level phase transitions.
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
#include "umicom/debug/workbench/debug_session_state_model.h"
#include <string.h>

/*
 * Initialise debug workbench debug session state model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_init(UmiDebugWorkbenchDebugSessionStateModel *model, const char *session_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(model->session_id, sizeof model->session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->phase = UMI_DEBUG_WORKBENCH_SESSION_IDLE;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug session state model transition operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_transition(UmiDebugWorkbenchDebugSessionStateModel *model, UmiDebugWorkbenchSessionPhase phase)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_session_transition_allowed(model->phase, phase)) return UMI_STATUS_INVALID_STATE;
    model->phase = phase;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug session state model record stop operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_record_stop(UmiDebugWorkbenchDebugSessionStateModel *model)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    model->stop_sequence++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug session state model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_session_state_model_valid(const UmiDebugWorkbenchDebugSessionStateModel *model)
{
    return model != NULL && umi_debug_workbench_id_valid(model->session_id) && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U;
}
